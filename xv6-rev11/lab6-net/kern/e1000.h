#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

int
e1000_attachfn(struct pci_func *pcif);

#define PCI_E1000_VENDOR_ID 0x8086
#define PCI_E1000_DEVICE_ID 0x100E

#endif  // SOL >= 6
