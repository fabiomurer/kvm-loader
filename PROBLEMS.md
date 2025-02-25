# how linux initialize registers

```
(gdb) info all-registers 
rax            0x0                 0
rbx            0x0                 0
rcx            0x0                 0
rdx            0x0                 0
rsi            0x0                 0
rdi            0x0                 0
rbp            0x0                 0x0
rsp            0x7fffffffdf70      0x7fffffffdf70
r8             0x0                 0
r9             0x0                 0
r10            0x0                 0
r11            0x0                 0
r12            0x0                 0
r13            0x0                 0
r14            0x0                 0
r15            0x0                 0
rip            0x401720            0x401720 <_start>
eflags         0x202               [ IF ]
cs             0x33                51
ss             0x2b                43
ds             0x0                 0
es             0x0                 0
fs             0x0                 0
gs             0x0                 0
st0            0                   (raw 0x00000000000000000000)
st1            0                   (raw 0x00000000000000000000)
st2            0                   (raw 0x00000000000000000000)
st3            0                   (raw 0x00000000000000000000)
st4            0                   (raw 0x00000000000000000000)
st5            0                   (raw 0x00000000000000000000)
st6            0                   (raw 0x00000000000000000000)
st7            0                   (raw 0x00000000000000000000)
fctrl          0x37f               895
fstat          0x0                 0
ftag           0xffff              65535
fiseg          0x0                 0
fioff          0x0                 0
foseg          0x0                 0
fooff          0x0                 0
fop            0x0                 0
mxcsr          0x1f80              [ IM DM ZM OM UM PM ]
fs_base        0x0                 0
gs_base        0x0                 0
ymm0           {v16_bfloat16 = {0x0 <repeats 16 times>}, v16_half = {0x0 <repeats 16 times>}, v8_float = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_double = {0x0, 0x0, 0x0, 0x0}, v32_int8 = {0x0 <repeats 32 times>}, v16_int16 = {0x0 <repeats 16 times>}, v8_int32 = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_int64 = {0x0, 0x0, 0x0, 0x0}, v2_int128 = {0x0, 0x0}}
ymm1           {v16_bfloat16 = {0x0 <repeats 16 times>}, v16_half = {0x0 <repeats 16 times>}, v8_float = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_double = {0x0, 0x0, 0x0, 0x0}, v32_int8 = {0x0 <repeats 32 times>}, v16_int16 = {0x0 <repeats 16 times>}, v8_int32 = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_int64 = {0x0, 0x0, 0x0, 0x0}, v2_int128 = {0x0, 0x0}}
--Type <RET> for more, q to quit, c to continue without paging--
ymm2           {v16_bfloat16 = {0x0 <repeats 16 times>}, v16_half = {0x0 <repeats 16 times>}, v8_float = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_double = {0x0, 0x0, 0x0, 0x0}, v32_int8 = {0x0 <repeats 32 times>}, v16_int16 = {0x0 <repeats 16 times>}, v8_int32 = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_int64 = {0x0, 0x0, 0x0, 0x0}, v2_int128 = {0x0, 0x0}}
ymm3           {v16_bfloat16 = {0x0 <repeats 16 times>}, v16_half = {0x0 <repeats 16 times>}, v8_float = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_double = {0x0, 0x0, 0x0, 0x0}, v32_int8 = {0x0 <repeats 32 times>}, v16_int16 = {0x0 <repeats 16 times>}, v8_int32 = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_int64 = {0x0, 0x0, 0x0, 0x0}, v2_int128 = {0x0, 0x0}}
ymm4           {v16_bfloat16 = {0x0 <repeats 16 times>}, v16_half = {0x0 <repeats 16 times>}, v8_float = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_double = {0x0, 0x0, 0x0, 0x0}, v32_int8 = {0x0 <repeats 32 times>}, v16_int16 = {0x0 <repeats 16 times>}, v8_int32 = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_int64 = {0x0, 0x0, 0x0, 0x0}, v2_int128 = {0x0, 0x0}}
ymm5           {v16_bfloat16 = {0x0 <repeats 16 times>}, v16_half = {0x0 <repeats 16 times>}, v8_float = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_double = {0x0, 0x0, 0x0, 0x0}, v32_int8 = {0x0 <repeats 32 times>}, v16_int16 = {0x0 <repeats 16 times>}, v8_int32 = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_int64 = {0x0, 0x0, 0x0, 0x0}, v2_int128 = {0x0, 0x0}}
ymm6           {v16_bfloat16 = {0x0 <repeats 16 times>}, v16_half = {0x0 <repeats 16 times>}, v8_float = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_double = {0x0, 0x0, 0x0, 0x0}, v32_int8 = {0x0 <repeats 32 times>}, v16_int16 = {0x0 <repeats 16 times>}, v8_int32 = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_int64 = {0x0, 0x0, 0x0, 0x0}, v2_int128 = {0x0, 0x0}}
ymm7           {v16_bfloat16 = {0x0 <repeats 16 times>}, v16_half = {0x0 <repeats 16 times>}, v8_float = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_double = {0x0, 0x0, 0x0, 0x0}, v32_int8 = {0x0 <repeats 32 times>}, v16_int16 = {0x0 <repeats 16 times>}, v8_int32 = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_int64 = {0x0, 0x0, 0x0, 0x0}, v2_int128 = {0x0, 0x0}}
ymm8           {v16_bfloat16 = {0x0 <repeats 16 times>}, v16_half = {0x0 <repeats 16 times>}, v8_float = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_double = {0x0, 0x0, 0x0, 0x0}, v32_int8 = {0x0 <repeats 32 times>}, v16_int16 = {0x0 <repeats 16 times>}, v8_int32 = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_int64 = {0x0, 0x0, 0x0, 0x0}, v2_int128 = {0x0, 0x0}}
ymm9           {v16_bfloat16 = {0x0 <repeats 16 times>}, v16_half = {0x0 <repeats 16 times>}, v8_float = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_double = {0x0, 0x0, 0x0, 0x0}, v32_int8 = {0x0 <repeats 32 times>}, v16_int16 = {0x0 <repeats 16 times>}, v8_int32 = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_int64 = {0x0, 0x0, 0x0, 0x0}, v2_int128 = {0x0, 0x0}}
ymm10          {v16_bfloat16 = {0x0 <repeats 16 times>}, v16_half = {0x0 <repeats 16 times>}, v8_float = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_double = {0x0, 0x0, 0x0, 0x0}, v32_int8 = {0x0 <repeats 32 times>}, v16_int16 = {0x0 <repeats 16 times>}, v8_int32 = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_int64 = {0x0, 0x0, 0x0, 0x0}, v2_int128 = {0x0, 0x0}}
ymm11          {v16_bfloat16 = {0x0 <repeats 16 times>}, v16_half = {0x0 <repeats 16 times>}, v8_float = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_double = {0x0, 0x0, 0x0, 0x0}, v32_int8 = {0x0 <repeats 32 times>}, v16_int16 = {0x0 <repeats 16 times>}, v8_int32 = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_int64 = {0x0, 0x0, 0x0, 0x0}, v2_int128 = {0x0, 0x0}}
ymm12          {v16_bfloat16 = {0x0 <repeats 16 times>}, v16_half = {0x0 <repeats 16 times>}, v8_float = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_double = {0x0, 0x0, 0x0, 0x0}, v32_int8 = {0x0 <repeats 32 times>}, v16_int16 = {0x0 <repeats 16 times>}, v8_int32 = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_int64 = {0x0, 0x0, 0x0, 0x0}, v2_int128 = {0x0, 0x0}}
ymm13          {v16_bfloat16 = {0x0 <repeats 16 times>}, v16_half = {0x0 <repeats 16 times>}, v8_float = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_double = {0x0, 0x0, 0x0, 0x0}, v32_int8 = {0x0 <repeats 32 times>}, v16_int16 = {0x0 <repeats 16 times>}, v8_int32 = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_int64 = {0x0, 0x0, 0x0, 0x0}, v2_int128 = {0x0, 0x0}}
ymm14          {v16_bfloat16 = {0x0 <repeats 16 times>}, v16_half = {0x0 <repeats 16 times>}, v8_float = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_double = {0x0, 0x0, 0x0, 0x0}, v32_int8 = {0x0 <repeats 32 times>}, v16_int16 = {0x0 <repeats 16 times>}, v8_int32 = {0x0, 0x0, 0--Type <RET> for more, q to quit, c to continue without paging--
x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_int64 = {0x0, 0x0, 0x0, 0x0}, v2_int128 = {0x0, 0x0}}
ymm15          {v16_bfloat16 = {0x0 <repeats 16 times>}, v16_half = {0x0 <repeats 16 times>}, v8_float = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_double = {0x0, 0x0, 0x0, 0x0}, v32_int8 = {0x0 <repeats 32 times>}, v16_int16 = {0x0 <repeats 16 times>}, v8_int32 = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, v4_int64 = {0x0, 0x0, 0x0, 0x0}, v2_int128 = {0x0, 0x0}}
```

