#include <stdbool.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include "page.h"

#define MPT_COUNT_PAGES 0x1000
#define PAGE_TABLES_SIZE (PAGE_SIZE * MPT_COUNT_PAGES)
#define PAGE_TABLES_SLOT 2
#define PTE_ENTRY_SIZE 8

#define ALIGN		(PAGE_SIZE - 1)
// 4096 -> 8192 (4096*2)
#define ROUND_PG(x)	(((x) + (ALIGN)) & ~(ALIGN))
// 4095 -> 0
#define TRUNC_PG(x)	((x) & ~(ALIGN))

// page table start address in host
void *mpt;

// alloc memory for host
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
		perror("Failed to create region for page tables\n");
		return -1;
	}
	return 0;
}

// first empty page
static size_t page_num = 0;


#define ALLOC_FAILED ((struct alloc_result){ .size = 0, .host = 0, .guest = 0 })

bool is_mapped_failed(struct alloc_result* mem) {
	if ((mem->host == 0) && (mem->guest == 0)) return true;
	else return false;
}

// reserve memory in the mpt
struct alloc_result alloc_pages_from_mpt(size_t page_count)
{
	// start address of free page in host
	pt_addr res_host = (uint64_t)mpt + PAGE_SIZE * page_num;
	// start address of free page in guest
	pt_addr res_guest = GUEST_PT_ADDR + PAGE_SIZE * page_num;

	// not enought free pages, dont alloc
	if (page_num + page_count >= MPT_COUNT_PAGES)
		return ALLOC_FAILED;

	// pages now are used
	page_num += page_count;
	// initialize mem to 0
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

// first page level
static struct alloc_result pml4t_addr;
void init_page_tables(int vmfd)
{
	init_page_table_space(vmfd);
	// first page is for plm4
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

// set 4 level page table for address translation
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

	// if not alligned
	if (vaddr % PAGE_SIZE != 0)
		return -1;
	if (phys_addr % PAGE_SIZE != 0)
		return -1;
	
	// map page walk
	for (i = 0; i < PAGE_TABLE_LEVELS; i++) {
		pt_addr *g_a = (pt_addr *)(cur_addr.host + ind[i] * sizeof(pt_addr));
		
		// if last level
		if (i == PAGE_TABLE_LEVELS - 1) {
			// is alredy mapped (not 0)
			if (*g_a) {
				printf("%lx Already mapped to %lx!\n", vaddr, *g_a);
				return 0;
			}
			// Last page. Just set it to the physicall address
			*g_a = set_pte_flags((pt_addr)phys_addr, PAGE_PRESENT | PAGE_RW);
			break;
		}
		// if the part of the current level is not mapped, map it
		if (!*g_a) {
			printf("Allocating level %zu\n", i);
			*g_a = set_pte_flags(alloc_pages_from_mpt(1).guest, PAGE_PRESENT | PAGE_RW);
		}
		cur_addr = from_guest(*g_a);
	}
	
	return 0;
}

bool segment_already_mapped(uint64_t vaddr) {
	size_t i = 0;

	struct alloc_result cur_addr = pml4t_addr;
	uint64_t ind[PAGE_TABLE_LEVELS] = {
		(vaddr & _AC(0xff8000000000, ULL)) >> SHIFT_LVL_0,
		(vaddr & _AC(0x7fc0000000, ULL)) >> SHIFT_LVL_1,
		(vaddr & _AC(0x3fe00000, ULL)) >> SHIFT_LVL_2,
		(vaddr & _AC(0x1FF000, ULL)) >> SHIFT_LVL_3,
	};

	// if not alligned
	vaddr = TRUNC_PG(vaddr);
	
	// map page walk
	for (i = 0; i < PAGE_TABLE_LEVELS; i++) {
		pt_addr *g_a = (pt_addr *)(cur_addr.host + ind[i] * sizeof(pt_addr));
		
		// if last level
		if (i == PAGE_TABLE_LEVELS - 1) {
			if (*g_a) 	return true;
			else 		return false;
		}
		// if the part of the current level is not mapped, page is not mapped
		if (!*g_a) return false;

		cur_addr = from_guest(*g_a);
	}
	return false;
}

// intruct mmu where to find pages
int map_range(pt_addr vaddr, pt_addr phys_addr, size_t pages_count)
{
	size_t mapped;
	printf("Mapping range from %lx to %lx\n", vaddr, vaddr + pages_count * PAGE_SIZE);

	for (mapped = 0; mapped < pages_count; mapped++)
		map_addr(vaddr + PAGE_SIZE * mapped, phys_addr + PAGE_SIZE * mapped);
	return 0;
}

// simirla to mmap(vaddr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
struct alloc_result map_guest_memory(uint64_t guest_vaddr, ssize_t length) {

	uint64_t aligned_addr = TRUNC_PG(guest_vaddr);
	
	ssize_t pages_count = ((length + (guest_vaddr - aligned_addr)) / PAGE_SIZE);
	

	struct alloc_result mem = alloc_pages_from_mpt(pages_count);
	
	if (!is_mapped_failed(&mem)) 
		map_range(aligned_addr, mem.guest, pages_count);
	
	return mem;
}

// debugging and info

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
