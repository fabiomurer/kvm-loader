#ifndef __KL_PAGING
#define __KL_PAGING
#include <stddef.h>
#include <stdint.h>
#include <linux/kvm.h>
#define PAGE_SIZE 4096
#define GUEST_PT_ADDR 0xA000
#define pt_addr uint64_t

struct alloc_result {
	size_t size;
	pt_addr host;
	pt_addr guest;
};

int map_addr(uint64_t vaddr, uint64_t phys_addr);
int map_range(pt_addr vaddr, pt_addr phys_addr, size_t pages_count);
void print_page_mapping(void);
void init_page_tables(int vmfd);
void setup_paging(struct kvm_sregs *sregs);
struct alloc_result alloc_pages_mapped(size_t page_count);
struct alloc_result alloc_pages_from_mpt(size_t page_count);
#endif
