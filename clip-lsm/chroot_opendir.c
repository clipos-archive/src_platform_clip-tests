// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2018 ANSSI. All Rights Reserved.
/* Test for CLSM_CHROOT_OPENDIR checks on chroot.
 * Tries to perform two chroot calls into the 'chroot' directory,
 * with the second call performed after opening the current directory.
 * First call should succeed, and second one should fail.
 * Usage : ./test as root (with CAP_SYS_CHROOT - add it to kernel.clip.rootcap on CLIP)
 * A 'chroot' sub-directory must exist in $CWD.
 *
 * Author: Vincent Strubel <clipos@ssi.gouv.fr>
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>


int do_chroot_child(void)
{
	if (chroot("chroot")) {
		perror("chroot");
		_exit(-1);
	}
	_exit(0);
}

int do_chroot(void)
{
	pid_t pid;
	int status;

	pid = fork();
	switch (pid) {
		case -1:
			perror("fork");
			return -1;
		case 0:
			if (chroot("chroot"))
				_exit(-1);
			_exit(0);
		default:
			if (waitpid(pid, &status, 0) != pid) {
				perror("waitpid");
				return -1;
			}
			if (!WIFEXITED(status)) {
				fputs("child did not exit as expected\n", stderr);
				return -1;
			}
			return WEXITSTATUS(status);
	}
}



int main(void)
{
	int dfd;

	if (do_chroot()) {
		fputs("First chroot failed - check privileges and make sure 'chroot' exists\n", stderr);
		return EXIT_FAILURE;
	}

	dfd = open(".", O_RDONLY);
	if (dfd == -1) {
		perror("open");
		return EXIT_FAILURE;
	}

	if (do_chroot()) {
		puts("Second chroot failed as expected\n");
		close(dfd);
		return EXIT_SUCCESS;
	} else {
		fputs("Second chroot succeeded with an open directory fd!\n", stderr);
		return EXIT_FAILURE;
	}
}
