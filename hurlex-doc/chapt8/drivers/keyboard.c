#include "timer.h"
#include "debug.h"
#include "common.h"
#include "idt.h"

#define KBD_BUF_PORT 0x60 // 键盘 buffer 寄存器端口号为 0x60

// 键盘中断处理程序
void keyboard_callback(pt_regs *regs) {
    printk_color(rc_black, rc_red, "keyboard_callback\n");
    inb(KBD_BUF_PORT);
    return;
}

// 键盘初始化
void init_keyboard() {
    register_interrupt_handler(IRQ1, keyboard_callback);
    printk_color(rc_black, rc_red, "keyboard_init\n");
}