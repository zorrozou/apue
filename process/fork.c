#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
	pid_t pid;

	printf("this is parent!");
//	fflush(stdout);

	pid = fork();
	if (pid < 0) {
		perror("fork()");
		exit(1);
	}

	if (pid == 0) {
		printf("this is child!\n");
		exit(0);
	}

	printf("this is parent!\n");
	exit(0);
}
