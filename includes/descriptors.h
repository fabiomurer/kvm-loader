#ifndef __KL_DESC
#define __KL_DESC
#include <linux/kvm.h>
#include <stdint.h>

struct __attribute__((packed)) seg_desc {
	uint16_t limit0;
	uint16_t base0;
	uint16_t base1: 8, type: 4, s: 1, dpl: 2, p: 1;
	uint16_t limit1: 4, avl: 1, l: 1, d: 1, g: 1, base2: 8;
};

void init_gdt(struct kvm_sregs *sregs);
void print_seg(struct seg_desc *desc);
#endif
