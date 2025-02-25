#pragma once 

#include <linux/kvm.h>

struct linux_cpu_state_kvm {
    struct kvm_regs kvm_regs;
    struct kvm_fpu kvm_fpu;
};

struct linux_cpu_state_kvm load_program(char** argv);