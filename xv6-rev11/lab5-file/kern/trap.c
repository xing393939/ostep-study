#include <inc/mmu.h>
#include <inc/x86.h>
#include <inc/assert.h>

#include <kern/pmap.h>
#include <kern/trap.h>
#include <kern/console.h>
#include <kern/monitor.h>
#include <kern/env.h>
#include <kern/syscall.h>
#include <kern/sched.h>
#include <kern/kclock.h>
#include <kern/picirq.h>
#include <kern/cpu.h>
#include <kern/spinlock.h>

static struct Taskstate ts;

/* For debugging, so print_trapframe can distinguish between printing
 * a saved trapframe and printing the current trapframe and print some
 * additional information in the latter case.
 */
static struct Trapframe *last_tf;

/* Interrupt descriptor table.  (Must be built at run time because
 * shifted function addresses can't be represented in relocation records.)
 */
struct Gatedesc idt[256] = { { 0 } };
struct Pseudodesc idt_pd = {
	sizeof(idt) - 1, (uint32_t) idt
};


static const char *trapname(int trapno)
{
	static const char * const excnames[] = {
		"Divide error",
		"Debug",
		"Non-Maskable Interrupt",
		"Breakpoint",
		"Overflow",
		"BOUND Range Exceeded",
		"Invalid Opcode",
		"Device Not Available",
		"Double Fault",
		"Coprocessor Segment Overrun",
		"Invalid TSS",
		"Segment Not Present",
		"Stack Fault",
		"General Protection",
		"Page Fault",
		"(unknown trap)",
		"x87 FPU Floating-Point Error",
		"Alignment Check",
		"Machine-Check",
		"SIMD Floating-Point Exception"
	};

	if (trapno < ARRAY_SIZE(excnames))
		return excnames[trapno];
	if (trapno == T_SYSCALL)
		return "System call";
	if (trapno >= IRQ_OFFSET && trapno < IRQ_OFFSET + 16)
		return "Hardware Interrupt";
	return "(unknown trap)";
}


void
trap_init(void)
{
	extern struct Segdesc gdt[];

	// LAB 3: Your code here.
    void T_DIVIDE_handler();
    void T_DEBUG_handler();
    void T_NMI_handler();
    void T_BRKPT_handler();
    void T_OFLOW_handler();
    void T_BOUND_handler();
    void T_ILLOP_handler();
    void T_DEVICE_handler();
    void T_DBLFLT_handler();
    void T_TSS_handler();
    void T_SEGNP_handler();
    void T_STACK_handler();
    void T_GPFLT_handler();
    void T_PGFLT_handler();
    void T_FPERR_handler();
    void T_ALIGN_handler();
    void T_MCHK_handler();
    void T_SIMDERR_handler();
    void T_SYSCALL_handler();
    SETGATE(idt[T_DIVIDE], 0, GD_KT, T_DIVIDE_handler, 0);
    SETGATE(idt[T_DEBUG], 0, GD_KT, T_DEBUG_handler, 0);
    SETGATE(idt[T_NMI], 0, GD_KT, T_NMI_handler, 0);
    SETGATE(idt[T_BRKPT], 1, GD_KT, T_BRKPT_handler, 3);
    SETGATE(idt[T_OFLOW], 1, GD_KT, T_OFLOW_handler, 0);
    SETGATE(idt[T_BOUND], 0, GD_KT, T_BOUND_handler, 0);
    SETGATE(idt[T_ILLOP], 0, GD_KT, T_ILLOP_handler, 0);
    SETGATE(idt[T_DEVICE], 0, GD_KT, T_DEVICE_handler, 0);
    SETGATE(idt[T_DBLFLT], 0, GD_KT, T_DBLFLT_handler, 0);
    SETGATE(idt[T_TSS], 0, GD_KT, T_TSS_handler, 0);
    SETGATE(idt[T_SEGNP], 0, GD_KT, T_SEGNP_handler, 0);
    SETGATE(idt[T_STACK], 0, GD_KT, T_STACK_handler, 0);
    SETGATE(idt[T_GPFLT], 0, GD_KT, T_GPFLT_handler, 0);
    SETGATE(idt[T_PGFLT], 0, GD_KT, T_PGFLT_handler, 0);
    SETGATE(idt[T_FPERR], 0, GD_KT, T_FPERR_handler, 0);
    SETGATE(idt[T_ALIGN], 0, GD_KT, T_ALIGN_handler, 0);
    SETGATE(idt[T_MCHK], 0, GD_KT, T_MCHK_handler, 0);
    SETGATE(idt[T_SIMDERR], 0, GD_KT, T_SIMDERR_handler, 0);
    SETGATE(idt[T_SYSCALL], 0, GD_KT, T_SYSCALL_handler, 3);

    // IRQ
    void IRQ_timerHandler();
    void IRQ_kbdHandler();
    void IRQ_serialHandler();
    void IRQ_spuriousHandler();
    void IRQ_ideHandler();
    void IRQ_errorHandler();
    SETGATE(idt[IRQ_OFFSET + IRQ_TIMER],    0, GD_KT, IRQ_timerHandler,    0);
    SETGATE(idt[IRQ_OFFSET + IRQ_KBD],      0, GD_KT, IRQ_kbdHandler,      0);
    SETGATE(idt[IRQ_OFFSET + IRQ_SERIAL],   0, GD_KT, IRQ_serialHandler,   0);
    SETGATE(idt[IRQ_OFFSET + IRQ_SPURIOUS], 0, GD_KT, IRQ_spuriousHandler, 0);
    SETGATE(idt[IRQ_OFFSET + IRQ_IDE],      0, GD_KT, IRQ_ideHandler,      0);
    SETGATE(idt[IRQ_OFFSET + IRQ_ERROR],    0, GD_KT, IRQ_errorHandler,    0);

	// Per-CPU setup 
	trap_init_percpu();
}

