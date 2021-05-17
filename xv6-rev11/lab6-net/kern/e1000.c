#include <kern/e1000.h>
#include <kern/pmap.h>

// LAB 6: Your driver code here
int
e1000_attachfn(struct pci_func *pcif) {
    pci_func_enable(pcif);
    return 0;
}