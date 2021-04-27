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