// Initialize and load the per-CPU TSS and IDT
void
trap_init_percpu(void)
{
	// The example code here sets up the Task State Segment (TSS) and
	// the TSS descriptor for CPU 0. But it is incorrect if we are
	// running on other CPUs because each CPU has its own kernel stack.
	// Fix the code so that it works for all CPUs.
	//
	// Hints:
	//   - The macro "thiscpu" always refers to the current CPU's
	//     struct CpuInfo;
	//   - The ID of the current CPU is given by cpunum() or
	//     thiscpu->cpu_id;
	//   - Use "thiscpu->cpu_ts" as the TSS for the current CPU,
	//     rather than the global "ts" variable;
	//   - Use gdt[(GD_TSS0 >> 3) + i] for CPU i's TSS descriptor;
	//   - You mapped the per-CPU kernel stacks in mem_init_mp()
	//   - Initialize cpu_ts.ts_iomb to prevent unauthorized environments
	//     from doing IO (0 is not the correct value!)
	//
	// ltr sets a 'busy' flag in the TSS selector, so if you
	// accidentally load the same TSS on more than one CPU, you'll
	// get a triple fault.  If you set up an individual CPU's TSS
	// wrong, you may not get a fault until you try to return from
	// user space on that CPU.
	//
	// LAB 4: Your code here:
    thiscpu->cpu_ts.ts_esp0 = KSTACKTOP - thiscpu->cpu_id * (KSTKSIZE + KSTKGAP);
    thiscpu->cpu_ts.ts_ss0 = GD_KD;
    thiscpu->cpu_ts.ts_iomb = sizeof(struct Taskstate);

    // Initialize the TSS slot of the gdt.
    gdt[(GD_TSS0 >> 3) + thiscpu->cpu_id] = SEG16(STS_T32A, (uint32_t)(&(thiscpu->cpu_ts)), sizeof(struct Taskstate) - 1, 0);
    gdt[(GD_TSS0 >> 3) + thiscpu->cpu_id].sd_s = 0;

    // Load the TSS selector (like other segment selectors, the
    // bottom three bits are special; we leave them 0)
    ltr(GD_TSS0 + 8 * thiscpu->cpu_id); // low 3 bits was special.

    // Load the IDT
    lidt(&idt_pd);
}

void
print_trapframe(struct Trapframe *tf)
{
	cprintf("TRAP frame at %p from CPU %d\n", tf, cpunum());
	print_regs(&tf->tf_regs);
	cprintf("  es   0x----%04x\n", tf->tf_es);
	cprintf("  ds   0x----%04x\n", tf->tf_ds);
	cprintf("  trap 0x%08x %s\n", tf->tf_trapno, trapname(tf->tf_trapno));
	// If this trap was a page fault that just happened
	// (so %cr2 is meaningful), print the faulting linear address.
	if (tf == last_tf && tf->tf_trapno == T_PGFLT)
		cprintf("  cr2  0x%08x\n", rcr2());
	cprintf("  err  0x%08x", tf->tf_err);
	// For page faults, print decoded fault error code:
	// U/K=fault occurred in user/kernel mode
	// W/R=a write/read caused the fault
	// PR=a protection violation caused the fault (NP=page not present).
	if (tf->tf_trapno == T_PGFLT)
		cprintf(" [%s, %s, %s]\n",
			tf->tf_err & 4 ? "user" : "kernel",
			tf->tf_err & 2 ? "write" : "read",
			tf->tf_err & 1 ? "protection" : "not-present");
	else
		cprintf("\n");
	cprintf("  eip  0x%08x\n", tf->tf_eip);
	cprintf("  cs   0x----%04x\n", tf->tf_cs);
	cprintf("  flag 0x%08x\n", tf->tf_eflags);
	if ((tf->tf_cs & 3) != 0) {
		cprintf("  esp  0x%08x\n", tf->tf_esp);
		cprintf("  ss   0x----%04x\n", tf->tf_ss);
	}
}

