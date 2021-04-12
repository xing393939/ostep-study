/*
 * 实验发现，栈端在高地址，其他在低地址（低到高依次是text、data、bbs、堆）
 */
#include<stdio.h>
#include<malloc.h>
#include<unistd.h>

int bss_var;
int data_var0 = 1;

void call_func() {
    int stack_var1 = 3;
    printf("%p\n", &stack_var1);
}

int main(int argc, char **argv) {
    printf("below are addresses of types of process's mem\n");
    printf("Text location:\n");
    printf("%p\n", main);
    printf("____________________________\n");
    int stack_var0 = 2;
    printf("Stack Location:\n");
    printf("%p\n", &stack_var0);
    call_func();
    printf("____________________________\n");
    printf("Data Location:\n");
    printf("%p\n", &data_var0);
    static int data_var1 = 4;
    printf("%p\n", &data_var1);
    printf("____________________________\n");
    printf("BSS Location:\n");
    printf("%p\n", &bss_var);
    printf("____________________________\n");
    char *b = sbrk((ptrdiff_t) 0);
    printf("Heap Location:\n");
    printf("%p\n", b);
    brk(b + 4);
    b = sbrk((ptrdiff_t) 0);
    printf("%p\n", b);
    return 0;
}