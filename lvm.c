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

#define __pck __attribute__((packed))

#define GDT_ENTRY(flags, base, limit)			\
	((((base)  & _AC(0xff000000,ULL)) << (56-24)) |	\
	 (((flags) & _AC(0x0000f0ff,ULL)) << 40) |	\
	 (((limit) & _AC(0x000f0000,ULL)) << (48-16)) |	\
	 (((base)  & _AC(0x00ffffff,ULL)) << 16) |	\
	 (((limit) & _AC(0x0000ffff,ULL))))

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

	printf("gdt old base: %x gdt old limit: %x\n", sregs->gdt.base, sregs->gdt.limit);
	printf("idt old base: %x idt old limit: %x\n", sregs->idt.base, sregs->idt.limit);
	sregs->gdt.base = GDT_OFFSET + GUEST_INFO_START;
	sregs->gdt.limit = 3 * 8 - 1;
	printf("GDT base: %llu GDT limit: %llu\n", sregs->gdt.base, sregs->gdt.limit);
	memset(mem + IDT_OFFSET + GUEST_INFO_START, 0, 8);
	sregs->idt.base = IDT_OFFSET + GUEST_INFO_START;
	sregs->idt.limit = 7;
	sregs->cr0 |= 1 | (0x80000000ULL);
	sregs->efer |= 0x100 | 0x400;
	printf("Efer: %llx\n", sregs->efer);
	printf("cs limit: %x cs base: %x\n", sregs->cs.limit, sregs->cs.base);
	sregs->cs = seg_from_desc(KERNEL_CODE_SEG, 1);
	sregs->ds = seg_from_desc(DATA_SEG, 2);
	sregs->ss = seg_from_desc(DATA_SEG, 2);
	sregs->es = seg_from_desc(DATA_SEG, 2);
	sregs->fs = seg_from_desc(DATA_SEG, 2);
	sregs->gs = seg_from_desc(DATA_SEG, 2);
	printf("GDT CS base: %llx\n", sregs->ds.base);
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

size_t floor_up(size_t x)
{
	return (x + SEG_SIZE - 1) / SEG_SIZE * SEG_SIZE;
}

