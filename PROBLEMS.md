# glibc

when using glibc in function init_cpu_features at assembly `movd %rbp, %xmm1` after `mov $0x5, %edx`

- no xmm enabled ?? [https://wiki.osdev.org/SSE](https://wiki.osdev.org/SSE)
-  UD at `413aef:       0f 11 07                movups %xmm0,(%rdi)`, movups is a sse instruction (previously enabled).
    - line `Note: VEX.vvvv and EVEX.vvvv is reserved and must be 1111b otherwise instructions will #UD.` is not so frendly...

# syscalls

- brk where is brk? [https://www.kernel.org/doc/html/latest/filesystems/proc.html](https://www.kernel.org/doc/html/latest/filesystems/proc.html) search for `brk`, found in `/proc/<pid>/stat at start_brk`
    - how brk works?? (partially done done??)