## sse
- `mxcsr 0x1f80 [ IM DM ZM OM UM PM ]`
- `0b1111110000000`
    ```
    Bits 0 through 5 of the MXCSR register indicate whether a SIMD floating-point exception has been detected. They
    are “sticky” flags. That is, after a flag is set, it remains set until explicitly cleared. To clear these flags, use the
    LDMXCSR or the FXRSTOR instruction to write zeroes to them.
    Bits 7 through 12 provide individual mask bits for the SIMD floating-point exceptions. An exception type is masked
    if the corresponding mask bit is set, and it is unmasked if the bit is clear. These mask bits are set upon a power-up
    or reset. This causes all SIMD floating-point exceptions to be initially masked.
    ```
    - Precision Mask
    - Underflow Mask
    - Overflow Mask
    - Divide-by-Zero Mask
    - Invalid Operation Mask
    - Denormals Are Zeros*

## other


### fctrl
In the x86_64 architecture, `fctrl` in GDB refers to the Floating-Point Control Register (also known as the x87 FPU Control Word or FPUCW). This is not a general-purpose register but rather a special register that controls the behavior of the x87 floating-point unit.

```
Bits 15-13    12     11-10    9-8     7-6     5     4     3     2     1     0
   Reserved   IC      RC      PC    Reserved  PM    UM    OM    ZM    DM    IM

Where:

    PC (Precision Control) - Bits 8-9:
        00: Single precision (24 bits)
        10: Double precision (53 bits)
        11: Extended precision (64 bits)

    RC (Rounding Control) - Bits 10-11:
        00: Round to nearest (even)
        01: Round down (toward -∞)
        10: Round up (toward +∞)
        11: Round toward zero (truncate)

    Exception Mask Bits - Bits 0-5:
        IM: Invalid Operation Mask
        DM: Denormal Operand Mask
        ZM: Zero Divide Mask
        OM: Overflow Mask
        UM: Underflow Mask
        PM: Precision Mask

When a bit is set (1), the corresponding exception is masked (ignored).
```

