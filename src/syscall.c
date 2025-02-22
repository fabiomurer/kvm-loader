#include <asm/unistd_64.h>
#include <linux/kvm.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#include "syscall.h"
#include "page.h"

extern void *mpt;

// vlinux virtual linux

uint64_t vlinux_brk;

void* vm_guest_to_host(u_int64_t guest_addr, int vcpufd) {
	struct kvm_translation transl_addr;
	transl_addr.linear_address = guest_addr;

	if (ioctl(vcpufd, KVM_TRANSLATE, &transl_addr) < 0) {
		perror("KVM_TRANSLATE");
		exit(-1);
	}

	return (void*)(mpt + transl_addr.physical_address - GUEST_PT_ADDR);
}

bool is_syscall(struct kvm_regs* regs, int vcpufd) {

	u_int8_t* addr = (u_int8_t*)vm_guest_to_host(regs->rip, vcpufd); 

	__u64 rip_content = addr[1] | (addr[0] << 8);
	if (rip_content == SYSCALL_OPCODE) {
		return true;
	} else {
		return false;
	}
}

u_int64_t vlinux_syscall_brk(u_int64_t addr) {
	/*
	the actual Linux system call returns the new program
    break on success.  On failure, the system call returns the current
    break.
	*/
	if (addr == 0) {
		return vlinux_brk;
	}

	if (addr >= vlinux_brk) {
		u_int64_t increment = (u_int64_t)addr - vlinux_brk;
		
		struct alloc_result mem = map_guest_memory(vlinux_brk, increment);
		
		if (is_mapped_failed(&mem)) {
			fprintf(stderr, "brk failed\n");
			return vlinux_brk;
		} else {
			vlinux_brk += increment;
			return vlinux_brk;
		}
	} else {
		// shrink with brk not supported
		fprintf(stderr, "brk shrink not supported\n");
	}

	return vlinux_brk;
}


u_int64_t syscall_handler(struct kvm_regs* regs, int vcpufd) {
	u_int64_t sysno = regs->rax;
	u_int64_t arg1 = regs->rdi;
	u_int64_t arg2 = regs->rsi;
	u_int64_t arg3 = regs->rdx;
	u_int64_t ret = 0;

	switch (sysno) {
		case __NR_write:
			void* buf = vm_guest_to_host(arg2, vcpufd);

			printf("=======__NR_write\n");
			printf("fd: %d\n", (int)arg1);
			printf("buff: %s\n", (char*)buf);
			printf("len: %lu\n", arg3);
			printf("=======\n");

			ret = write((int)arg1, (char*)buf, arg3);
			if (ret == (u_int64_t)-1) {
				perror("__NR_write");
			} else {
				printf("byte written: %lu\n", ret);
			}
			break;

		case __NR_brk:
			printf("=======__NR_brk\n");
			printf("addr: %p\n", (void*)arg1);
			printf("=======\n");
			ret = vlinux_syscall_brk(arg1);
			break;

		case __NR_exit:
			printf("=======__NR_exit\n");
			printf("exit code: %d\n", (int)arg1);
			printf("=======\n");
			_exit(arg1);
			break;

		default:
			printf("ENOSYS, syscall number %d\n", (int)sysno);
			ret = -ENOSYS;
			sysno = ENOSYS; // return syscall not recognised
	}
	regs->rax = ret;
	return sysno;
}