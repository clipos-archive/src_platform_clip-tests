// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2018 ANSSI. All Rights Reserved.
/* Smallish test case for grsec's GRKERNSEC_BRUTE.
 * Run as <test> 3 : check that 1st child exit comes immediately,
 * and next two come after a 30s delay.
 * Author: Vincent Strubel <clipos@ssi.gouv.fr>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

void
do_child(void)
{
	if (kill(getpid(), SIGILL)) {
		fprintf(stderr, "Child %d - kill error %s\n", getpid(),
					strerror(errno));
		_exit(1);
	}
}

int
do_father(pid_t pid)
{
	int status;
	if (waitpid(pid, &status, 0) != pid) {
		fprintf(stderr, "Child %d - wait error %s\n", strerror(errno));
		return -1;
	}
	if (WIFEXITED(status)) {
		fprintf(stderr, "Child %d - exited normally ?\n");
		return -1;
	}

	printf("Child %d terminated\n", pid);

	return 0;
}

int
main(int argc, char *argv[])
{
	pid_t pid;
	int numfork, stop=0;

	if (argc < 2) {
		fputs("Usage : <test> <numfork>\n", stderr);
		return EXIT_FAILURE;
	}

	numfork = atoi(argv[1]);
	if (!numfork) {
		fputs("Usage : <test> <numfork>\n", stderr);
		return EXIT_FAILURE;
	}

	do {
		switch (pid = fork()) {
			case -1:
				perror("fork failure");
				return EXIT_FAILURE;
			case 0:
				do_child();
				fputs("What the heck am I doing here?\n", stderr);
				_exit(EXIT_FAILURE);
			default:
				stop = do_father(pid);
		}
	} while (!stop && numfork--);

	return EXIT_SUCCESS;
}
