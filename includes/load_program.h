#pragma once 

#include <linux/kvm.h>

struct kvm_regs load_program(char** argv);