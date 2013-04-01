#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>

#define TIMESLICE 100 /* time in ms */
#define BUCKETSIZE 4096

int tbf_init(void);

int tbf_start(void);

int set_rate(void);

int tbf_end(void);


