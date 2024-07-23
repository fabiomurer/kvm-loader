#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include "elf.h"
#include "page.h"

#define GUEST_PT_ADDR 0xA000
#define MPT_COUNT_PAGES 0x100
#define PAGE_TABLES_SIZE (PAGE_SIZE * MPT_COUNT_PAGES)
#define PAGE_TABLES_SLOT 2
#define PTE_ENTRY_SIZE 8

static void *mpt;
static int init_page_table_space(int vmfd)
{
	int err;
	struct kvm_userspace_memory_region region = {};

	mpt = mmap(NULL, PAGE_TABLES_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	memset(mpt, 0, PAGE_TABLES_SIZE);

	region.slot = PAGE_TABLES_SLOT;
	region.guest_phys_addr = GUEST_PT_ADDR;
	region.memory_size = PAGE_TABLES_SIZE;
	region.userspace_addr = (uint64_t) mpt;

	err = ioctl(vmfd, KVM_SET_USER_MEMORY_REGION, &region);
	if (err) {
		printf("Failed to create region for page tables\n");
		return -1;
	}
}

static size_t page_num = 0;

struct alloc_result alloc_pages_from_mpt(size_t page_count)
{
	pt_addr res_host = (uint64_t)mpt + PAGE_SIZE * page_num;
	pt_addr res_guest = GUEST_PT_ADDR + PAGE_SIZE * page_num;

	if (page_num + page_count >= MPT_COUNT_PAGES)
		return (struct alloc_result){ .host = 0, .guest = 0 };

	page_num += page_count;
	memset((void *)res_host, 0, PAGE_SIZE * page_count);

	struct alloc_result res = {
		.size = PAGE_SIZE * page_count,
		.host = res_host,
		.guest = res_guest,
	};
	return res;
}

struct alloc_result alloc_pages_mapped(size_t page_count)
{
	struct alloc_result res = alloc_pages_from_mpt(page_count);
	map_range(res.guest, res.guest, page_count);
	return res;
}

static struct alloc_result pml4t_addr;
void init_page_tables(int vmfd)
{
	init_page_table_space(vmfd);
	pml4t_addr = alloc_pages_from_mpt(1);
}

struct alloc_result from_guest(pt_addr gaddr)
{
	gaddr = gaddr / PAGE_SIZE * PAGE_SIZE;
	struct alloc_result res = {
		.guest = gaddr,
		.host = (uint64_t)mpt + (gaddr - GUEST_PT_ADDR),
	};

	return res;
}

#define CR0_ENABLE_PAGING	(1ULL << 31)
#define CR4_ENABLE_PAE		(1ULL << 5)
void setup_paging(struct kvm_sregs *sregs)
{
	sregs->cr3 = (uint64_t)pml4t_addr.guest;
	sregs->cr4 |= CR4_ENABLE_PAE;
	sregs->cr0 |= CR0_ENABLE_PAGING;
}

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define PAGE_TABLE_LEVELS 4

#define SHIFT_LVL_0	39
#define SHIFT_LVL_1	30
#define SHIFT_LVL_2	21
#define SHIFT_LVL_3	12

#define PAGE_PRESENT	(1 << 0)
#define PAGE_RW		(1 << 1)

static pt_addr set_pte_flags(pt_addr e, uint64_t flags)
{
	return (pt_addr) ((uint64_t)e | flags);
}

int map_addr(uint64_t vaddr, uint64_t phys_addr)
{
	printf("Mapping %lx to %lx\n", vaddr, phys_addr);
	size_t i = 0;
	struct alloc_result cur_addr = pml4t_addr;
	uint64_t ind[PAGE_TABLE_LEVELS] = {
		(vaddr & _AC(0xff8000000000, ULL)) >> SHIFT_LVL_0,
		(vaddr & _AC(0x7fc0000000, ULL)) >> SHIFT_LVL_1,
		(vaddr & _AC(0x3fe00000, ULL)) >> SHIFT_LVL_2,
		(vaddr & _AC(0x1FF000, ULL)) >> SHIFT_LVL_3,
	};

	if (vaddr % PAGE_SIZE != 0)
		return -1;
	if (phys_addr % PAGE_SIZE != 0)
		return -1;
	
	for (i = 0; i < PAGE_TABLE_LEVELS; i++) {
		pt_addr *g_a = (pt_addr *)(cur_addr.host + ind[i] * sizeof(pt_addr));
		if (i == PAGE_TABLE_LEVELS - 1) {
			if (*g_a) {
				printf("%lx Already mapped to %lx!\n", vaddr, *g_a);
			}
			// Last page. Just set it to the physicall address
			*g_a = set_pte_flags((pt_addr)phys_addr, PAGE_PRESENT | PAGE_RW);
			break;
		}
		if (!*g_a) {
			printf("Allocating level %zu\n", i);
			*g_a = set_pte_flags(alloc_pages_from_mpt(1).guest, PAGE_PRESENT | PAGE_RW);
		}
		cur_addr = from_guest(*g_a);
	}
	
	return 0;
}

int map_range(pt_addr vaddr, pt_addr phys_addr, size_t pages_count)
{
	size_t mapped;
	printf("Mapping range from %lx to %lx\n", vaddr, vaddr + pages_count * PAGE_SIZE);

	for (mapped = 0; mapped < pages_count; mapped++)
		map_addr(vaddr + PAGE_SIZE * mapped, phys_addr + PAGE_SIZE * mapped);
	return 0;
}

static void follow_addr(pt_addr addr, int level)
{
	size_t i = 0;

	if (level == 4)
		return;
	for (i = 0; i < PAGE_SIZE / PTE_ENTRY_SIZE; i++) {
		pt_addr *p = (pt_addr *)(from_guest(addr).host + i * sizeof(pt_addr));
		if (*p) {
			printf("Entry %zu at level %d points to %lx\n", i, level, *p);
			follow_addr(*p, level + 1);
		}
	}
}

void print_page_mapping(void)
{
	printf("===PAGE TABLE STUFF===\n");
	follow_addr(pml4t_addr.guest, 0);
	printf("===PAGE TABLE STUFF===\n");
}
