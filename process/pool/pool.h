#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LOCKFILE "/tmp/lock"
#define START 1010001
#define END 1020000
#define NUM 4

int mylock(void);

int myunlock(void);
