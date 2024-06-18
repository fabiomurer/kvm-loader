#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "elf.h"

static int read_elf_header(int fd, struct elf32_header *result)
{
	lseek(fd, 0, 0);
	read(fd, result, sizeof(*result));

	return 0;
}

static int read_seg_headers(int fd, struct elf32_header *elf_hdr, struct elf32_segment_hdr **res)
{
	size_t i; 
	lseek(fd, elf_hdr->e_phoff, 0);
	
	for (i = 0; i < elf_hdr->e_phnum; i++) {
		res[i] = malloc(sizeof(*res[i]));
		if (!res[i]) {
			printf("Failed to malloc a segment table entry!\n");
		}
		read(fd, res[i], sizeof(*res[i]));
	}

	return 0;
}

void print_ident(struct elf32_header *hdr)
{
	uint32_t i;

	for (i = 0; i < sizeof(hdr->e_ident); i++) {
		printf("%02x ", hdr->e_ident[i]);
	}
	printf("\n");
}

static int alloc_segment_headers(struct elf32_program *elf)
{
	elf->segment_headers = malloc(sizeof(*elf->segment_headers) * elf->elf_header->e_phnum);
	if (!elf->segment_headers)
		return -ENOMEM;
	return 0;
}

struct elf32_program *parse_elf(char *filename)
{
	struct elf32_program *elf;
	int fd;

	elf = malloc(sizeof(*elf));
	if (!elf) {
		printf("Can't allocate memory for the elf32_program!\n");
		return NULL;
	}

	elf->elf_header = malloc(sizeof(*elf->elf_header));
	if (!elf->elf_header) {
		free(elf);
		printf("Can't alloc memory for elf header\n");
		return NULL;
	}

	fd = open(filename, O_RDONLY);
	read_elf_header(fd, elf->elf_header);
	alloc_segment_headers(elf);
	read_seg_headers(fd, elf->elf_header, elf->segment_headers);
	close(fd);

	return elf;
}

void free_elf(struct elf32_program *elf)
{
	size_t i;
	for (i = 0; i < elf->elf_header->e_phnum; i++) {
		free(elf->segment_headers[i]);
	}
	free(elf->segment_headers);
	free(elf->elf_header);
	free(elf);
}
