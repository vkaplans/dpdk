/*
 *   BSD LICENSE
 *
 *   Copyright(c) 2015 RehiveTech. All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of RehiveTech nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RTE_CPUFLAGS_ARM32_H_
#define _RTE_CPUFLAGS_ARM32_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <elf.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "generic/rte_cpuflags.h"

extern const struct feature_entry rte_cpu_feature_table[];

#ifndef AT_HWCAP
#define AT_HWCAP 16
#endif

#ifndef AT_HWCAP2
#define AT_HWCAP2 26
#endif

#ifndef AT_PLATFORM
#define AT_PLATFORM 15
#endif

/* software based registers */
enum cpu_register_t {
	REG_HWCAP = 0,
	REG_HWCAP2,
	REG_PLATFORM,
};

/**
 * Enumeration of all CPU features supported
 */
enum rte_cpu_flag_t {
	RTE_CPUFLAG_SWP = 0,
	RTE_CPUFLAG_HALF,
	RTE_CPUFLAG_THUMB,
	RTE_CPUFLAG_A26BIT,
	RTE_CPUFLAG_FAST_MULT,
	RTE_CPUFLAG_FPA,
	RTE_CPUFLAG_VFP,
	RTE_CPUFLAG_EDSP,
	RTE_CPUFLAG_JAVA,
	RTE_CPUFLAG_IWMMXT,
	RTE_CPUFLAG_CRUNCH,
	RTE_CPUFLAG_THUMBEE,
	RTE_CPUFLAG_NEON,
	RTE_CPUFLAG_VFPv3,
	RTE_CPUFLAG_VFPv3D16,
	RTE_CPUFLAG_TLS,
	RTE_CPUFLAG_VFPv4,
	RTE_CPUFLAG_IDIVA,
	RTE_CPUFLAG_IDIVT,
	RTE_CPUFLAG_VFPD32,
	RTE_CPUFLAG_LPAE,
	RTE_CPUFLAG_EVTSTRM,
	RTE_CPUFLAG_AES,
	RTE_CPUFLAG_PMULL,
	RTE_CPUFLAG_SHA1,
	RTE_CPUFLAG_SHA2,
	RTE_CPUFLAG_CRC32,
	RTE_CPUFLAG_V7L,
	/* The last item */
	RTE_CPUFLAG_NUMFLAGS,/**< This should always be the last! */
};

/*
 * Read AUXV software register and get cpu features for ARM
 */
static inline void
rte_cpu_get_features(__attribute__((unused)) uint32_t leaf,
	__attribute__((unused)) uint32_t subleaf, cpuid_registers_t out)
{
	int auxv_fd;
	Elf32_auxv_t auxv;

	auxv_fd = open("/proc/self/auxv", O_RDONLY);
	assert(auxv_fd);
	while (read(auxv_fd, &auxv,
		sizeof(Elf32_auxv_t)) == sizeof(Elf32_auxv_t)) {
		if (auxv.a_type == AT_HWCAP)
			out[REG_HWCAP] = auxv.a_un.a_val;
		else if (auxv.a_type == AT_HWCAP2)
			out[REG_HWCAP2] = auxv.a_un.a_val;
		else if (auxv.a_type == AT_PLATFORM) {
			if (!strcmp((const char *)auxv.a_un.a_val, "v7l"))
				out[REG_PLATFORM] = 0x0001;
		}
	}
}

/*
 * Checks if a particular flag is available on current machine.
 */
static inline int
rte_cpu_get_flag_enabled(enum rte_cpu_flag_t feature)
{
	const struct feature_entry *feat;
	cpuid_registers_t regs = {0};

	if (feature >= RTE_CPUFLAG_NUMFLAGS)
		/* Flag does not match anything in the feature tables */
		return -ENOENT;

	feat = &rte_cpu_feature_table[feature];

	if (!feat->leaf)
		/* This entry in the table wasn't filled out! */
		return -EFAULT;

	/* get the cpuid leaf containing the desired feature */
	rte_cpu_get_features(feat->leaf, feat->subleaf, regs);

	/* check if the feature is enabled */
	return (regs[feat->reg] >> feat->bit) & 1;
}

#ifdef __cplusplus
}
#endif

#endif /* _RTE_CPUFLAGS_ARM32_H_ */
