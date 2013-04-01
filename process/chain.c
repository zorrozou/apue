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
		if (pid > 0) {
			wait(NULL);
			break;
		}

		if (pid == 0) {
			printf("my parent is %d\n", getppid());
			printf("this is child %d\n", getpid());
		}
	}
	exit(0);
}
