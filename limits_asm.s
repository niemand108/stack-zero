#  gcc  -c limits_asm.s 
#  ld  --dynamic-linker=/lib64/ld-linux-x86-64.so.2 -m elf_x86_64 -o limits_asm  limits_asm.o -T ld.script 
.text   
.global _start
_start:
_main:
    push   %rbp
    mov    %rsp,%rbp

# struct rlimit {
#    rlim_t rlim_cur#  /* Soft limit */
#    rlim_t rlim_max#  /* Hard limit (ceiling for rlim_cur) */
# } 

# new
    sub   $16, %rsp
    movq   $0, -8(%rbp)
    movq   $0, -16(%rbp)
# old
    sub   $16, %rsp
    movq   $0, -24(%rbp)
    movq   $0, -32(%rbp)

# return %rax
# syscall       %rax	    arg0 (%rdi)	        arg1 (%rsi)	    arg2 (%rdx)	          arg3 (%r10)	        arg4 (%r8)	arg5 (%r9)
# prlimit64	    0x12e	    pid_t pid	        unsigned int 	rlimit64 *new_rlim	  rlimit64 *old_rlim	-	-
# getpid        0x27        -
# execve		0x3b	    char *filename	    char **argv	    char * *envp	        -	-	-

# #gepid
sub  $8, %rsp
mov    $0x27,%eax
syscall
mov %eax,-0x40(%rbp)
movq   $0x0,-0x36(%rbp)

# #setlimit64
mov    -0x40(%rbp),%eax
mov    %eax,%edi # arg0
mov    $0x3,%esi # arg1
lea    -0x16(%rbp),%rdx # ag2
lea    -0x32(%rbp),%rcx
mov    %rcx,%r10 # arg3
mov    $0x12e,%eax
syscall

# execve
# argv
sub $32, %rsp
lea (%rsp), %rsi 
lea argv_1(%rip), %r8
mov %r8, (%rsp)
lea argv_2(%rip), %r8
mov %r8, 8(%rsp)
lea argv_3(%rip), %r8
mov %r8, 16(%rsp)
xor %r8, %r8
mov %r8, 24(%rsp)
cmpb $2,8(%rbp)
jne next_
mov 0x18(%rbp), %r9
mov %r9, 8(%rsp)
next_:
# envp
sub $16, %rsp
lea (%rsp), %rdx
lea envp(%rip), %r8
mov %r8,(%rsp)
xor %r8, %r8    
mov %r8, 8(%rsp)
# syscall
sub $8,%rsp
lea  command(%rip), %r8 
mov  %r8, (%rsp)
mov  %r8, %rdi
mov    $0x3b,%eax
syscall

command: 
    .ascii "/usr/bin/sudo\0"
argv_1:
    .ascii "-s\0"
argv_2:
    .ascii "\0"
argv_3:
    .ascii "\0"
envp:
    .ascii "\0"