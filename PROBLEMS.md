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

- UD at `405857:       48 c7 83 38 09 00 00    movq   $0x0,0x938(%rbx)` in `<__libc_setup_tls>`
```
0000000000405730 <__libc_setup_tls>:
  405730:	f3 0f 1e fa          	endbr64
  405734:	55                   	push   %rbp
  405735:	48 89 e5             	mov    %rsp,%rbp
  405738:	53                   	push   %rbx
  405739:	48 83 ec 08          	sub    $0x8,%rsp
  40573d:	48 8b 1d fc 29 0a 00 	mov    0xa29fc(%rip),%rbx        # 4a8140 <_dl_ns>
  405744:	e8 b7 bc 01 00       	call   421400 <__tls_pre_init_tp>
  405749:	48 8b 0d 68 82 0a 00 	mov    0xa8268(%rip),%rcx        # 4ad9b8 <_dl_phnum>
  405750:	48 8b 05 69 82 0a 00 	mov    0xa8269(%rip),%rax        # 4ad9c0 <_dl_phdr>
  405757:	48 8d 14 cd 00 00 00 	lea    0x0(,%rcx,8),%rdx
  40575e:	00 
  40575f:	48 29 ca             	sub    %rcx,%rdx
  405762:	48 8d 14 d0          	lea    (%rax,%rdx,8),%rdx
  405766:	48 39 d0             	cmp    %rdx,%rax
  405769:	72 0e                	jb     405779 <__libc_setup_tls+0x49>
  40576b:	e9 80 00 00 00       	jmp    4057f0 <__libc_setup_tls+0xc0>
  405770:	48 83 c0 38          	add    $0x38,%rax
  405774:	48 39 d0             	cmp    %rdx,%rax
  405777:	73 77                	jae    4057f0 <__libc_setup_tls+0xc0>
  405779:	83 38 07             	cmpl   $0x7,(%rax)
  40577c:	75 f2                	jne    405770 <__libc_setup_tls+0x40>
  40577e:	48 8b 50 10          	mov    0x10(%rax),%rdx
  405782:	f3 0f 6f 40 20       	movdqu 0x20(%rax),%xmm0
  405787:	48 8b 40 30          	mov    0x30(%rax),%rax
  40578b:	48 03 13             	add    (%rbx),%rdx
  40578e:	48 89 1d 13 38 0a 00 	mov    %rbx,0xa3813(%rip)        # 4a8fa8 <static_slotinfo+0x28>
  405795:	48 c7 05 e0 37 0a 00 	movq   $0x40,0xa37e0(%rip)        # 4a8f80 <static_slotinfo>
  40579c:	40 00 00 00 
  4057a0:	48 89 83 78 04 00 00 	mov    %rax,0x478(%rbx)
  4057a7:	48 8d 05 d2 37 0a 00 	lea    0xa37d2(%rip),%rax        # 4a8f80 <static_slotinfo>
  4057ae:	80 8b 56 03 00 00 80 	orb    $0x80,0x356(%rbx)
  4057b5:	48 89 93 60 04 00 00 	mov    %rdx,0x460(%rbx)
  4057bc:	48 89 05 95 33 0a 00 	mov    %rax,0xa3395(%rip)        # 4a8b58 <_dl_tls_dtv_slotinfo_list>
  4057c3:	b8 01 00 00 00       	mov    $0x1,%eax
  4057c8:	48 c7 83 90 04 00 00 	movq   $0x1,0x490(%rbx)
  4057cf:	01 00 00 00 
  4057d3:	48 c7 05 8a 33 0a 00 	movq   $0x1,0xa338a(%rip)        # 4a8b68 <_dl_tls_max_dtv_idx>
  4057da:	01 00 00 00 
  4057de:	0f 11 83 68 04 00 00 	movups %xmm0,0x468(%rbx)
  4057e5:	eb 10                	jmp    4057f7 <__libc_setup_tls+0xc7>
  4057e7:	66 0f 1f 84 00 00 00 	nopw   0x0(%rax,%rax,1)
  4057ee:	00 00 
  4057f0:	48 8b 05 71 33 0a 00 	mov    0xa3371(%rip),%rax        # 4a8b68 <_dl_tls_max_dtv_idx>
  4057f7:	31 ff                	xor    %edi,%edi
  4057f9:	48 89 05 50 33 0a 00 	mov    %rax,0xa3350(%rip)        # 4a8b50 <_dl_tls_static_nelem>
  405800:	e8 0b b2 01 00       	call   420a10 <_dl_tls_static_surplus_init>
  405805:	e8 46 b4 01 00       	call   420c50 <_dl_determine_tlsoffset>
  40580a:	48 8b 1d 37 33 0a 00 	mov    0xa3337(%rip),%rbx        # 4a8b48 <_dl_tls_static_size>
  405811:	48 8b 3d 20 33 0a 00 	mov    0xa3320(%rip),%rdi        # 4a8b38 <_dl_tls_static_align>
  405818:	48 01 df             	add    %rbx,%rdi
  40581b:	e8 00 d2 01 00       	call   422a20 <_dl_early_allocate>
  405820:	49 89 c0             	mov    %rax,%r8
  405823:	48 85 c0             	test   %rax,%rax
  405826:	0f 84 c4 00 00 00    	je     4058f0 <__libc_setup_tls+0x1c0>
  40582c:	48 8b 35 05 33 0a 00 	mov    0xa3305(%rip),%rsi        # 4a8b38 <_dl_tls_static_align>
  405833:	31 d2                	xor    %edx,%edx
  405835:	49 8d 4c 30 ff       	lea    -0x1(%r8,%rsi,1),%rcx
  40583a:	48 89 c8             	mov    %rcx,%rax
  40583d:	48 f7 f6             	div    %rsi
  405840:	31 c0                	xor    %eax,%eax
  405842:	48 29 d1             	sub    %rdx,%rcx
  405845:	48 8d 9c 0b c0 f6 ff 	lea    -0x940(%rbx,%rcx,1),%rbx
  40584c:	ff 
  40584d:	48 8d 7b 08          	lea    0x8(%rbx),%rdi
  405851:	48 89 d9             	mov    %rbx,%rcx
  405854:	48 89 de             	mov    %rbx,%rsi
# 405857:	48 c7 83 38 09 00 00 	movq   $0x0,0x938(%rbx)
  40585e:	00 00 00 00 
  405862:	48 83 e7 f8          	and    $0xfffffffffffffff8,%rdi
  405866:	48 29 f9             	sub    %rdi,%rcx
  405869:	81 c1 40 09 00 00    	add    $0x940,%ecx
  40586f:	c1 e9 03             	shr    $0x3,%ecx
  405872:	f3 48 ab             	rep stos %rax,%es:(%rdi)
  405875:	48 8d 05 14 33 0a 00 	lea    0xa3314(%rip),%rax        # 4a8b90 <_dl_static_dtv+0x10>
  40587c:	48 89 5b 10          	mov    %rbx,0x10(%rbx)
  405880:	bf 02 10 00 00       	mov    $0x1002,%edi
  405885:	48 c7 05 f0 32 0a 00 	movq   $0x3e,0xa32f0(%rip)        # 4a8b80 <_dl_static_dtv>
  40588c:	3e 00 00 00 
  405890:	48 89 43 08          	mov    %rax,0x8(%rbx)
  405894:	b8 9e 00 00 00       	mov    $0x9e,%eax
  405899:	48 89 1b             	mov    %rbx,(%rbx)
  40589c:	0f 05                	syscall
  40589e:	85 c0                	test   %eax,%eax
  4058a0:	75 1e                	jne    4058c0 <__libc_setup_tls+0x190>
  4058a2:	e8 a9 bb 01 00       	call   421450 <__tls_init_tp>
  4058a7:	48 89 df             	mov    %rbx,%rdi
  4058aa:	be 01 00 00 00       	mov    $0x1,%esi
  4058af:	48 8b 5d f8          	mov    -0x8(%rbp),%rbx
  4058b3:	c9                   	leave
  4058b4:	e9 97 b6 01 00       	jmp    420f50 <_dl_allocate_tls_init>
  4058b9:	90                   	nop
  4058ba:	66 0f 1f 44 00 00    	nopw   0x0(%rax,%rax,1)
  4058c0:	ba 2d 00 00 00       	mov    $0x2d,%edx
  4058c5:	bf 02 00 00 00       	mov    $0x2,%edi
  4058ca:	b8 01 00 00 00       	mov    $0x1,%eax
  4058cf:	48 8d 35 d2 8c 07 00 	lea    0x78cd2(%rip),%rsi        # 47e5a8 <_libc_intl_domainname+0xedb>
  4058d6:	0f 05                	syscall
  4058d8:	bf 7f 00 00 00       	mov    $0x7f,%edi
  4058dd:	b8 e7 00 00 00       	mov    $0xe7,%eax
  4058e2:	0f 05                	syscall
  4058e4:	eb bc                	jmp    4058a2 <__libc_setup_tls+0x172>
  4058e6:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  4058ed:	00 00 00 
  4058f0:	ba 2d 00 00 00       	mov    $0x2d,%edx
  4058f5:	bf 02 00 00 00       	mov    $0x2,%edi
  4058fa:	b8 01 00 00 00       	mov    $0x1,%eax
  4058ff:	48 8d 35 a2 8c 07 00 	lea    0x78ca2(%rip),%rsi        # 47e5a8 <_libc_intl_domainname+0xedb>
  405906:	0f 05                	syscall
  405908:	bf 7f 00 00 00       	mov    $0x7f,%edi
  40590d:	b8 e7 00 00 00       	mov    $0xe7,%eax
  405912:	0f 05                	syscall
  405914:	e9 13 ff ff ff       	jmp    40582c <__libc_setup_tls+0xfc>
```

# syscalls

- brk where is brk? [https://www.kernel.org/doc/html/latest/filesystems/proc.html](https://www.kernel.org/doc/html/latest/filesystems/proc.html) search for `brk`, found in `/proc/<pid>/stat at start_brk`
    - how brk works?? (partially done done??)