#include <kern/e1000.h>
#include <kern/pmap.h>
#include <kern/pci.h>

// LAB 6: Your driver code here
volatile void *bar_va;

int
e1000_attachfn(struct pci_func *pcif) {
    pci_func_enable(pcif);
    bar_va = mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);
    cprintf("device status:[%08x]\n", (uint32_t *) E1000REG(E1000_STATUS));
    return 0;
}