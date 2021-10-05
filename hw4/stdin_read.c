#include <unistd.h>
#include <stdio.h>

int main(int argc, char** argv) {
    char *ch;
    size_t size = 1;
    long ret;
    
    printf("輸入一個字元:\n");
    
    __asm__ volatile ( 
        "mov $0, %%rax\n"   //system call number 0(read)
        "mov $1, %%rdi\n"   //stdout (fd)
        "mov %1, %%rsi\n"   //buffer (char *buf)
        "mov %2, %%rdx\n"   //size (size_t count)
        "syscall\n"
        "mov %%rax, %0\n"
        : "=m"(ret), "=m"(ch)
        : "g"(size)
        : "rax", "rbx", "rcx", "rdx");
        
    getchar();
        
    printf("讀入的字元是""%c""\n", *ch);
    printf("回傳值是%ld\n", ret);
   
    return 0;
}
