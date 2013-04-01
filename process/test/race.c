#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define FILEPATH "/tmp/count"
#define COUNT 500
#define BUF 64

static int semid;

int initlock()
{
	int ret;

	semid = semget(IPC_PRIVATE, 1, 0600);
	if (semid == -1) {
		perror("semget()");
		exit(1);
	}

	ret = semctl(semid, 0, SETVAL, 1);
	if (ret == -1) {
		perror("semctl()");
		exit(1);
	}

	return ret;
}

int lock()
{
	struct sembuf op;
	int ret;

	op.sem_num = 0;
	op.sem_op = -1;
	op.sem_flg = 0;

	ret = semop(semid, &op, 1);
	if (ret == -1) {
		perror("semop()");
		exit(1);
	}
	return ret;
}

int unlock()
{
    struct sembuf op;
    int ret;
    
    op.sem_num = 0;
    op.sem_op = 1;
    op.sem_flg = 0;

    ret = semop(semid, &op, 1);
    if (ret == -1) {
        perror("semop()");
        exit(1);
    }
    return ret;

}

int do_child(void)
{
	FILE *fp;
	char buf[BUF];
	size_t size;
	int ret;

	fp = fopen(FILEPATH, "r+");
	if (fp == NULL) {
		perror("fopen(r+)");
		exit(1);
	}

	fgets(buf, BUF, fp);
//	buf[size] = '\0';
	ret = atoi(buf);
	ret++;
	fseek(fp, 0, SEEK_SET);
	fprintf(fp, "%d", ret);
	fclose(fp);

	return 0;
}

int main(void)
{
	pid_t pid;
	int count;

	initlock();

	for (count=0;count<COUNT;count++) {
		pid = fork();
		if (pid < 0) {
			perror("fork()");
			exit(1);
		}
		if (pid == 0) {
			lock();
			do_child();
			unlock();
			exit(0);
		}
	}

	for (count=0;count<COUNT;count++) {
		wait(NULL);
	}
	exit(0);
}
