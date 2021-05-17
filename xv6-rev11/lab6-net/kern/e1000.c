#include <kern/e1000.h>
#include <kern/pmap.h>
#include <kern/pci.h>

// LAB 6: Your driver code here
#define E1000_LOCATE(offset)  (offset >> 2)
volatile uint32_t *e1000;

int
pci_e1000_attach(struct pci_func *pcif) {
    pci_func_enable(pcif);
    e1000 = mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);
    cprintf("device status:[%08x]\n", e1000[E1000_LOCATE(E1000_DEVICE_STATUS)]);
    return 0;
}