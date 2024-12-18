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
#include "page.h"
#include "descriptors.h"
#include "xen.h"
#include "syscall.h"

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

static struct elf_vm_info load_elf(char *fname, int vmfd)
{
	struct elf64_program *program;
	struct elf_vm_info res = { 0 };
	size_t i, n = 1, pages_count;
	struct alloc_result mem;
	int err;
	void *stuff;

	program = parse_elf(fname);
	if (!program) {
		printf("Failed to parse elf!\n");
		return res;
	}

	for (i = 0; i < program->elf_header->e_phnum; i++) {
		struct elf64_segment_hdr *sh = program->segment_headers[i];
		if (sh->p_type != 0x01)
			continue;
		printf("Reading %lx bytes from %lx offset into the memory at addr %lx\n", sh->p_filesz, sh->p_offset, sh->p_vaddr);
		pages_count = (sh->p_filesz + PAGE_SIZE - 1) / PAGE_SIZE;
		mem = alloc_pages_from_mpt(pages_count);
		map_range(sh->p_vaddr, mem.guest, pages_count);
		read_from_file((void *)mem.host, fname, sh->p_offset, sh->p_filesz);
	}
	res.start_addr = program->elf_header->e_entry;

	free_elf(program);
	return res;
}

static uint64_t alloc_stack(void)
{
	struct alloc_result stack = alloc_pages_mapped(2);
	printf("Stack: %lx\n", stack.guest);

	return stack.guest + stack.size - 8;
}

static int setup_sregs(int vcpufd, int vmfd)
{
	struct kvm_sregs sregs;
	int err;

	err = ioctl(vcpufd, KVM_GET_SREGS, &sregs);
	if (err) {
		printf("Failed to get sregs: %d\n", err);
		return err;
	}

	init_gdt(&sregs);
	setup_paging(&sregs);
	err = ioctl(vcpufd, KVM_SET_SREGS, &sregs);
	if (err)
		printf("Failed to set sregs: %d\n", err);

	return err;
}

static int setup_regs(int vcpufd, uint64_t start_addr, uint64_t stack_addr)
{
	int err;
	struct kvm_regs regs = {
		.rip = start_addr,
		.rsp = stack_addr,
		.rbp = stack_addr,
		.rflags = 0x02,
	};

	err = ioctl(vcpufd, KVM_SET_REGS, &regs);
	if (err)
		printf("Failed to set regs: %d\n", err);
	return err;
}

static int vm_cycle(int kvm, int vcpufd)
{
	size_t mmap_size;
	struct kvm_run *run;

	mmap_size = ioctl(kvm, KVM_GET_VCPU_MMAP_SIZE, NULL);
	run = mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, vcpufd, 0);

	while (1) {
		ioctl(vcpufd, KVM_RUN, NULL);
		switch (run->exit_reason) {
		case KVM_EXIT_HLT:
			printf("KVM_EXIT_HLT\n");
			return 0;
		case KVM_EXIT_IO:
			if (run->io.direction == KVM_EXIT_IO_OUT
			    && run->io.port == 0x3f8)
				putchar(*(((char *)run) + run->io.data_offset));
			else if (run->io.direction == KVM_EXIT_IO_OUT && run->io.port == 0x39) {
				uint32_t num = *((uint32_t *)((uint8_t *)run + run->io.data_offset));
				printf("Hypercall number: %d\n", num);
			}
			else {
				printf("Unhandled KVM_EXIT_IO\n");
			}
			break;
		case KVM_EXIT_SHUTDOWN:
			struct kvm_regs regs;
			if (ioctl(vcpufd, KVM_GET_REGS, &regs) < 0) {
				perror("KVM_GET_REGS");
				exit(-1);
			}

			if (is_syscall(&regs, vcpufd)) {
				syscall_handler(&regs, vcpufd);
				
				regs.rip += SYSCALL_OP_SIZE;

				if (ioctl(vcpufd, KVM_SET_REGS, &regs) < 0) {
					perror("KVM_GET_REGS");
					exit(-1);
				}
			} else {
				printf("unespected shutdown\n");
				exit(-1);
			}
			break;
		case KVM_EXIT_FAIL_ENTRY:
			printf("KVM_EXIT_FAIL_ENTRY: 0x%lx\n",
			     (uint64_t)run->fail_entry.hardware_entry_failure_reason);
			break;
		case KVM_EXIT_INTERNAL_ERROR:
			printf("KVM_EXIT_INTERNAL_ERROR: suberror = 0x%x\n",
			     run->internal.suberror);
			return -1;
		default:
			printf("Odd exit reason: %d\n", run->exit_reason);
			return -1;
		}
	}

	return 0;
}

static int start_vm(char *fname)
{
	int kvm, vmfd, vcpufd;
	struct elf_vm_info info;
	int ret;
	uint64_t stack_addr;

	kvm = open("/dev/kvm", O_RDWR | O_CLOEXEC);

	ret = ioctl(kvm, KVM_CHECK_EXTENSION, KVM_CAP_USER_MEMORY);
	if (ret == -1) {
		printf("KVM_CHECK_EXTENSION is not available\n");
		return -1;
	}
	if (!ret) {
		printf("KVM_CAP_USER_MEMORY is not available\n");
		return -1;
	}

	vmfd = ioctl(kvm, KVM_CREATE_VM, (unsigned long)0);
	vcpufd = ioctl(vmfd, KVM_CREATE_VCPU, (unsigned long)0);
	init_page_tables(vmfd);

	ret = setup_sregs(vcpufd, vmfd);
	if (ret)
		return ret;

	info = load_elf(fname, vmfd);
	stack_addr = alloc_stack();
	//init_hypercalls_page(0x2000);
	print_page_mapping();
	printf("Starting ELF at 0x%lx...\n", info.start_addr);

	ret = setup_regs(vcpufd, info.start_addr, stack_addr);
	if (ret)
		return ret;

	ret = vm_cycle(kvm, vcpufd);
clean:
	close(kvm);
	return ret;
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		exit(-1);
	}
	
	return start_vm(argv[1]);
}
