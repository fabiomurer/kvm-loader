#include <asm/kvm.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/ioctl.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/kvm.h>
#include <sys/mman.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include "load_program.h"
#include "page.h"
#include "descriptors.h"
#include "syscall.h"
#include "vcpuinfo.h"

void init_sse(struct kvm_sregs* sregs) {
	/*
	clear the CR0.EM bit (bit 2) [ CR0 &= ~(1 << 2) ]
	set the CR0.MP bit (bit 1) [ CR0 |= (1 << 1) ]
	set the CR4.OSFXSR bit (bit 9) [ CR4 |= (1 << 9) ]
	set the CR4.OSXMMEXCPT bit (bit 10) [ CR4 |= (1 << 10) ]
	*/
	sregs->cr0 &= ~(1 << 2);
	sregs->cr0 |= (1 << 1);
	sregs->cr4 |= (1 << 9);
	sregs->cr4 |= (1 << 10);
}

static int setup_sregs(int vcpufd)
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
	init_sse(&sregs);
	err = ioctl(vcpufd, KVM_SET_SREGS, &sregs);
	if (err)
		printf("Failed to set sregs: %d\n", err);

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

		printf("exit reason: %d\n", run->exit_reason);

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
				
				if (syscall_handler(&regs, vcpufd) == ENOSYS) {
					printf("syscall num: %lld not supported\n", regs.rax);
					exit(-1);
				}
				
				regs.rip += SYSCALL_OP_SIZE;

				if (ioctl(vcpufd, KVM_SET_REGS, &regs) < 0) {
					perror("KVM_GET_REGS");
					exit(-1);
				}
			} else {
				printf("unespected shutdown\n");
				vcpu_events_logs(kvm, vcpufd);
				vcpu_regs_log(kvm, vcpufd);
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

static int start_vm(char** argv)
{
	int kvm, vmfd, vcpufd;

	int ret;

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

	ret = setup_sregs(vcpufd);
	if (ret)
		return ret;

	struct kvm_regs regs = load_program(argv);

	print_page_mapping();
	
	int err = ioctl(vcpufd, KVM_SET_REGS, &regs);
	if (err) {
		printf("Failed to set regs: %d\n", err);
		exit(EXIT_FAILURE);
	}

	ret = vm_cycle(kvm, vcpufd);

	close(kvm);
	return ret;
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		exit(-1);
	}
	
	return start_vm(&argv[1]);
}
