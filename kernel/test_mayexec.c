// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2018 ANSSI. All Rights Reserved.
/* Test for the O_MAYEXEC open() option specific to clip kernels
 * Usage: test <fname> with <fname> the path of an existing file to which
 * the caller has read permission.
 * The test should be run twice :
 *  - with <fname> on a VFS mount without the noexec option : the test should succeed.
 *  - with <fname> on a VFS mount with the noexec option : the test should print
 *    "Permission denied"
 * Author: Vincent Strubel <clipos@ssi.gouv.fr>
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define USAGE "<test> <filename>"
#ifndef O_MAYEXEC
#define O_MAYEXEC 00000040
#endif

int
main(int argc, char *argv[])
{
	int fd;

	if (argc < 2) {
		fputs(USAGE, stderr);
		return EXIT_FAILURE;
	}

	fd = open(argv[1], O_RDONLY|O_MAYEXEC);
	if (fd != -1) {
		printf("%s opened OK in MAYEXEC mode\n", argv[1]);
		close(fd);
		/* Actually, success or failure depending on test settings */
		return EXIT_SUCCESS;
	} else {
		if (errno == EPERM || errno == EACCES) {
			printf("Permission denied when opening %s in MAYEXEC "
					"mode\n", argv[1]);
			return EXIT_SUCCESS;
		} else {
			printf("Unexpected error opening %s - %s\n", argv[1],
					strerror(errno));
			return EXIT_FAILURE;
		}
	}
}