void
print_regs(struct PushRegs *regs)
{
	cprintf("  edi  0x%08x\n", regs->reg_edi);
	cprintf("  esi  0x%08x\n", regs->reg_esi);
	cprintf("  ebp  0x%08x\n", regs->reg_ebp);
	cprintf("  oesp 0x%08x\n", regs->reg_oesp);
	cprintf("  ebx  0x%08x\n", regs->reg_ebx);
	cprintf("  edx  0x%08x\n", regs->reg_edx);
	cprintf("  ecx  0x%08x\n", regs->reg_ecx);
	cprintf("  eax  0x%08x\n", regs->reg_eax);
}

static void
trap_dispatch(struct Trapframe *tf)
{
	// Handle processor exceptions.
	// LAB 3: Your code here.
    switch(tf->tf_trapno) {
        case T_PGFLT:
            page_fault_handler(tf);
            return;
        case T_BRKPT:
            monitor(tf);
            return;
		case T_SYSCALL:
			if (tf->tf_regs.reg_eax >= NSYSCALLS)
				return;
			tf->tf_regs.reg_eax = syscall(
				tf->tf_regs.reg_eax,
				tf->tf_regs.reg_edx,
				tf->tf_regs.reg_ecx,
				tf->tf_regs.reg_ebx,
				tf->tf_regs.reg_edi,
				tf->tf_regs.reg_esi
			);
			return;
        default:
            break;
    }

	// Handle spurious interrupts
	// The hardware sometimes raises these because of noise on the
	// IRQ line or other reasons. We don't care.
	if (tf->tf_trapno == IRQ_OFFSET + IRQ_SPURIOUS) {
		cprintf("Spurious interrupt on irq 7\n");
		print_trapframe(tf);
		return;
	}

	// Handle clock interrupts. Don't forget to acknowledge the
	// interrupt using lapic_eoi() before calling the scheduler!
	// LAB 4: Your code here.
    if (tf->tf_trapno == (IRQ_OFFSET + IRQ_TIMER)) {
        lapic_eoi();
        sched_yield();
    }

	// Handle keyboard and serial interrupts.
	// LAB 5: Your code here.
    if (tf->tf_trapno == (IRQ_OFFSET + IRQ_KBD)) {
        kbd_intr();
        return;
    }
    if (tf->tf_trapno == (IRQ_OFFSET + IRQ_SERIAL)) {
        serial_intr();
        return;
    }

	// Unexpected trap: The user process or the kernel has a bug.
	print_trapframe(tf);
	if (tf->tf_cs == GD_KT)
		panic("unhandled trap in kernel");
	else {
		env_destroy(curenv);
		return;
	}
}

void
trap(struct Trapframe *tf)
{
	// The environment may have set DF and some versions
	// of GCC rely on DF being clear
	asm volatile("cld" ::: "cc");

	// Halt the CPU if some other CPU has called panic()
	extern char *panicstr;
	if (panicstr)
		asm volatile("hlt");

	// Re-acqurie the big kernel lock if we were halted in
	// sched_yield()
	if (xchg(&thiscpu->cpu_status, CPU_STARTED) == CPU_HALTED)
		lock_kernel();
	// Check that interrupts are disabled.  If this assertion
	// fails, DO NOT be tempted to fix it by inserting a "cli" in
	// the interrupt path.
	assert(!(read_eflags() & FL_IF));

	if ((tf->tf_cs & 3) == 3) {
		// Trapped from user mode.
		// Acquire the big kernel lock before doing any
		// serious kernel work.
		// LAB 4: Your code here.
        lock_kernel();
		assert(curenv);

		// Garbage collect if current enviroment is a zombie
		if (curenv->env_status == ENV_DYING) {
			env_free(curenv);
			curenv = NULL;
			sched_yield();
		}

		// Copy trap frame (which is currently on the stack)
		// into 'curenv->env_tf', so that running the environment
		// will restart at the trap point.
		curenv->env_tf = *tf;
		// The trapframe on the stack should be ignored from here on.
		tf = &curenv->env_tf;
	}

	// Record that tf is the last real trapframe so
	// print_trapframe can print some additional information.
	last_tf = tf;

	// Dispatch based on what type of trap occurred
	trap_dispatch(tf);

	// If we made it to this point, then no other environment was
	// scheduled, so we should return to the current environment
	// if doing so makes sense.
	if (curenv && curenv->env_status == ENV_RUNNING)
		env_run(curenv);
	else
		sched_yield();
}


