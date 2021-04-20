#include<stdio.h>
#include<malloc.h>
#include<unistd.h>

void test_mov() {
    int a = 10, b;
    asm ("movl %1, %%eax;"
         "movl %%eax, %0;"
    :"=r"(b)          /* output 该字段的语法后面会详细说明，此处可无视，下同 */
    :"r"(a)           /* input   */
    :"%eax"           /* clobbered register */
    );
    printf("b=%d\n", b);
}

int main(int argc, char **argv) {
    test_mov();

    return 0;
}

int
mon_backtrace(int argc, char **argv, struct Trapframe *tf) {
    // Your code here.
    uint32_t ebp = read_ebp();
    uint32_t eip = 0;
    struct Eipdebuginfo info;
    #define TO_INT(x)  *((uint32_t*)(x))
    while (ebp) {
        eip = TO_INT((ebp + 4));
        cprintf("ebp %08x  eip %08x  args %08x %08x %08x %08x %08x\n",
                ebp,                /*ebp*/
                eip,                /*eip*/
                TO_INT((ebp + 8)),    /*arg1*/
                TO_INT((ebp + 12)),   /*arg2*/
                TO_INT((ebp + 16)),   /*arg3*/
                TO_INT((ebp + 20)),   /*arg4*/
                TO_INT((ebp + 24)));  /*arg5*/

        if (!debuginfo_eip(eip, &info)) {
            cprintf("%s:%d: %.*s+%d\n",
                    info.eip_file,
                    info.eip_line,
                    info.eip_fn_namelen, info.eip_fn_name,
                    eip - info.eip_fn_addr);
        }
        ebp = TO_INT(ebp);
    }
    return 0;
}