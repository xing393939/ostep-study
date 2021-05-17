// hello, world
#include <inc/lib.h>

void
umain(int argc, char **argv)
{
	cprintf("hello, world\n");
	char *content = "abcd";
	cprintf("sys_pkt_send: %d\n", sys_pkt_send(content, 5));
}
