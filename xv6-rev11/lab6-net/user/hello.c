// hello, world
#include <inc/lib.h>

void
sleep(int sec) {
    unsigned now = sys_time_msec();
    unsigned end = now + sec * 1000;

    if ((int) now < 0 && (int) now > -MAXERROR)
        panic("sys_time_msec: %e", (int) now);
    if (end < now)
        panic("sleep: wrap");

    while (sys_time_msec() < end)
        sys_yield();
}

void
umain(int argc, char **argv) {
    cprintf("hello, world\n");
    char *content = "abcd";
    for (int i = 0; i < 50; i++) {
        sleep(1);
        cprintf("sys_pkt_send: %d\n", sys_pkt_send(content, 5));
    }
}
