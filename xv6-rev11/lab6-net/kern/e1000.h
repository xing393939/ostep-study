#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#include <kern/pci.h>

int
e1000_attachfn(struct pci_func *pcif);

static void
e1000_transmit_init();

int
e1000_transmit(void *data, size_t len);

static void
e1000_receive_init();

int
e1000_receive(void *addr, size_t *len);

#define E1000_VENDER_ID_82540EM 0x8086  /* 供应商ID */
#define E1000_DEV_ID_82540EM 0x100E     /* 设备ID */

#define TXDESCS 32              /* 发送队列长度 */
#define TX_PKT_SIZE 1518
#define E1000_STATUS   0x00008  /* Device Status - RO */
#define E1000_TCTL     0x00400  /* TX Control - RW */
#define E1000_TIPG     0x00410  /* TX Inter-packet gap -RW */
#define E1000_TDBAL    0x03800  /* TX Descriptor Base Address Low - RW */
#define E1000_TDBAH    0x03804  /* TX Descriptor Base Address High - RW */
#define E1000_TDLEN    0x03808  /* TX Descriptor Length - RW */
#define E1000_TDH      0x03810  /* TX Descriptor Head - RW */
#define E1000_TDT      0x03818  /* TX Descripotr Tail - RW */
#define E1000_TXD_STAT_DD    0x00000001 /* Descriptor Done */
#define E1000_TXD_CMD_EOP    0x00000001 /* End of Packet */
#define E1000_TXD_CMD_RS     0x00000008 /* Report Status */

#define RXDESCS 128
#define RX_PKT_SIZE 1518
#define E1000_RCTL 0x00100
#define E1000_RCTL_EN     0x00000002    /* enable */
#define E1000_RCTL_BAM    0x00008000    /* broadcast enable */
#define E1000_RCTL_SECRC  0x04000000    /* Strip Ethernet CRC */
#define E1000_RDBAL    0x02800  /* RX Descriptor Base Address Low - RW */
#define E1000_RDBAH    0x02804  /* RX Descriptor Base Address High - RW */
#define E1000_RDLEN    0x02808  /* RX Descriptor Length - RW */
#define E1000_RDH      0x02810  /* RX Descriptor Head - RW */
#define E1000_RDT      0x02818  /* RX Descriptor Tail - RW */
#define E1000_RA       0x05400  /* Receive Address - RW Array */
#define E1000_RAH_AV   0x80000000        /* Receive descriptor valid */
#define E1000_RXD_STAT_DD       0x01    /* Descriptor Done */
#define E1000_RXD_STAT_EOP      0x02    /* End of Packet */

/* 通过网卡设备MMIO的基址bar_va、偏移字节offset得到寄存器的va */
#define E1000REG(offset) (void *) (bar_va + offset)

/* T CTL（发送控制器【Controller】） */
struct e1000_tctl {
    uint32_t rsv1:   1;
    uint32_t en:     1;
    uint32_t rsv2:   1;
    uint32_t psp:    1;
    uint32_t ct:     8;
    uint32_t cold:   10;
    uint32_t swxoff: 1;
    uint32_t rsv3:   1;
    uint32_t rtlc:   1;
    uint32_t nrtu:   1;
    uint32_t rsv4:   6;
};
/* T IPG（发送的帧间间距【inter-packet gap】） */
struct e1000_tipg {
    uint32_t ipgt:   10;
    uint32_t ipgr1:  10;
    uint32_t ipgr2:  10;
    uint32_t rsv:    2;
};
/* td的尾指针 */
struct e1000_tdt {
    uint16_t tdt;
    uint16_t rsv;
};
/* td的长度 */
struct e1000_tdlen {
    uint32_t zero: 7;
    uint32_t len:  13;
    uint32_t rsv:  12;
};
/* td的头指针 */
struct e1000_tdh {
    uint16_t tdh;
    uint16_t rsv;
};

#endif
