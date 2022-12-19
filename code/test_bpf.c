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

int main() {
    char name[16] = "";
    strcpy(name, "aaaa");
    printf("%s\n", name);
}