### ftag
The ftag register in x86_64 architecture, as displayed in GDB, refers to the Floating-Point Tag Register. This is a component of the x87 FPU (Floating-Point Unit) that keeps track of the status of each of the eight floating-point registers (ST0-ST7).
Overview of the ftag Register

The ftag register is a 16-bit register that contains information about the content type stored in each x87 floating-point register. It allocates 2 bits per register to indicate its status.
Register Structure
Code

```
Bits: 15-14 13-12 11-10 9-8 7-6 5-4 3-2 1-0
      ST7   ST6   ST5   ST4 ST3 ST2 ST1 ST0

Each 2-bit field has the following encodings:

    00: Valid - Register contains a valid floating-point value
    01: Zero - Register contains a zero value (±0)
    10: Special - Register contains a special value (NaN, infinity, denormal)
    11: Empty - Register is empty
```


# glibc

when using glibc in function init_cpu_features at assembly `movd %rbp, %xmm1` after `mov $0x5, %edx`

- no xmm enabled ?? [https://wiki.osdev.org/SSE](https://wiki.osdev.org/SSE)
-  UD at `413aef:       0f 11 07                movups %xmm0,(%rdi)`, movups is a sse instruction (previously enabled).
    - line `Note: VEX.vvvv and EVEX.vvvv is reserved and must be 1111b otherwise instructions will #UD.` is not so frendly...

# syscalls

- brk where is brk? [https://www.kernel.org/doc/html/latest/filesystems/proc.html](https://www.kernel.org/doc/html/latest/filesystems/proc.html) search for `brk`, found in `/proc/<pid>/stat at start_brk`
    - how brk works?? (partially done done??)