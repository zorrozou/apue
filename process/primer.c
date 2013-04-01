#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void child_handler(int s)
{
	while (waitpid(-1, NULL, WNOHANG)) {
		V();
	}
}

int main()
{
}
