#ifndef __KL_SYSCALL
#define __KL_SYSCALL

#include <stdbool.h>
#include <linux/kvm.h>

#define SYSCALL_OPCODE 0x0F05
#define SYSCALL_OP_SIZE 2

bool is_syscall(struct kvm_regs* regs, int vcpufd);

void syscall_handler(struct kvm_regs* regs, int vcpufd);

#endif