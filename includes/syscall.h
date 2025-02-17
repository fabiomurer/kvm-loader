#ifndef __KL_SYSCALL
#define __KL_SYSCALL

#include <stdbool.h>
#include <linux/kvm.h>
#include <sys/types.h>

#define SYSCALL_OPCODE 0x0F05
#define SYSCALL_OP_SIZE 2

void* vm_guest_to_host(u_int64_t guest_addr, int vcpufd);

bool is_syscall(struct kvm_regs* regs, int vcpufd);

u_int64_t syscall_handler(struct kvm_regs* regs, int vcpufd);

#endif