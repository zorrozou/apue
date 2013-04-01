#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>

int main()
{
	key_t key;

	key = ftok("/etc/hosts", 1);

	printf("%d\n", key);
}
