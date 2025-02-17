#ifndef __KL_VCPUINFO
#define __KL_VCPUINFO

void vcpu_events_logs(int kvm, int vcpufd);

void vcpu_regs_log(int kvm, int vcpufd);

#endif