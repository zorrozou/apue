#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define START 20100001
#define END 20200000
#define NUM 6

typedef struct item {
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	int count;
} Myitem;

static Myitem item = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0};

void *process_primer(void *arg)
{
	int tmp;
	int ret;
	int i;

	printf("I'm thread %d\n", pthread_self());
	while (1) {
		ret = pthread_mutex_lock(&item.mutex);
		if (ret != 0) {
			perror("pthread_mutex_lock()");
			exit(1);
		}

		if (item.count == 0) {
			pthread_cond_wait(&item.cond, &item.mutex);
		}
		tmp = item.count;
		item.count = 0;
		ret = pthread_mutex_unlock(&item.mutex);
		if (ret != 0) {
			perror("pthread_mutex_unlock()");
			exit(1);
		}
		for (i=2;i<tmp/2;i++) {
			if (tmp%i == 0) {
				ret = 0;
				break;
			} else {
				ret = 1;
				continue;
			}
		}
		if (ret == 1) {
			printf("%d\n", tmp);
		}
	}
	pthread_exit((void *)1);
}

void create_num(unsigned int num)
{
	int ret;

	while (1) {
//			printf("in create_num %d\n", item.count);
			ret = pthread_mutex_lock(&item.mutex);
			if (ret != 0) {
					perror("pthread_mutex_lock()");
					exit(1);
			}

			if (item.count != 0) {
					ret = pthread_mutex_unlock(&item.mutex);
					if (ret != 0) {
							perror("pthread_mutex_unlock()");
							exit(1);
					}
//			printf("in create_num %d\n", item.count);
					continue;
			}

			item.count = num;	
			ret = pthread_mutex_unlock(&item.mutex);
			if (ret != 0) {
					perror("pthread_mutex_unlock()");
					exit(1);
			}
			ret = pthread_cond_signal(&item.cond);
			if (ret != 0) {
					perror("pthread_cond_signal()");
					exit(1);
			}
//			printf("in create_num %d\n", item.count);
			break;
	}
	return;
}

int main(void)
{
	int count, threadn;
	int ret;
	pthread_t tid[NUM];

	for (threadn=0;threadn<NUM;threadn++) {
		ret = pthread_create(&tid[threadn], NULL, process_primer, NULL);
		if (ret != 0) {
			perror("pthread_create()");
			exit(1);
		}
	}
//	sleep(1);
	for (count=START;count<END;count++) {
			create_num(count);
//			printf("in main %d\n", item.count);
	}
	for (threadn=0;threadn<NUM;threadn++) {
			pthread_join(tid[threadn], NULL);
	}
}
