/*
 * Watchdog Driver Test Program
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/watchdog.h>

int fd;
int magic_close;

static void keep_alive(void)
{
	int dummy;

	ioctl(fd, WDIOC_KEEPALIVE, &dummy);
}


static void term(int sig)
{
	if (magic_close)
		write(fd, "V", 1);

	close(fd);

	fprintf(stderr, "Stopping watchdog ticks...\n");
	exit(0);
}

int main(int argc, char *argv[])
{
	int flags;
	int timeout;
	struct watchdog_info info;

	fd = open("/dev/watchdog", O_WRONLY);

	if (fd == -1) {
		perror("Watchdog device not enabled.\n");
		exit(-1);
	}

	if (ioctl(fd, WDIOC_GETTIMEOUT, &timeout)) {
		perror("Cannot read watchdog interval\n");
		exit(-1);
	} 

	fprintf(stdout, "Current watchdog interval is %d\n", timeout);

	if (ioctl(fd, WDIOC_GETSUPPORT, &info)) {
		    perror("ioctl failed");
		    exit(-1);
	}

	if (WDIOF_MAGICCLOSE & info.options) {
		printf("Watchdog supports magic close char\n");
		magic_close = 1;
	}

	signal(SIGINT, term);

	while(1) {
		keep_alive();
		sleep(5);
	}

	/* never reach here */
	close(fd);
	return 0;
}
