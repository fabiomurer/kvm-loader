#ifndef __KL_DESC
#define __KL_DESC
#include <linux/kvm.h>
#include <stdint.h>

/*
GDT entry 8 bytes long -> 64 bits

## Global Descriptor Table 
| Address 			 | Content
| ------------------ | -------
| GDTR Offset + 0 	| Null
| GDTR Offset + 8 	| Entry 1
| GDTR Offset + 16 	| Entry 2
| GDTR Offset + 24 	| Entry 3 

## GDT entry 

- (0-15)	limit0 always 0 in 64 bit mode
- (16-31)	base0  ...
- (32,39) 	base1

## access bytes (40,47)
- type 4 bit
	- A accessed bit CPU set it when segment is accessed unless set to 1 in advance
	- RW code (write newer allowed) -> read if 1, data (read always allowed) write if 1
	- DC for data->direction bit 0 := up, code-> 0 executed if cpu in dlp
	- E Executable bit, 1 := executable segment
Types available in Long Mode:
    0x2: LDT
    0x9: 64-bit TSS (Available)
    0xB: 64-bit TSS (Busy)

- s Descriptor type, 0 := system segment, 1 := code or data segment
- 2 bit dlp CPU Privilege level of the segment. (0 kernel-3 user)
- p present bit. Must be set (1) for any valid segment
- limit1

## flags
- avl reserved
- l long mode flag if 1 defines a 64-bit code segment, When set, DB = 0, other type of segment = 0
- DB size flag 0 16 bit segment 1 32 bit segment 
- G If 0, the Limit is in 1 Byte blocks. If 1, the Limit is in 4 KiB blocks

- base2
*/
struct __attribute__((packed)) seg_desc {
	uint16_t limit0;
	uint16_t base0;
	uint16_t base1: 8, type: 4, s: 1, dpl: 2, p: 1;
	uint16_t limit1: 4, avl: 1, l: 1, d: 1, g: 1, base2: 8;
};

void init_gdt(struct kvm_sregs *sregs);
void print_seg(struct seg_desc *desc);
#endif
