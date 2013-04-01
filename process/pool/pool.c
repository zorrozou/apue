#include "pool.h"

#define PRIMER "/tmp/primer"
#define BUF 64

int getnum(char * path)
{
	FILE * fp;
	char buf[BUF];
	char *str;
	int result;

	fp = fopen(path, "r");
	if (fp == NULL) {
		perror("fopen(getnum)");
		exit(1);
	}

	str = fgets(buf, BUF, fp);
	if (str == NULL) {
		perror("fgets(getnum)");
		exit(1);
	}

	result  = atoi(buf);

	fclose(fp);
	return (result);
}

int setnum(char *path, int result)
{
	FILE * fp;
	char buf[BUF];
	char *str;
	
	fp = fopen(path, "r+");
	if (fp == NULL) {
		perror("fopen(getnum)");
		exit(1);
	}

	fprintf(fp, "%d", result);
	fclose(fp);
	return 0;
}

int do_child(void)
{
		int ret;
		int tmp;
		int i;
		while (1) {
				ret = mylock();
				if (ret != 0) {
						fprintf(stderr, "mylock fail!\n");
						exit(1);
				}

				if ((tmp = getnum(PRIMER)) == 0) {
						ret = myunlock();
						if (ret != 0) {
								fprintf(stderr, "myunlock fail!\n");
								exit(1);
						}
						continue;
				}
				if (tmp == END) {
						ret = myunlock();
						if (ret != 0) {
								fprintf(stderr, "myunlock fail!\n");
								exit(1);
						}
						break;
				}
				ret = setnum(PRIMER, 0);
				if (ret != 0) {
					fprintf(stderr, "setnum() error!\n");
					exit(1);
				}
				ret = myunlock();
				if (ret != 0) {
						fprintf(stderr, "myunlock fail!\n");
						exit(1);
				}
				for (i=2;i<tmp/2;i++) {
					if (tmp%i == 0) {
						ret = 0;
						break;
					}
					ret = 1;
					continue;
				}
				if (ret == 1) {
					printf("%d\n", tmp);
				}
		}
	exit(0);
}

int create_num(int num)
{
	int ret;
	int tmp;

	while (1) {
			ret = mylock();
			if (ret != 0) {
					fprintf(stderr, "mylock fail!\n");
					exit(1);
			}
			tmp = getnum(PRIMER);
			if (tmp != 0) {
				ret = myunlock();
				if (ret != 0) {
					fprintf(stderr, "myunlock fail!\n");
					exit(1);
				}
				continue;
			}
			ret = setnum(PRIMER, num);
			if (ret != 0) {
				fprintf(stderr, "setnum() error!\n");
				exit(1);
			}
			
			ret = myunlock();
			if (ret != 0) {
					fprintf(stderr, "myunlock fail!\n");
					exit(1);
			}
	}
	return 0;
}

int main(void)
{
	pid_t pid[NUM];
	int count;

	for (count=0;count<NUM;count++) {
		pid[count] = fork();
		if (pid[count] == 0) {
			do_child();
		}
	}
	for (count=START;count<=END;count++) {
		create_num(count);
	}

	for (count=0;count<NUM;count++) {
		wait(NULL);
	}
	exit(0);
}
