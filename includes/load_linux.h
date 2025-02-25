#pragma once

#include <sys/types.h>
#include <sys/user.h>
pid_t load_linux(char** argv, struct user_regs_struct* user_regs, struct user_fpregs_struct* user_fpregs);