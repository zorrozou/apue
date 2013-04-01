#include "tbfwrite.h"

#define FILEPATH "/etc/services"


int main()
{
	int fd;
	size_t ret, size;
	char buf[BUCKETSIZE];

	fd = open(FILEPATH, O_RDONLY);
	if (fd < 0) {
		perror("open()");
		exit(1);
	}

	tbf_start();

	while (1) {
		size = read(fd, buf, BUCKETSIZE);
		if (size < 0) {
			perror("read()");
			exit(1);
		}

		if (size == 0) {
			break;
		}

		ret = tbfwrite(1, buf, size);
		if (ret < 0) {
			perror("tbfwrite()");
			exit(1);
		}
	}
	tbf_end();
	close(fd);
	exit(0);
}
