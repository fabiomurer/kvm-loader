#include <stdint.h>
#define PAGE_SIZE 4096
#define pt_addr void *

int map_addr(uint64_t vaddr, uint64_t phys_addr);
int map_range(pt_addr vaddr, pt_addr phys_addr, size_t pages_count);
void follow_addr(pt_addr addr, int level);
