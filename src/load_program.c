#include "load_program.h"
#include "load_kvm.h"
#include "load_linux.h"

#include <asm/kvm.h>
#include <linux/kvm.h>
#include <sys/types.h>
#include <sys/user.h>

struct kvm_regs load_program(char** argv) {
    struct user_regs_struct user_regs = {0};
    pid_t pid = load_linux(argv, &user_regs);

    linux_to_kvm_map(pid);

    struct kvm_regs kvm_regs = {
        .rax = user_regs.rax,
        .rbx = user_regs.rbx,
        .rcx = user_regs.rcx,
        .rdx = user_regs.rdx,
        .rsi = user_regs.rsi,
        .rdi = user_regs.rdi,
        .rsp = user_regs.rsp,
        .rbp = user_regs.rbp,
        .r8 = user_regs.r8,
        .r9 = user_regs.r9,
        .r10 = user_regs.r10,
        .r11 = user_regs.r11,
        .r12 = user_regs.r12,
        .r13 = user_regs.r13,
        .r14 = user_regs.r14,
        .r15 = user_regs.r15,
        .rip = user_regs.rip,
        .rflags = 0x02 // has to be 1 second bit 
    };


    return kvm_regs;
}