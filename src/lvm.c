#include <asm/kvm.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <cpuid.h>
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

void load_linux_cpu_state_kvm(int vcpufd, struct linux_cpu_state_kvm* cpu_state) {
	struct kvm_regs regs;
	struct kvm_fpu fpu;

	if (ioctl(vcpufd, KVM_GET_REGS, &regs) == -1) {
		perror("KVM_GET_REGS");
		exit(EXIT_FAILURE);
	}

	regs.rsp = cpu_state->kvm_regs.rsp;
	regs.rip = cpu_state->kvm_regs.rip;
	regs.rflags = cpu_state->kvm_regs.rflags;

	if (ioctl(vcpufd, KVM_SET_REGS, &regs) == -1) {
		perror("KVM_SET_REGS");
		exit(EXIT_FAILURE);
	}

	if (ioctl(vcpufd, KVM_GET_FPU, &fpu) == -1) {
		perror("KVM_GET_FPU");
		exit(EXIT_FAILURE);
	}

	fpu.fcw = cpu_state->kvm_fpu.fcw;
	fpu.ftwx = cpu_state->kvm_fpu.ftwx;
	fpu.mxcsr = cpu_state->kvm_fpu.mxcsr;

	if (ioctl(vcpufd, KVM_SET_FPU, &fpu) == -1) {
		perror("KVM_SET_FPU");
		exit(EXIT_FAILURE);
	}
}

void init_sse(int vcpufd) {
	struct kvm_sregs sregs;
	if (ioctl(vcpufd, KVM_GET_SREGS, &sregs) == -1) {
		perror("KVM_GET_SREGS");
		exit(EXIT_FAILURE);
	}
	/*
	clear the CR0.EM bit (bit 2) [ CR0 &= ~(1 << 2) ]
	set the CR0.MP bit (bit 1) [ CR0 |= (1 << 1) ]
	set the CR4.OSFXSR bit (bit 9) [ CR4 |= (1 << 9) ]
	set the CR4.OSXMMEXCPT bit (bit 10) [ CR4 |= (1 << 10) ]
	*/
	sregs.cr0 &= ~(1 << 2); // CR0_EM
	sregs.cr0 &= ~(1 << 3); // CR0_TS
	sregs.cr0 |= (1 << 1);
	sregs.cr4 |= (1 << 9);
	sregs.cr4 |= (1 << 10);

	// avx
	sregs.cr4 |= (1 << 18); // OS Support for XSAVE and related instructions. Indicates that the operating system can manage CPU extended states (e.g., for AVX).

	if (ioctl(vcpufd, KVM_SET_SREGS, &sregs) == -1) {
		perror("KVM_GET_SREGS");
		exit(EXIT_FAILURE);
	}
}

#define XCR0_X87    (1ULL << 0)  // x87 FPU/MMX state (must be 1)
#define XCR0_SSE    (1ULL << 1)  // SSE state
#define XCR0_AVX    (1ULL << 2)  // AVX state

int check_xcr0_support() {
    unsigned int eax, ebx, ecx, edx;
    
    // Check XSAVE feature flag
    __cpuid(1, eax, ebx, ecx, edx);
    if (!(ecx & (1 << 26))) {
        printf("CPU doesn't support XSAVE\n");
        return 0;
    }
    
    // Get XCR0 supported mask
    __cpuid_count(0xD, 0, eax, ebx, ecx, edx);
    printf("Host XCR0 supported mask: 0x%x\n", eax);
    return eax;
}

void init_avx(int kvm, int vcpufd) {
	check_xcr0_support();

	int xcr_capability = ioctl(kvm, KVM_CHECK_EXTENSION, KVM_CAP_XCRS);
    if (!xcr_capability) {
        printf("KVM_CAP_XCRS is not supported\n");
        exit(EXIT_FAILURE);
    }

	struct kvm_xcrs xcrs;
	memset(&xcrs, 0, sizeof(xcrs));

	// We're setting XCR0 register
    xcrs.nr_xcrs = 1; // nuber of xcrs
	xcrs.flags = 0;	  // should be 0 bho?

    xcrs.xcrs[0].xcr = 0;  // XCR0 register
    xcrs.xcrs[0].reserved = 0;
    // Enable x87 FPU, SSE, and AVX
    // All three must be enabled for AVX to work
    xcrs.xcrs[0].value = XCR0_X87 | XCR0_SSE | XCR0_AVX;
    
    // Apply the changes using KVM_SET_XCRS ioctl
    if (ioctl(vcpufd, KVM_SET_XCRS, &xcrs) < 0) {
        perror("KVM_SET_XCRS ioctl failed");
        exit(EXIT_FAILURE);
    }
}

void setup_sregs(int vcpufd)
{
	struct kvm_sregs sregs;
	
	if (ioctl(vcpufd, KVM_GET_SREGS, &sregs) == -1) {
		perror("KVM_GET_SREGS");
		exit(EXIT_FAILURE);
	}

	init_gdt(&sregs);
	setup_paging(&sregs);
	
	if (ioctl(vcpufd, KVM_SET_SREGS, &sregs) == -1) {
		perror("KVM_SET_SREGS");
		exit(EXIT_FAILURE);
	}
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

	setup_sregs(vcpufd);

	struct linux_cpu_state_kvm cpu_state = load_program(argv);

	load_linux_cpu_state_kvm(vcpufd, &cpu_state);

	init_sse(vcpufd);
	//init_avx(kvm, vcpufd);

	print_page_mapping();
	
	int err = ioctl(vcpufd, KVM_SET_REGS, &cpu_state.kvm_regs);
	if (err) {
		printf("Failed to set regs: %d\n", err);
		exit(EXIT_FAILURE);
	}

	err = ioctl(vcpufd, KVM_SET_FPU, &cpu_state.kvm_fpu);
	if (err) {
		printf("Failed to set fpu: %d\n", err);
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
