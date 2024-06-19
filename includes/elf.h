#ifndef __KL_ELF
#define __KL_ELF
#include <stdint.h>

struct __attribute__((packed)) elf64_header {
	uint8_t e_ident[16];
	uint16_t e_type;
	uint16_t e_machine;
	uint32_t e_version;
	uint64_t e_entry;
	uint64_t e_phoff;
	uint64_t e_shoff;
	uint32_t e_flags;
	uint16_t e_ehsize;
	uint16_t e_phentsize;
	uint16_t e_phnum;
	uint16_t e_shentsize;
	uint16_t e_shnum;
	uint16_t e_shstrndx;
};

struct __attribute__((packed)) elf64_segment_hdr {
	uint32_t p_type;
	uint32_t p_flags;
	uint64_t p_offset;
	uint64_t p_vaddr;
	uint64_t p_paddr;
	uint64_t p_filesz;
	uint64_t p_memsz;
	uint64_t p_align;
};

struct elf64_program {
	struct elf64_header *elf_header;
	struct elf64_segment_hdr **segment_headers;
};

void print_ident(struct elf64_header *hdr);
struct elf64_program *parse_elf(char *filename);
void free_elf(struct elf64_program *elf);
#endif
