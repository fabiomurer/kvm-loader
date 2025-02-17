#include <string.h>
#include <stdio.h>
#include "descriptors.h"
#include "page.h"

#define GDT_OFFSET 0x500
#define IDT_OFFSET 0x520

static const struct seg_desc CODE_SEG = {
	.limit0 = 0xFFFF,
	.base0 = 0,
	.base1 = 0,
	.type = (
		TYPE_A_ACCES_DONTSET | 
		TYPE_RW_CODE_READ | 
		TYPE_DC_CODE_EXEC_IFDLP | 
		TYPE_E_CODE),
	.s = S_DATA_OR_CODE,
	.dpl = DLP_KERNEL,
	.p = P_VALID,
	.limit1 = 0xF,
	.avl = AVL,
	.l = L_LONGMODE_CODE,
	.d = D_LONGMODE,
	.g = G_4KIB,
	.base2 = 0,
};

static const struct seg_desc DATA_SEG = {
	.limit0 = 0xFFFF,
	.base0 = 0,
	.base1 = 0,
	.type = 0x2 | 0x1,
	.s = S_DATA_OR_CODE,
	.dpl = DLP_KERNEL,
	.p = P_VALID,
	.limit1 = (
		TYPE_A_ACCES_DONTSET |
		TYPE_RW_DATA_WRITE |
		TYPE_E_DATA
	),
	.avl = AVL,
	.l = L_OTHER, // era L_LONGMODE_CODE ??
	.d = D_LONGMODE,
	.g = G_4KIB,
	.base2 = 0,
};

static struct kvm_segment seg_from_desc(struct seg_desc e, uint32_t idx)
{
	struct kvm_segment res = {
		.base = e.base0 | ((uint64_t)e.base1 << 16) | ((uint64_t)e.base2 << 24),
		.limit = (uint64_t)e.limit0 | ((uint64_t)e.limit1 << 16),
		.selector = idx * 8,
		.type = e.type,
		.present = e.p,
		.dpl = e.dpl,
		.db = e.d,
		.s = e.s,
		.l = e.l,
		.g = e.g,
		.avl = e.avl,
		.padding = 0,
		.unusable = 0,
	};

	return res;
}

// https://wiki.osdev.org/GDT_Tutorial
void init_gdt(struct kvm_sregs *sregs)
{
	struct alloc_result mem = alloc_pages_mapped(1);
	void *gdt_addr = (void *)(mem.host + GDT_OFFSET);
	printf("descriptors: %lx\n", mem.guest);

	struct kvm_segment code_segment = seg_from_desc(CODE_SEG, 1);
	struct kvm_segment data_segment = seg_from_desc(DATA_SEG, 2);

	// null descriptor
	memset(gdt_addr, 0, 8);
	// one code segment 
	memcpy(gdt_addr + 8, &CODE_SEG, 8);
	// one data segment
	memcpy(gdt_addr + 16, &DATA_SEG, 8);

	sregs->gdt.base = GDT_OFFSET + mem.guest;
	sregs->gdt.limit = 3 * 8 - 1;
	memset((void *)(mem.host + IDT_OFFSET), 0, 8);
	sregs->idt.base = IDT_OFFSET + mem.guest;
	sregs->idt.limit = 7;
	sregs->cr0 |= 1;
	sregs->efer |= 0x100 | 0x400;
	sregs->cs = code_segment;
	sregs->ds = data_segment;
	sregs->ss = data_segment;
	sregs->es = data_segment;
	sregs->fs = data_segment;
	sregs->gs = data_segment;
}

void print_seg(struct seg_desc *desc)
{
	printf("Base0: %x base1: %x base2: %x\n", desc->base0, desc->base1,
		desc->base2);
	printf("Limit0: %d limit1: %d\n", desc->limit0, desc->limit1);
	printf("Type: %x\n", desc->type);
	printf("S: %x\n", desc->s);
	printf("DPL: %d\n", desc->dpl);
	printf("P: %d\n", desc->p);
	printf("AVL: %d\n", desc->avl);
	printf("L: %d\n", desc->l);
	printf("D: %d\n", desc->d);
	printf("G: %d\n", desc->g);
}
