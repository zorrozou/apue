#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define FILENAME "/etc/services"
#define BUCKETSIZE 4096
#define BUFSIZE 4096

static int token;

void
sig_alarm(int s)
{
	if (token+BUFSIZE > BUCKETSIZE) {
		token = BUCKETSIZE;
	} else {
		token += BUFSIZE;
	}
}

int
main(void)
{
	int fd, ret;
	struct  sigaction act;
	struct itimerval it, oit;
	char buf[BUCKETSIZE];

	fd = open(FILENAME, O_RDONLY);
	if (fd < 0) {
		perror("open()");
		exit(1);
	}

	act.sa_handler = sig_alarm;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (sigaction(SIGALRM, &act, NULL) < 0) {
		perror("sigaction()");
		exit(1);
	}

	it.it_value.tv_sec =0;
	it.it_value.tv_usec = 100000;
	memcpy(&it.it_interval, &it.it_value, sizeof(it.it_value));

	setitimer(ITIMER_REAL, &it, &oit);

	while (1) {
		while (!token) {
			sleep(1);
		}
		ret = read(fd, buf, token);
		if (ret == 0) {
			break;
		}
		token -= ret;
		write(1, buf, ret);
	}

	setitimer(ITIMER_REAL, &oit, NULL);
	close(fd);
	exit(0);
}
