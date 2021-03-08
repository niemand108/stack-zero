# stack-zero
Experiments with stack &amp; suid when the stack size is zero.

Compiling
=========

gcc  -c limits_asm.s 

ld  --dynamic-linker=/lib64/ld-linux-x86-64.so.2 -m elf_x86_64 -o limits_asm  limits_asm.o -T ld.script 

In ld.script you can set the load address for the assembly code.

Experiments
===========

The first parameter will be the first parameter for execve("sudo")

./limits_asm $(python -c "print 'A' * 3500")

Results
=======

Although the stack size of sudo is set to zero, the kernel expands the stack a bit during the elf binary load. This expansion may be enough for start the execution but it will be a sigsegv when the loader exhaust the minimal stack set for the kernel. If the first stack expansion is insufficient, the kernel will try to expand a second time, failing ant terminating the elf binary load, finally forcing a sigsegv. The interesting part is that after the execve syscall the kernel will have updated the memory mappings and (very interesting too) the cpu registers will be those of the execve calling process, the rip register will point to the next instruction after the syscall. You can change the "limits_asm" load address in ld.script to match with the executable meory map of sudo but this trick is useless because our experimental process will never execute code. I tried to send SIGCONT signals before the kernel force a sigsegv but again is useless, our process always die. 

Others experiments
==================

Settings limits of pending signals and queue of messages.
