#include <pthread.h>

typedef struct stage_tag {
	pthread_mutex_t mutex;
	pthread_cond_t avail;
	pthread_cond_t ready;
	int data_ready;
	long data;
	pthread_t thread;
	struct stage_tag *next;
} stage_t;

typedef struct pipe_tag {
	pthread_mutex_t mutex;
	stage_t *head;
	stage_t *tail;
	int stages;
	int active;
} pipe_t;

int pipe_send(stage_t *stage, long data)
{
	int status;

	status = pthread_mutex_lock(&stage->mutex);
	if (status != 0) {
		return status;
	}

	while (stage->data_ready) {
		status = pthread_cond_wait(&stage->ready, &stage->mutex);
		if (status != 0) {
			pthread_mutex_unlock(&stage->mutex);
			return status;
		}
	}

	stage->data = data;
	stage->data_ready = 1;
	status = pthread_cond_signal(&stage->avail);
	if (status != 0) {
		pthread_mutex_unlock(&stage->mutex);
		return status;
	}

	status = pthread_mutex_unolock(&stage->mutex);
	return status;
}


void *pipe_stage(void *arg)
{
	stage_t *stage = (stage_t *)arg;
	stage_t *next_stage = stage->next;
	int status;

	status = pthread_mutex_lock(&stage->mutex);
	if (status != 0) {
		perror("pthread_mutex_lock()");
		exit(1);
	}

	while (1) {
		while (stage->data->ready != 1) {
			status = pthread_cond_wait(&stage->avail, &stage->mutex);
			if (status != 0) {
				perror("pthread_cond_wait()");
				exit(1);
			}
		}
		pipe_send(next_stage, stage->data+1);
		stage->data_ready = 0;
		status = pthread_cond_signal(&stage->ready);
		if (status != 0) {
			perror("pthread_cond_signal()");
			exit(1);
		}
	}
}
