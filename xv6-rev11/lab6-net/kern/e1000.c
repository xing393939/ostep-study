#include <kern/e1000.h>
#include <kern/pmap.h>
#include <kern/pci.h>

/* 网卡设备MMIO的基址va */
volatile void *bar_va;
/* 发送包 */
struct e1000_tdh *tdh;
struct e1000_tdt *tdt;
struct e1000_tx_desc tx_desc_array[TXDESCS];
char tx_buffer_array[TXDESCS][TX_PKT_SIZE];
/* 接收包 */
struct e1000_rdh *rdh;
struct e1000_rdt *rdt;
struct e1000_rx_desc rx_desc_array[RXDESCS];
char rx_buffer_array[RXDESCS][RX_PKT_SIZE];
/* 网卡地址 */
uint32_t E1000_MAC[6] = {0x52, 0x54, 0x00, 0x12, 0x34, 0x56};

int
e1000_attachfn(struct pci_func *pcif) {
    pci_func_enable(pcif);
    bar_va = mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);
    uint32_t *status_reg = E1000REG(E1000_STATUS);
    cprintf("device status:[%08x]\n", *status_reg);
    return 0;
}

static void
e1000_transmit_init()
{
    int i;
    for (i = 0; i < TXDESCS; i++) {
        tx_desc_array[i].addr = PADDR(tx_buffer_array[i]);
        tx_desc_array[i].cmd = 0;
        tx_desc_array[i].status |= E1000_TXD_STAT_DD;
    }
    //TDLEN register
    struct e1000_tdlen *tdlen = (struct e1000_tdlen *)E1000REG(E1000_TDLEN);
    tdlen->len = TXDESCS;

    //TDBAL register
    uint32_t *tdbal = (uint32_t *)E1000REG(E1000_TDBAL);
    *tdbal = PADDR(tx_desc_array);

    //TDBAH regsiter
    uint32_t *tdbah = (uint32_t *)E1000REG(E1000_TDBAH);
    *tdbah = 0;

    //TDH register, should be init 0
    tdh = (struct e1000_tdh *)E1000REG(E1000_TDH);
    tdh->tdh = 0;

    //TDT register, should be init 0
    tdt = (struct e1000_tdt *)E1000REG(E1000_TDT);
    tdt->tdt = 0;

    //TCTL register
    struct e1000_tctl *tctl = (struct e1000_tctl *)E1000REG(E1000_TCTL);
    tctl->en = 1;
    tctl->psp = 1;
    tctl->ct = 0x10;
    tctl->cold = 0x40;

    //TIPG register
    struct e1000_tipg *tipg = (struct e1000_tipg *)E1000REG(E1000_TIPG);
    tipg->ipgt = 10;
    tipg->ipgr1 = 4;
    tipg->ipgr2 = 6;
}