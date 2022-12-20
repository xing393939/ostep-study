#define _GNU_SOURCE

#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/bpf.h>
#include <linux/version.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <errno.h>

int bpf_create_map(enum bpf_map_type map_type,
                   unsigned int key_size,
                   unsigned int value_size,
                   unsigned int max_entries) {
    union bpf_attr attr = {
            .map_type = map_type,
            .key_size = key_size,
            .value_size = value_size,
            .max_entries = max_entries
    };
    return bpf(BPF_MAP_CREATE, &attr, sizeof(attr));
}

int main() {
    int bfd;
    unsigned char buf[1024] = {};
    struct bpf_insn *insn;
    union bpf_attr attr = {};
    unsigned char log_buf[4096] = {};
    int ret;
    int efd;
    int pfd;
    int n;
    int i;
    struct perf_event_attr pattr = {};

    bfd = open("./test_bpf", O_RDONLY);
    if (bfd < 0) {
        printf("open eBPF program error: %s\n", strerror(errno));
        exit(-1);
    }
    n = read(bfd, buf, 1024);
    for (i = 0; i < n; ++i) {
        printf("%02x ", buf[i]);
        if ((i + 1) % 8 == 0)
            printf("\n");
    }
    close(bfd);
    insn = (struct bpf_insn *) buf;
    strcpy(attr.prog_name, "mymain");
    attr.prog_type = BPF_PROG_TYPE_TRACEPOINT;
    attr.insns = (unsigned long) insn;
    attr.insn_cnt = n / sizeof(struct bpf_insn);
    attr.license = (unsigned long) "GPL";
    attr.log_size = sizeof(log_buf);
    attr.log_buf = (unsigned long) log_buf;
    attr.log_level = 1;
    // see LINUX_VERSION_CODE in /usr/include/linux/version.h
    attr.kern_version = 331580;
    pfd = syscall(SYS_bpf, BPF_PROG_LOAD, &attr, sizeof(attr));
    if (pfd < 0) {
        printf("bpf syscall error: %s\n", strerror(errno));
        printf("log_buf = %s\n", log_buf);
        exit(-1);
    }

    pattr.type = PERF_TYPE_TRACEPOINT;
    pattr.sample_type = PERF_SAMPLE_RAW;
    pattr.sample_period = 1;
    pattr.wakeup_events = 1;
    // cat /sys/kernel/debug/tracing/events/syscalls/sys_enter_bpf/id
    pattr.config = 484;
    pattr.size = sizeof(pattr);
    efd = syscall(SYS_perf_event_open, &pattr, -1, 0, -1, 0);
    if (efd < 0) {
        printf("perf_event_open error: %s\n", strerror(errno));
        exit(-1);
    }
    ret = ioctl(efd, PERF_EVENT_IOC_ENABLE, 0);
    if (ret < 0) {
        printf("PERF_EVENT_IOC_ENABLE error: %s\n", strerror(errno));
        exit(-1);
    }
    ret = ioctl(efd, PERF_EVENT_IOC_SET_BPF, pfd);
    if (ret < 0) {
        printf("PERF_EVENT_IOC_SET_BPF error: %s\n", strerror(errno));
        exit(-1);
    }
    while (1);
}
