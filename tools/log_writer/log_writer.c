#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#define LOG_FILE "/var/log/test.log"
#define DIRTY_FILE "/dirtyfile"

void get_formatted_time(struct timeval *tv, char *formatted_time, size_t size) {
    time_t rawtime = tv->tv_sec;
    struct tm *timeinfo;
    timeinfo = localtime(&rawtime);
    strftime(formatted_time, size, "%Y-%m-%d %H:%M:%S", timeinfo);
}

void *dirty_file_thread(void *arg) {
    while (1) {
        system("echo 3 > /proc/sys/vm/drop_caches");
        int dirty_fd = open(DIRTY_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (dirty_fd < 0) {
            perror("Failed to open dirty file");
            return NULL;
        }

        char buffer[1024 * 1024];
        memset(buffer, 0, sizeof(buffer));

        for (int i = 0; i < 2048; i++) {
            if (write(dirty_fd, buffer, sizeof(buffer)) < 0) {
                perror("Failed to write to dirty file");
                close(dirty_fd);
                return NULL;
            }
        }

        close(dirty_fd);
        sleep(40);

        if (remove(DIRTY_FILE) < 0) {
            perror("Failed to remove dirty file");
            return NULL;
        }
    }

    return NULL;
}

int main() {
    int log_fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd < 0) {
        perror("Failed to open log file");
        return 1;
    }

    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, dirty_file_thread, NULL) != 0) {
        perror("Failed to create dirty file thread");
        close(log_fd);
        return 1;
    }

    while (1) {
        struct timeval start_tv, end_tv;
        long duration;
        char log_entry[256], time_buf[32];

        gettimeofday(&start_tv, NULL);
        get_formatted_time(&start_tv, time_buf, sizeof(time_buf));

        int millis = (int) (start_tv.tv_usec / 1000);
        int len = snprintf(log_entry, sizeof(log_entry), "%s.%03d ", time_buf, millis);

        if (write(log_fd, log_entry, len) < 0) {
            perror("Failed to write to log file");
            close(log_fd);
            return 1;
        }

        gettimeofday(&end_tv, NULL);
        duration = (end_tv.tv_sec - start_tv.tv_sec) * 1000000 + (end_tv.tv_usec - start_tv.tv_usec);

        len = snprintf(log_entry, sizeof(log_entry), "write syscall duration: %ld us\n", duration);

        if (write(log_fd, log_entry, len) < 0) {
            perror("Failed to write to log file");
            close(log_fd);
            return 1;
        }

//        fsync(log_fd);
        sleep(1);
    }

    close(log_fd);
    pthread_exit(NULL);
    return 0;
}

