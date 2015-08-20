#include <stdio.h>

/* 打印调用栈的最大深度 */
#define DUMP_STACK_DEPTH_MAX 16

/* 获取寄存器ebp的值 */
void get_ebp(unsigned long *ebp) {
    __asm__ __volatile__ (
        "mov %%ebp, %0"
        :"=m"(*ebp)
        ::"memory");
}

/* 获取调用栈 */
int dump_stack(void **stack, int size) {
    unsigned long ebp = 0;
    int depth = 0;

    /* 1.得到首层函数的栈基址 */
    get_ebp(&ebp);

    /* 2.逐层回溯栈基址 */
    for (depth = 0; (depth < size) && (0 != ebp) && (0 != *(unsigned long *)ebp) && (ebp != *(unsigned long *)ebp); ++depth) {
        stack[depth] = (void *)(*(unsigned long *)(ebp + sizeof(unsigned long)));
        ebp = *(unsigned long *)ebp;
    }

    return depth;
}

/* 测试函数 2 */
void test_meloner() {
    void *stack[DUMP_STACK_DEPTH_MAX] = {0};
    int stack_depth = 0;
    int i = 0;

    /* 获取调用栈 */
    stack_depth = dump_stack(stack, DUMP_STACK_DEPTH_MAX);

    /* 打印调用栈 */
    printf(" Stack Track: \r\n");
    for (i = 0; i < stack_depth; ++i) {
        printf(" [%d] %p \r\n", i, stack[i]);
    }

    return;
}

/* 测试函数 1 */
void test_hutaow() {
    test_meloner();
    return;
}

/* 主函数 */
int main(int argc, char *argv[]) {
    test_hutaow();
    return 0;
}

