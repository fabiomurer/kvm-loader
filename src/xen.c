#include <stdint.h>
#include <string.h>
#include "xen.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

// nop; ret;
static const uint8_t nop[] = {0x90, 0xc3};
static const uint8_t hypercall[] = {0xb8, 0x00, 0x00, 0x00, 0x00, 0xe7, 0x39};

static void write_hypercall(pt_addr table_addr, size_t num)
{
	uint8_t buf[7];
	memcpy(&buf, &hypercall, 7);

	buf[1] = num & 0xFF;
	buf[2] = (num & 0xFF00) >> 8;
	buf[3] = (num & 0xFF0000) >> 16;
	buf[4] = (num & 0xff000000) >> 24;

	memcpy((void *)(table_addr + num * 32), &buf, 7);
}

void init_hypercalls_page(pt_addr addr)
{
	size_t i;
	struct alloc_result mem = alloc_pages_from_mpt(1);
	map_addr(addr, mem.guest);

	for (i = 0; i < PAGE_SIZE / 32; i++)
		memcpy((void *)(mem.host + i * 32), &nop, ARRAY_SIZE(nop));

	write_hypercall(mem.host, 18);
}