size_t floor_down(size_t x)
{
	return x - (x % SEG_SIZE);
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
	struct elf32_program *program;
	struct elf_vm_info res = { 0 };
	size_t i, n = 1;
	void *mem;
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

	struct seg_desc *p = (struct seg_desc *)(stuff + GDT_OFFSET + 16);
	print_seg(p);

	err = ioctl(vmfd, KVM_SET_USER_MEMORY_REGION, &sregion);
	if (err) {
		printf("Failed to create memregion1: %d\n", err);
		exit(-1);
	}
	mem = mmap(NULL, 0xF0000, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	memset(mem, 0, 0xF0000);
	struct kvm_userspace_memory_region region = {};
	region.slot = 1;
	region.guest_phys_addr = 0x8000000;
	region.memory_size = 0xF0000;
	region.userspace_addr = (uint64_t)mem;
	err = ioctl(vmfd, KVM_SET_USER_MEMORY_REGION, &region);
	if (err) {
		printf("Failed to create memregion: %d\n", err);
		exit(-1);
	}
	program = parse_elf(fname);
	if (!program) {
		printf("Failed to parse elf!\n");
		return res;
	}

	for (i = 0; i < program->elf_header->e_phnum; i++) {
		struct elf32_segment_hdr *sh = program->segment_headers[i];
		if (sh->p_type != 0x01)
			continue;
		read_from_file(mem + (sh->p_vaddr - 0x8000000), fname, sh->p_offset, sh->p_filesz);
		print_nbytes(mem + (sh->p_vaddr - 0x8000000), sh->p_filesz);
	}
	res.start_addr = program->elf_header->e_entry;

	free_elf(program);
	return res;
}

static void *mpt;
#define GUEST_PT_ADDR 0xA000
static int init_page_table_space(int vmfd)
{
	int err;

	mpt = mmap(NULL, 0x66000, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	memset(mpt, 0, 0x66000);

	struct kvm_userspace_memory_region region = {};
	region.slot = 2;
	region.guest_phys_addr = GUEST_PT_ADDR;
	region.memory_size = 0x66000;
	region.userspace_addr = (uint64_t) mpt;

	err = ioctl(vmfd, KVM_SET_USER_MEMORY_REGION, &region);
	if (err) {
		printf("Failed to create region for page tables\n");
		return -1;
	}
}

static size_t page_num = 0;
#define PAGE_SIZE 4096

#define pt_addr void *

struct addr_pair {
	pt_addr host;
	pt_addr guest;
};

static struct addr_pair alloc_page_from_mpt(void)
{
	pt_addr res_host = mpt + PAGE_SIZE * page_num;
	pt_addr res_guest = GUEST_PT_ADDR + PAGE_SIZE * page_num;
	page_num++;
	memset(res_host, 0, PAGE_SIZE);

	struct addr_pair res = {
		.host = res_host,
		.guest = res_guest,
	};
	return res;
}

static struct addr_pair pml4t_addr;
void init_page_tables(int vmfd)
{
	init_page_table_space(vmfd);
	pml4t_addr = alloc_page_from_mpt();
}

struct addr_pair from_guest(pt_addr gaddr)
{
	gaddr = (pt_addr) ((uint64_t) gaddr / PAGE_SIZE * PAGE_SIZE);
	struct addr_pair res = {
		.guest = gaddr,
		.host = (pt_addr)((uint64_t)mpt + (uint64_t)(gaddr - GUEST_PT_ADDR)),
	};

	return res;
}

int map_addr(uint64_t vaddr, uint64_t phys_addr)
{
	if (vaddr % PAGE_SIZE != 0)
		return -1;
	if (phys_addr % PAGE_SIZE != 0)
		return -1;

	uint64_t ind[4] = {
		(vaddr & _AC(0xff8000000000, ULL)) >> 39,
		(vaddr & _AC(0x7fc0000000, ULL)) >> 30,
		(vaddr & _AC(0x3fe00000, ULL)) >> 21,
		(vaddr & _AC(0x1FF000, ULL)) >> 12,
	};
	printf("Mapping address %lx\n", vaddr);
	size_t i = 0;
	for (i = 0; i < 4; i++)
		printf("Ind %d: %lx ", i, ind[i]);
	printf("\n");
	struct addr_pair cur_addr = pml4t_addr;
	for (i = 0; i < 4; i++) {
		pt_addr *g_a = (pt_addr *)(cur_addr.host + ind[i] * sizeof(pt_addr));
		if (i == 3) {
			*g_a = (pt_addr)(phys_addr | 0x3);
			break;
		}
		if (!*g_a) {
			printf("Allocating level %d\n", i);
			*g_a = (pt_addr) ((uint64_t)alloc_page_from_mpt().guest | 0x03);
		} else {
			printf("Next exists! %p\n", *g_a);
		}
		cur_addr = from_guest(*g_a);
	}
	
	return 0;
}

int map_range(pt_addr vaddr, pt_addr phys_addr, size_t pages_count)
{
	size_t mapped;

	for (mapped = 0; mapped <= pages_count; mapped++)
		map_addr(vaddr + PAGE_SIZE * mapped, phys_addr + PAGE_SIZE * mapped);
	return 0;
}

void follow_addr(pt_addr addr, int level)
{
	if (level == 4)
		return;
	size_t i = 0;
	for (i = 0; i < 512; i++) {
		pt_addr *p = (pt_addr *)(from_guest(addr).host + i * sizeof(pt_addr));
		if (*p) {
			printf("Entry %d at level %d points to %p\n", i, level,
				*p);
			follow_addr(*p, level + 1);
		}
	}
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
	printf("KVM: %d\n", kvm);
	ret = ioctl(kvm, KVM_CHECK_EXTENSION, KVM_CAP_USER_MEMORY);
	if (ret == -1)
		err(1, "KVM_CHECK_EXTENSION");
	if (!ret)
		err(1, "KVM_CAP_USER_MEMORY is not available");
	
	vmfd = ioctl(kvm, KVM_CREATE_VM, (unsigned long)0);
	vcpufd = ioctl(vmfd, KVM_CREATE_VCPU, (unsigned long)0);
	ioctl(vcpufd, KVM_GET_SREGS, &sregs);
	init_page_tables(vmfd);

	printf("===PAGE TABLE STUFF===\n");
	map_range(GUEST_INFO_START, GUEST_INFO_START, 5);
	map_addr(0x8049000, 0x8049000);
	follow_addr(pml4t_addr.guest, 0);
	printf("===PAGE TABLE STUFF===\n");
	
	struct elf_vm_info info = load_elf(fname, vmfd, &sregs);
	sregs.cr3 = pml4t_addr.guest;
	sregs.cr4 |= 0x20;
	ioctl(vcpufd, KVM_SET_SREGS, &sregs);

	mmap_size = ioctl(kvm, KVM_GET_VCPU_MMAP_SIZE, NULL);
	run = mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, vcpufd, 0);	
	printf("Starting at %llx\n", info.start_addr);
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
	uint64_t entry = GDT_ENTRY(0xa09b, 0x0, 0xfffff);
	struct seg_desc *seg = (struct seg_desc *)&entry;
	printf("Sizeof: %d\n", sizeof(*seg));
	print_seg(seg);

	printf("S: %d\n", sizeof(struct seg_desc));
	if (argc != 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		exit(-1);
	}
	
	start_vm(argv[1]);	
	return 0;
}
