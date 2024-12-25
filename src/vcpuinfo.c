#include <asm/kvm.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/kvm.h>
#include <stdlib.h>

// https://www.sandpile.org/x86/except.htm
char exceptions_names[][30] = {
    "divide error", 
    "debug",
    "non-maskable interrupt",
    "breakpoint",
    "overflow",
    "boundary range exceeded",
    "undefined opcode",
    "device not available",
    "double fault",
    "reserved",
    "invalid TSS",
    "not present",
    "stack segment",
    "general protection",
    "page fault",
    "reserved",
    "math fault",
    "alignment checking",
    "machine check",
    "extended math fault",
    "virtualization exception",
    "control protection exception",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "HV injection exception",
    "VMM comm. exception",
    "security exception",
    "reserved"
};

void vcpu_events_logs(int kvm, int vcpufd) {
    struct kvm_vcpu_events events;

    if (ioctl(vcpufd, KVM_GET_VCPU_EVENTS, &events) < 0) {
        perror("KVM_GET_VCPU_EVENTS");
        exit(-1);
    }

    printf(
        "exceptions:\n\t"
        "injected: %d\n\t"
        "nr: %d [%s]\n\t"
        "has_error_code: %d\n\t"
        "pending: %d\n\t"
        "error_code: %d\n\t"
        "exception_has_payload: %d\n\t"
        "exception_payload: %p\n\t",
        events.exception.injected,
        events.exception.nr,
        exceptions_names[events.exception.nr],
        events.exception.has_error_code,
        events.exception.pending,
        events.exception.error_code,
        events.exception_has_payload,
        (void*)events.exception_payload
    );


}