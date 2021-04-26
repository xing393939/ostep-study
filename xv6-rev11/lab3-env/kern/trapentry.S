/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>



###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

.text

/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */
TRAPHANDLER_NOEC(T_DIVIDE_handler, T_DIVIDE)
TRAPHANDLER_NOEC(T_DEBUG_handler, T_DEBUG)
TRAPHANDLER_NOEC(T_NMI_handler, T_NMI)
TRAPHANDLER_NOEC(T_BRKPT_handler, T_BRKPT)
TRAPHANDLER_NOEC(T_OFLOW_handler, T_OFLOW)
TRAPHANDLER_NOEC(T_BOUND_handler, T_BOUND)
TRAPHANDLER_NOEC(T_ILLOP_handler, T_ILLOP)
TRAPHANDLER_NOEC(T_DEVICE_handler, T_DEVICE)
TRAPHANDLER(T_DBLFLT_handler, T_DBLFLT)
TRAPHANDLER(T_TSS_handler, T_TSS)
TRAPHANDLER(T_SEGNP_handler, T_SEGNP)
TRAPHANDLER(T_STACK_handler, T_STACK)
TRAPHANDLER(T_GPFLT_handler, T_GPFLT)
TRAPHANDLER(T_PGFLT_handler, T_PGFLT)
TRAPHANDLER_NOEC(T_FPERR_handler, T_FPERR)
TRAPHANDLER(T_ALIGN_handler, T_ALIGN)
TRAPHANDLER_NOEC(T_MCHK_handler, T_MCHK)
TRAPHANDLER_NOEC(T_SIMDERR_handler, T_SIMDERR)
TRAPHANDLER_NOEC(T_SYSCALL_handler, T_SYSCALL)


/*
 * Lab 3: Your code here for _alltraps
 */
_alltraps:
    /*
     * 注意这里直接用了pushl前面自动补0
     * 如果要严格的对应
     * - pushw $0
     * - pushw %ds
     * - pushw $0
     * - pushw %es
     */
    pushl %ds
    pushl %es
    pushal
    /*
     * 这里是因为后面要调用trap函数
     * 1.
     *   trap函数的定义是trap(struct Trapframe *tf)
     *   这里还有一个指针
     *   这个时候压入pushl %esp这个寄存器的内容。
     *   也就刚好是真正的指向struct Trapframe这个object的起始地址
     * 2.
     *   如果trap函数的定义是trap(struct Trapframe tfObject)
     *   那么这个pushl %esp是没有必要压进去的
     */
    pushl %esp
    /*然后指向内核数据段
     * 硬件上中断门描述符进来的时候
     * 已经把CPU设置成了GD_KT也就是内核代码段。
     * 这个是硬件操作
     */
    movw $GD_KD, %ax
    movw %ax, %ds
    movw %ax, %es
    call trap
    /* 操作完成之后，
     * 没有必要要按照反方向的顺序返回
     * 因为trap函数最终会走到env_pop_tf()这个函数
     * movl $tf, %esp
     * popal
     * popl %es
     * popl %ds
     * addl $0x08, %esp
     * iret
     */
