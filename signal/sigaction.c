#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>

void sig_func(int s)
{
	time_t t;
	sigset_t set;

	if (s == SIGQUIT) {
		printf("catch a SIGQUIT signal!\n");
		t = time(NULL);
		while (time(NULL) < t+5);
		sigemptyset(&set);
		if (sigpending(&set) == -1) {
			perror("sigpending()");
			exit(1);
		}
		if (sigismember(&set, SIGINT)) {
			printf("A int signal is pending!\n");
		}
	}
	printf("exit sig_func()!\n");
}

int main(void)
{
	struct sigaction act, oact;

	act.sa_handler = sig_func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	if (sigaction(SIGQUIT, &act, &oact) == -1) {
		perror("sigaction()");
		exit(1);
	}

	while (1) {
		pause();
	}
	exit(0);
}