void
page_fault_handler(struct Trapframe *tf)
{
	uint32_t fault_va;

	// Read processor's CR2 register to find the faulting address
	fault_va = rcr2();

	// Handle kernel-mode page faults.

	// LAB 3: Your code here.
    // 怎么判断是内核模式， CPL位
    if((tf->tf_cs & 3) == 0) {
        panic("page_fault in kernel mode, fault address %d\n", fault_va);
    }

    // We've already handled kernel-mode exceptions, so if we get here,
    // the page fault happened in user mode.

	// Call the environment's page fault upcall, if one exists.  Set up a
	// page fault stack frame on the user exception stack (below
	// UXSTACKTOP), then branch to curenv->env_pgfault_upcall.
	//
	// The page fault upcall might cause another page fault, in which case
	// we branch to the page fault upcall recursively, pushing another
	// page fault stack frame on top of the user exception stack.
	//
	// It is convenient for our code which returns from a page fault
	// (lib/pfentry.S) to have one word of scratch space at the top of the
	// trap-time stack; it allows us to more easily restore the eip/esp. In
	// the non-recursive case, we don't have to worry about this because
	// the top of the regular user stack is free.  In the recursive case,
	// this means we have to leave an extra word between the current top of
	// the exception stack and the new stack frame because the exception
	// stack _is_ the trap-time stack.
	//
	// If there's no page fault upcall, the environment didn't allocate a
	// page for its exception stack or can't write to it, or the exception
	// stack overflows, then destroy the environment that caused the fault.
	// Note that the grade script assumes you will first check for the page
	// fault upcall and print the "user fault va" message below if there is
	// none.  The remaining three checks can be combined into a single test.
	//
	// Hints:
	//   user_mem_assert() and env_run() are useful here.
	//   To change what the user environment runs, modify 'curenv->env_tf'
	//   (the 'tf' variable points at 'curenv->env_tf').

	// LAB 4: Your code here.
    if (curenv->env_pgfault_upcall) {
        // exist env_pgfault_upcall.
        struct UTrapframe *utf;
        if (tf->tf_esp >= (UXSTACKTOP - PGSIZE) && tf->tf_esp <= (UXSTACKTOP - 1)) {
            // the user environment is already running on the exception stack when an exception occurs.
            // Thus,we should start the new stack frame just under the tf->tf_esp.We have to push a word
            // at the top of the trap-time stack as a scratch space.
            utf = (struct UTrapframe *)(tf->tf_esp - 4 - sizeof(struct UTrapframe));
        } else {
            utf = (struct UTrapframe *)(UXSTACKTOP- sizeof(struct UTrapframe));
        }
        // 1.We have to check whether the environment allocate a page with permissions PTE_W for its exception
        // stack and whether the exception stack overflows.
        // 2.Set size to 1 is enough here,because the user environment exception stack's size is only on page,
        // user_mem_assert function will definitely fail as long as the utf's virtual address is already lower
        // than UXSTACKTOP-PGSIZE.Actually,any value between 1 and sizeof(struct UTrapFrame) is okay here.
        // 3.if user_mem_assert fail,this function will not return,just invoke env_destroy and sys_yield.
        user_mem_assert(curenv, (void *)utf, 1, PTE_W);
        // now we can construct UTrapFrame.
        utf->utf_esp = tf->tf_esp;
        utf->utf_eflags = tf->tf_eflags;
        utf->utf_eip = tf->tf_eip;
        utf->utf_regs = tf->tf_regs;
        utf->utf_err = tf->tf_err;
        utf->utf_fault_va = fault_va;
        // then branch to curenv->env_pgfault_upcall.
        curenv->env_tf.tf_eip = (uintptr_t)curenv->env_pgfault_upcall;
        // Don't forget set stack pointer.
        curenv->env_tf.tf_esp = (uintptr_t)utf;
        env_run(curenv); // never return.
    }

	// Destroy the environment that caused the fault.
	cprintf("[%08x] user fault va %08x ip %08x\n",
		curenv->env_id, fault_va, tf->tf_eip);
	print_trapframe(tf);
	env_destroy(curenv);
}

