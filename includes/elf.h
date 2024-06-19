#include <stdint.h>

struct __attribute__((packed)) elf32_header {
	uint8_t e_ident[16];
	uint16_t e_type;
	uint16_t e_machine;
	uint32_t e_version;
	uint32_t e_entry;
	uint32_t e_phoff;
	uint32_t e_shoff;
	uint32_t e_flags;
	uint16_t e_ehsize;
	uint16_t e_phentsize;
	uint16_t e_phnum;
	uint16_t e_shentsize;
	uint16_t e_shnum;
	uint16_t e_shstrndx;
};

struct __attribute__((packed)) elf32_segment_hdr {
	uint32_t p_type;
	uint32_t p_offset;
	uint32_t p_vaddr;
	uint32_t p_paddr;
	uint32_t p_filesz;
	uint32_t p_memsz;
	uint32_t p_flags;
	uint32_t p_align;
};

struct elf32_program {
	struct elf32_header *elf_header;
	struct elf32_segment_hdr **segment_headers;
};

void print_ident(struct elf32_header *hdr);
struct elf32_program *parse_elf(char *filename);
void free_elf(struct elf32_program *elf);
