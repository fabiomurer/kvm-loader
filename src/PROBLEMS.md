# glibc

when using glibc in function init_cpu_features at assembly `movd %rbp, %xmm1` after `mov $0x5, %edx`

- no xmm enabled ?? [https://wiki.osdev.org/SSE](https://wiki.osdev.org/SSE)

# syscalls

- brk where is brk? [https://www.kernel.org/doc/html/latest/filesystems/proc.html](https://www.kernel.org/doc/html/latest/filesystems/proc.html) search for `brk`, found in `/proc/<pid>/stat at start_brk`
    - how brk works?? 