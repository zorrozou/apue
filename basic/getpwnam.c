#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <pwd.h>

int main()
{
	struct passwd * pw;

	pw = getpwnam("zorro");
	if (pw == NULL) {
		perror("getpwnam()");
		exit(1);
	}

	printf("%s\n", pw->pw_passwd);
	exit(0);
}
