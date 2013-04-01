#include "pool.h"

int mylock(void)
{
	FILE *fp;

	while (1) {
		if (access(LOCKFILE, F_OK) != 0) {
			break;
		}
	}

	fp = fopen(LOCKFILE, "w");
	if (fp == NULL) {
		perror("fopen(mylock fail!)");
		return 1;
	}
	fclose(fp);
	return 0;
}

int myunlock(void)
{
	FILE *fp;

	if (access(LOCKFILE, F_OK) != 0) {
		return 1;
	}

	if (remove(LOCKFILE) != 0) {
		return 1;
	}
	return 0;
}
