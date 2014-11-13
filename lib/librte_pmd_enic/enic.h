/*
 * Copyright 2008-2014 Cisco Systems, Inc.  All rights reserved.
 * Copyright 2007 Nuova Systems, Inc.  All rights reserved.
 *
 * Copyright (c) 2014, Cisco Systems, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ident "$Id$"

#ifndef _ENIC_H_
#define _ENIC_H_

#include "vnic_enet.h"
#include "vnic_dev.h"
#include "vnic_wq.h"
#include "vnic_rq.h"
#include "vnic_cq.h"
#include "vnic_intr.h"
#include "vnic_stats.h"
#include "vnic_nic.h"
#include "vnic_rss.h"
#include "enic_res.h"

#define DRV_NAME		"enic_pmd"
#define DRV_DESCRIPTION		"Cisco VIC Ethernet NIC Poll-mode Driver"
#define DRV_VERSION		"1.0.0.4"
#define DRV_COPYRIGHT		"Copyright 2008-2014 Cisco Systems, Inc"

#define ENIC_WQ_MAX		8
#define ENIC_RQ_MAX		8
#define ENIC_CQ_MAX		(ENIC_WQ_MAX + ENIC_RQ_MAX)
#define ENIC_INTR_MAX		(ENIC_CQ_MAX + 2)

#define VLAN_ETH_HLEN           18

#define ENICPMD_SETTING(enic, f) ((enic->config.flags & VENETF_##f) ? 1 : 0)

#define ENICPMD_BDF_LENGTH      13   /* 0000:00:00.0'\0' */
#define PKT_TX_TCP_UDP_CKSUM    0x6000
#define ENIC_CALC_IP_CKSUM      1
#define ENIC_CALC_TCP_UDP_CKSUM 2
#define ENIC_MAX_MTU            9000
#define PAGE_SIZE               4096
#define PAGE_ROUND_UP(x) \
	((((unsigned long)(x)) + PAGE_SIZE-1) & (~(PAGE_SIZE-1)))

#define ENICPMD_VFIO_PATH          "/dev/vfio/vfio"
/*#define ENIC_DESC_COUNT_MAKE_ODD (x) do{if ((~(x)) & 1) { (x)--; } }while(0)*/

#define PCI_DEVICE_ID_CISCO_VIC_ENET         0x0043  /* ethernet vnic */
#define PCI_DEVICE_ID_CISCO_VIC_ENET_VF      0x0071  /* enet SRIOV VF */


#define ENICPMD_FDIR_MAX           64

struct enic_fdir_node {
	struct rte_fdir_filter filter;
	u16 fltr_id;
	u16 rq_index;
};

struct enic_fdir {
	struct rte_eth_fdir stats;
	struct rte_hash *hash;
	struct enic_fdir_node *nodes[ENICPMD_FDIR_MAX];
};

/* Per-instance private data structure */
struct enic {
	struct enic *next;
	struct rte_pci_device *pdev;
	struct vnic_enet_config config;
	struct vnic_dev_bar bar0;
	struct vnic_dev *vdev;

	struct rte_eth_dev *rte_dev;
	struct enic_fdir fdir;
	char bdf_name[ENICPMD_BDF_LENGTH];
	int dev_fd;
	int iommu_group_fd;
	int iommu_groupid;
	int eventfd;
	u_int8_t mac_addr[ETH_ALEN];
	pthread_t err_intr_thread;
	int promisc;
	int allmulti;
	int ig_vlan_strip_en;
	int link_status;
	u8 hw_ip_checksum;

	unsigned int flags;
	unsigned int priv_flags;

	/* work queue */
	struct vnic_wq wq[ENIC_WQ_MAX];
	unsigned int wq_count;

	/* receive queue */
	struct vnic_rq rq[ENIC_RQ_MAX];
	unsigned int rq_count;

	/* completion queue */
	struct vnic_cq cq[ENIC_CQ_MAX];
	unsigned int cq_count;

	/* interrupt resource */
	struct vnic_intr intr;
	unsigned int intr_count;
};

static inline unsigned int enic_cq_rq(struct enic *enic, unsigned int rq)
{
	return rq;
}

static inline unsigned int enic_cq_wq(struct enic *enic, unsigned int wq)
{
	return enic->rq_count + wq;
}

static inline unsigned int enic_msix_err_intr(struct enic *enic)
{
	return 0;
}

static inline struct enic *pmd_priv(struct rte_eth_dev *eth_dev)
{
	return (struct enic *)eth_dev->data->dev_private;
}

#endif /* _ENIC_H_ */