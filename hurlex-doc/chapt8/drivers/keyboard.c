#include "timer.h"
#include "debug.h"
#include "common.h"
#include "idt.h"

#define KBD_BUF_PORT 0x60 // 键盘 buffer 寄存器端口号为 0x60

/* 用转义字符定义部分控制字符 */
#define esc        '\033'     // 八进制表示字符,也可以用十六进制'\x1b'
#define backspace  '\b'
#define tab        '\t'
#define enter      '\r'
#define delete     '\177'     // 八进制表示字符,十六进制为'\x7f'

/* 以上不可见字符一律定义为0 */
#define char_invisible 0
#define ctrl_l_char    char_invisible
#define ctrl_r_char    char_invisible
#define shift_l_char   char_invisible
#define shift_r_char   char_invisible
#define alt_l_char     char_invisible
#define alt_r_char     char_invisible
#define caps_lock_char char_invisible

/* 定义控制字符的通码和断码 */
#define shift_l_make   0x2a
#define shift_r_make   0x36
#define alt_l_make     0x38
#define alt_r_make     0xe038
#define alt_r_break    0xe0b8
#define ctrl_l_make    0x1d
#define ctrl_r_make    0xe01d
#define ctrl_r_break   0xe09d
#define caps_lock_make 0x3a
/*
 * 以通码make_code为索引的二维数组
 * 扫描码   未与shift组合  与shift组合
 * 其它按键暂不处理
 */
static char keymap[][2] = {
        {0,    0},
        {esc,            esc},
        {'1',  '!'},
        {'2',  '@'},
        {'3',  '#'},
        {'4',  '$'},
        {'5',  '%'},
        {'6',  '^'},
        {'7',  '&'},
        {'8',  '*'},
        {'9',  '('},
        {'0',  ')'},
        {'-',  '_'},
        {'=',  '+'},
        {backspace,      backspace},
        {tab,            tab},
        {'q',  'Q'},
        {'w',  'W'},
        {'e',  'E'},
        {'r',  'R'},
        {'t',  'T'},
        {'y',  'Y'},
        {'u',  'U'},
        {'i',  'I'},
        {'o',  'O'},
        {'p',  'P'},
        {'[',  '{'},
        {']',  '}'},
        {enter,          enter},
        {ctrl_l_char,    ctrl_l_char},
        {'a',  'A'},
        {'s',  'S'},
        {'d',  'D'},
        {'f',  'F'},
        {'g',  'G'},
        {'h',  'H'},
        {'j',  'J'},
        {'k',  'K'},
        {'l',  'L'},
        {';',  ':'},
        {'\'', '"'},
        {'`',  '~'},
        {shift_l_char,   shift_l_char},
        {'\\', '|'},
        {'z',  'Z'},
        {'x',  'X'},
        {'c',  'C'},
        {'v',  'V'},
        {'b',  'B'},
        {'n',  'N'},
        {'m',  'M'},
        {',',  '<'},
        {'.',  '>'},
        {'/',  '?'},
        {shift_r_char,   shift_r_char},
        {'*',  '*'},
        {alt_l_char,     alt_l_char},
        {' ',  ' '},
        {caps_lock_char, caps_lock_char}
};
static uint16_t shift_status = FALSE;

// 键盘中断处理程序
void keyboard_callback(pt_regs *regs) {
    uint16_t scan_code = inb(KBD_BUF_PORT);
    uint16_t shift = shift_status;

    // 是否按下了shift键
    if (scan_code == shift_l_make || scan_code == shift_r_make) {
        shift_status = TRUE;
        return;
    }
    shift_status = FALSE;

    // 按键弹起不做处理
    if (scan_code >= 0x80) {
        return;
    }

    uint8_t index = (scan_code &= 0x00ff);  // 将扫描码的高字节置0，主要是针对高字节是e0的扫描码.
    char cur_char = keymap[index][shift];  // 在数组中找到对应的字符
    if (cur_char) {
        printk_color(rc_black, rc_red, "keyboard_callback %c\n", cur_char);
    }
    return;
}

// 键盘初始化
void init_keyboard() {
    register_interrupt_handler(IRQ1, keyboard_callback);
    printk_color(rc_black, rc_red, "keyboard_init\n");
}