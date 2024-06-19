#ifndef __KL_PAGING
#define __KL_PAGING
#include <stddef.h>
#include <stdint.h>
#include <linux/kvm.h>
#define PAGE_SIZE 4096
#define pt_addr uint64_t

struct addr_pair {
	pt_addr host;
	pt_addr guest;
};

int map_addr(uint64_t vaddr, uint64_t phys_addr);
int map_range(pt_addr vaddr, pt_addr phys_addr, size_t pages_count);
void print_page_mapping(void);
void init_page_tables(int vmfd);
void setup_paging(struct kvm_sregs *sregs);
struct addr_pair alloc_pages_from_mpt(size_t page_count);
#endif
