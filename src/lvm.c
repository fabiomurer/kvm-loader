#include <stdio.h>
#include <stdlib.h>

#include <sys/ioctl.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/kvm.h>
#include <sys/mman.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include "elf.h"
#include "page.h"

#define __pck __attribute__((packed))

#define SEG_SIZE 0x1000
#define GDT_OFFSET 0x500
#define IDT_OFFSET 0x520
#define STACK_START 0x2000

#define GUEST_INFO_START 0x1000

struct __pck seg_desc {
	uint16_t limit0;
	uint16_t base0;
	uint16_t base1: 8, type: 4, s: 1, dpl: 2, p: 1;
	uint16_t limit1: 4, avl: 1, l: 1, d: 1, g: 1, base2: 8;
};

static const struct seg_desc KERNEL_CODE_SEG = {
	.limit0 = 0xFFFF,
	.base0 = 0,
	.base1 = 0,
	.type = 11,
	.s = 1,
	.dpl = 0,
	.p = 1,
	.limit1 = 0xF,
	.avl = 0,
	.l = 1,
	.d = 0,
	.g = 1,
	.base2 = 0,
};

static const struct seg_desc DATA_SEG = {
	.limit0 = 0xFFFF,
	.base0 = 0,
	.base1 = 0,
	.type = 0x2 | 0x1,
	.s = 1,
	.dpl = 0,
	.p = 1,
	.limit1 = 0xF,
	.avl = 0,
	.l = 1,
	.d = 0,
	.g = 1,
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

static void init_gdt(void *mem, struct kvm_sregs *sregs)
{
	void *gdt_addr = mem + GDT_OFFSET;

	memset(gdt_addr, 0, 8);
	memcpy(gdt_addr + 8, &KERNEL_CODE_SEG, 8);
	memcpy(gdt_addr + 16, &DATA_SEG, 8);

	sregs->gdt.base = GDT_OFFSET + GUEST_INFO_START;
	sregs->gdt.limit = 3 * 8 - 1;
	memset(mem + IDT_OFFSET + GUEST_INFO_START, 0, 8);
	sregs->idt.base = IDT_OFFSET + GUEST_INFO_START;
	sregs->idt.limit = 7;
	sregs->cr0 |= 1;
	sregs->efer |= 0x100 | 0x400;
	sregs->cs = seg_from_desc(KERNEL_CODE_SEG, 1);
	sregs->ds = seg_from_desc(DATA_SEG, 2);
	sregs->ss = seg_from_desc(DATA_SEG, 2);
	sregs->es = seg_from_desc(DATA_SEG, 2);
	sregs->fs = seg_from_desc(DATA_SEG, 2);
	sregs->gs = seg_from_desc(DATA_SEG, 2);
}

static void read_from_file(void *dst, char *fname, size_t offset, size_t len)
{
	int fd;

	fd = open(fname, O_RDONLY);
	lseek(fd, offset, 0);
	read(fd, dst, len);
	close(fd);
}

struct elf_vm_info {
	uint64_t start_addr;
};

static void print_nbytes(uint8_t *addr, size_t count)
{
	size_t i;

	for (i = 0; i < count; i++) {
		printf("%x ", addr[i]);
	}
	printf("\n");
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

static struct elf_vm_info load_elf(char *fname, int vmfd, struct kvm_sregs *sregs)
{
	struct elf64_program *program;
	struct elf_vm_info res = { 0 };
	size_t i, n = 1, pages_count;
	struct addr_pair mem;
	int err;
	void *stuff;

	struct kvm_userspace_memory_region sregion = {0};
	stuff = mmap(NULL, 0x5000, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	sregion.slot = 0;
	sregion.guest_phys_addr = GUEST_INFO_START;
	sregion.memory_size = 0x5000;
	sregion.userspace_addr = (uint64_t) stuff;
	memset(stuff, 0, 0x5000);
	init_gdt(stuff, sregs);

	err = ioctl(vmfd, KVM_SET_USER_MEMORY_REGION, &sregion);
	if (err) {
		printf("Failed to create memregion1: %d\n", err);
		exit(-1);
	}
	program = parse_elf(fname);
	if (!program) {
		printf("Failed to parse elf!\n");
		return res;
	}

	for (i = 0; i < program->elf_header->e_phnum; i++) {
		struct elf64_segment_hdr *sh = program->segment_headers[i];
		if (sh->p_type != 0x01)
			continue;
		printf("Reading %lx bytes from %lx offset into the memory at addr %lx\n", sh->p_filesz, sh->p_offset, sh->p_vaddr);
		pages_count = (sh->p_filesz + PAGE_SIZE - 1) / PAGE_SIZE;
		mem = alloc_pages_from_mpt(pages_count);
		map_range(sh->p_vaddr, mem.guest, pages_count);
		read_from_file((void *)mem.host, fname, sh->p_offset, sh->p_filesz);
		//print_nbytes(mem + (sh->p_vaddr - 0x400000), sh->p_filesz);
	}
	res.start_addr = program->elf_header->e_entry;

	free_elf(program);
	return res;
}

int start_vm(char *fname)
{
	int kvm, vmfd, vcpufd;
	void *segments[5];
	struct kvm_sregs sregs;
	struct kvm_run *run;
	size_t mmap_size;
	int ret;

	kvm = open("/dev/kvm", O_RDWR | O_CLOEXEC);
	ret = ioctl(kvm, KVM_CHECK_EXTENSION, KVM_CAP_USER_MEMORY);
	if (ret == -1) {
		printf("KVM_CHECK_EXTENSION is not available\n");
		return -1;
	}
	if (!ret) {
		printf("KVM_CAP_USER_MEMORY is not available\n");
		return -1;
	}
	vmfd = ioctl(kvm, KVM_CREATE_VM, (unsigned long)0);
	vcpufd = ioctl(vmfd, KVM_CREATE_VCPU, (unsigned long)0);
	ioctl(vcpufd, KVM_GET_SREGS, &sregs);
	init_page_tables(vmfd);

	map_range(GUEST_INFO_START, GUEST_INFO_START, 5);
	struct elf_vm_info info = load_elf(fname, vmfd, &sregs);
	printf("===PAGE TABLE STUFF===\n");
	print_page_mapping();
	printf("===PAGE TABLE STUFF===\n");
	setup_paging(&sregs);
	ioctl(vcpufd, KVM_SET_SREGS, &sregs);

	mmap_size = ioctl(kvm, KVM_GET_VCPU_MMAP_SIZE, NULL);
	run = mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, vcpufd, 0);	
	printf("Starting ELF at 0x%lx...\n", info.start_addr);
	struct kvm_regs regs = {
		.rip = info.start_addr,
		.rsp = STACK_START,
		.rbp = STACK_START,
		.rflags = 0x02,
	};
	ioctl(vcpufd, KVM_SET_REGS, &regs);
	while (1) {
		ioctl(vcpufd, KVM_RUN, NULL);
		switch (run->exit_reason) {
		case KVM_EXIT_HLT:
			printf("KVM_EXIT_HLT\n");
			goto clean;
			break;
		case KVM_EXIT_IO:
			if (run->io.direction == KVM_EXIT_IO_OUT
			    && run->io.port == 0x3f8)
				putchar(*(((char *)run) + run->io.data_offset));
			else {
				printf("Unhandled KVM_EXIT_IO\n");
			}
			break;
		case KVM_EXIT_FAIL_ENTRY:
			printf("KVM_EXIT_FAIL_ENTRY: 0x%lx\n",
			     (uint64_t)run->fail_entry.hardware_entry_failure_reason);
			break;
		case KVM_EXIT_INTERNAL_ERROR:
			printf("KVM_EXIT_INTERNAL_ERROR: suberror = 0x%x\n",
			     run->internal.suberror);
			goto clean;
			break;
		default:
			printf("Odd exit reason: %d\n", run->exit_reason);
			goto clean;
		}
	}
clean:
	close(kvm);
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		exit(-1);
	}
	
	start_vm(argv[1]);	
	return 0;
}
