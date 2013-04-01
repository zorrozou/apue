#include "tbfwrite.h"

static int token = 0;
static int rate = 100;
static struct itimerval it;
static struct sigaction oact;

void sig_timer(int s)
{
	if (token+rate >= BUCKETSIZE) {
		token = BUCKETSIZE;
	} else {
		token += rate;
	}
}

int tbf_start(void)
{
	int ret;
	struct sigaction act;

	act.sa_handler = sig_timer;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART;
	if (sigaction(SIGALRM, &act, &oact) < 0) {
		perror("sigaction()");
		exit(1);
	}

	it.it_value.tv_sec = 0;
	it.it_value.tv_usec = 1000 * TIMESLICE;

	memcpy(&it.it_interval, &it.it_value, sizeof(it.it_value));
	setitimer(ITIMER_REAL, &it, NULL);
	return(ret);
}

int tbf_end(void)
{
	int ret;
	if (sigaction(SIGALRM, &oact, NULL) < 0) {
		perror("sigaction()");
		exit(1);
	}

	
	it.it_value.tv_sec = 0;
	it.it_value.tv_usec = 0;
	memcpy(&it.it_interval, &it.it_value, sizeof(it.it_value));
	setitimer(ITIMER_REAL, &it, NULL);
	return(ret);
}

int tbfwrite(int fd, char * buf, size_t size)
{
	int writesize = 0;
	int current = 0;
	int ret;

	while (1) {
		if (current >= size) {
			break;
		}
		while (!token) {
			sleep(1);
		}
		if (size >= token) {
			writesize = token;
		} else {
			writesize = size;
		}
		ret = write(fd, &(buf[current]), writesize);
		if (ret < 0) {
			perror("write()");
			return(ret);
		}
		current += ret;
		token -= ret;
	}
	return(current);
}
