#include "load_program.h"
#include "load_kvm.h"
#include "load_linux.h"

#include <asm/kvm.h>
#include <signal.h>
#include <linux/kvm.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>



struct linux_cpu_state_kvm load_program(char** argv) {
    struct user_regs_struct user_regs = {0};
    struct user_fpregs_struct user_fpregs = {0};

    pid_t pid = load_linux(argv, &user_regs, &user_fpregs);

    linux_to_kvm_map(pid);

    // kill traced process
    if (kill(pid, SIGKILL) == -1) {
        perror("kill");
        exit(EXIT_FAILURE);
    }
    
    int status;
    waitpid(pid, &status, 0);


    /*
| user_fpregs_struct Field | kvm_fpu Field        | Notes                                                                                       |
|--------------------------|----------------------|-------------------------------------------------------------------------------------------------|
| cwd                      | fcw                  | Both represent the FPU control word.                                                            |
| swd                      | fsw                  | Both represent the FPU status word.                                                             |
| ftw                      | ftwx                 | Both hold the tag word (ftwx in kvm_fpu is in the FXSAVE format).                               |
| fop                      | last_opcode          | fop indicates the last executed FPU opcode; last_opcode holds that value in kvm_fpu.            |
| rip                      | last_ip              | Both point to the last instruction pointer used in an FPU operation.                            |
| rdp                      | last_dp              | Both point to the last data (operand) pointer.                                                  |
| mxcsr                    | mxcsr                | Both represent the MXCSR register.                                                              |
| st_space[32]             | fpr[8][16]           | st_space holds 128 bytes (8 registers × 16 bytes each), mapping to the 8 FP registers in fpr.   |
| xmm_space[64]            | xmm[16][16]          | xmm_space holds 256 bytes (16 registers × 16 bytes each), mapping to the 16 XMM registers in xmm|
| mxcr_mask                | —                    | There is no direct equivalent in kvm_fpu.                                                       |
| padding[24]              | (pad1, pad2)         | kvm_fpu contains additional padding (pad1 and pad2) which differ from user_fpregs_struct.       |
    */
    struct kvm_regs kvm_regs = {
        .rax = user_regs.rax,
        .rbx = user_regs.rbx,
        .rcx = user_regs.rcx,
        .rdx = user_regs.rdx,
        .rsi = user_regs.rsi,
        .rdi = user_regs.rdi,
        .rsp = user_regs.rsp, // to set
        .rbp = user_regs.rbp,
        .r8 = user_regs.r8,
        .r9 = user_regs.r9,
        .r10 = user_regs.r10,
        .r11 = user_regs.r11,
        .r12 = user_regs.r12,
        .r13 = user_regs.r13,
        .r14 = user_regs.r14,
        .r15 = user_regs.r15,
        .rip = user_regs.rip, // to set
        .rflags = user_regs.eflags // to set TODO: controllare che non sovrascriva altri bit di rlfags (elfags sottotipo id rflags)
    };

    struct kvm_fpu kvm_fpu = {
        .fcw = user_fpregs.cwd, // to set
        .fsw = user_fpregs.swd,
        .ftwx = 0xFF,//user_fpregs.ftw, // to set (set all empty 0xFF)
        .last_opcode = user_fpregs.fop,
        .last_ip = user_fpregs.rip,
        .last_dp = user_fpregs.rdp,
        .mxcsr = user_fpregs.mxcsr & user_fpregs.mxcr_mask, // to set
    };

    struct linux_cpu_state_kvm cpu_state = {
        .kvm_regs = kvm_regs,
        .kvm_fpu = kvm_fpu,
    };


    return cpu_state;
}