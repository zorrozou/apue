#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <curl/curl.h>

#define URL "http://10.211.55.4/"
#define REQUESTS_PER_SECOND 1000
#define TOKEN_INTERVAL_US 1000

typedef struct {
	int total_requests;
	int status_count[7]; // 0:200, 1:302, 2:403, 3:404, 4:500, 5:502, 6:others
} RequestStats;

RequestStats stats;
pthread_mutex_t stats_mutex;

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
	return size * nmemb;
}

void update_status_count(long http_code) {
	switch (http_code) {
		case 200:
			stats.status_count[0]++;
			break;
		case 302:
			stats.status_count[1]++;
			break;
		case 403:
			stats.status_count[2]++;
			break;
		case 404:
			stats.status_count[3]++;
			break;
		case 500:
			stats.status_count[4]++;
			break;
		case 502:
			stats.status_count[5]++;
			break;
		default:
			stats.status_count[6]++;
	}
}

void send_http_request() {
	CURL *curl;
	CURLcode res;
	long http_code = 0;

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, URL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

		res = curl_easy_perform(curl);
		if(res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		} else {
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		}

		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();

	pthread_mutex_lock(&stats_mutex);
	stats.total_requests++;
	update_status_count(http_code);
	pthread_mutex_unlock(&stats_mutex);
}

void *request_worker(void *arg) {
	send_http_request();
	return NULL;
}

void token_handler(int sig) {
	pthread_t worker_thread;
	pthread_create(&worker_thread, NULL, request_worker, NULL);
	pthread_detach(worker_thread);
}

void setup_token_timer() {
	struct sigaction sa;
	struct itimerval timer;

	sa.sa_handler = &token_handler;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGALRM, &sa, NULL) == -1) {
		perror("Error: cannot handle SIGALRM");
		exit(1);
	}

	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = TOKEN_INTERVAL_US;
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = TOKEN_INTERVAL_US;
	if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
		perror("Error: cannot set interval timer");
		exit(1);
	}
}

void *stats_thread(void *arg) {
	time_t start_time, end_time;
	while (1) {
		start_time = time(NULL);
		sleep(1);
		end_time = time(NULL);
		if (end_time - start_time >= 1) {
			pthread_mutex_lock(&stats_mutex);
			printf("Total requests: %d\n", stats.total_requests);
			printf("Status 200: %.2f%%\n", (double)stats.status_count[0] / stats.total_requests * 100);
			printf("Status 302: %.2f%%\n", (double)stats.status_count[1] / stats.total_requests * 100);
			printf("Status 403: %.2f%%\n", (double)stats.status_count[2] / stats.total_requests * 100);
			printf("Status 404: %.2f%%\n", (double)stats.status_count[3] / stats.total_requests * 100);
			printf("Status 500: %.2f%%\n", (double)stats.status_count[4] / stats.total_requests * 100);
			printf("Status 502: %.2f%%\n", (double)stats.status_count[5] / stats.total_requests * 100);
			printf("Other status codes: %.2f%%\n", (double)stats.status_count[6] / stats.total_requests * 100);
			printf("\n");

			stats.total_requests = 0;
			for (int i = 0; i < 7; i++) {
				stats.status_count[i] = 0;
			}
			pthread_mutex_unlock(&stats_mutex);
		}
	}
	return NULL;
}

int main() {
	pthread_t stat_thread;
	pthread_mutex_init(&stats_mutex, NULL);
	setup_token_timer();

	pthread_create(&stat_thread, NULL, stats_thread, NULL);

	pthread_join(stat_thread, NULL);

	pthread_mutex_destroy(&stats_mutex);

	return 0;
}
