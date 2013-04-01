#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


int main()
{
	pid_t pid;
	int count;

	setvbuf(stdout, NULL, _IONBF, 0);
	printf("this is parent %d!\n", getpid());
	for (count=0;count<=10;count++) {
		pid = fork();
		if (pid < 0) {
			perror("fork()");
			exit(1);
		}
		if (pid == 0) {
				printf("this is child %d\n", getpid());
				printf("my parent is %d\n", getppid());
				exit(0);
		}
	}

	for (count=0;count<=10;count++) {
		wait(NULL);
	}
	exit(0);
}
