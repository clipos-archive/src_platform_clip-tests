// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2018 ANSSI. All Rights Reserved.
/* Test for immutable, read-only signalfd inode on CLIP kernel.
 * Usage :
 *  - ./test r : check reads - should succeed, and print out the inode's mode
 *  - ./test a : check atimes - shoud succed, and print out the inode's atime
 *  - ./test w : check write (fchmod) - should not succeed, and exit with a
 *               permission denied error.
 * Cross checks :
 *  - use 'r' tests to check that 'w' have indeed not modified the mode
 *  - use 'a' tests in combination with read/writes to check that the inode's atime
 *            is not modified.
 *
 * Author: Vincent Strubel <clipos@ssi.gouv.fr>
 */
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syscall.h>

#define SYS_signalfd 321

static int fd;

static inline int
signalfd(int fd, sigset_t *mask, size_t len)
{
	return syscall(SYS_signalfd, fd, mask, len);
}

static int
do_chmods(void)
{
	char buf[8];
	mode_t mode;
	for(;;) {
		if (!fgets(buf, 8, stdin)) {
			fputs("fgets error\n", stderr);
			return EXIT_FAILURE;
		}

		/* yuck */
		mode = (mode_t) strtoul(buf, NULL, 0);

		if (fchmod(fd, mode)) {
			perror("fchmod");
			return EXIT_FAILURE;
		}
	}

	return EXIT_FAILURE;
}

static int
do_stats(void)
{
	struct stat stat_buf;

	for(;;) {
		/* yuck again :) */
		(void)fgetc(stdin);

		if (fstat(fd, &stat_buf)) {
			perror("fstat");
			return EXIT_FAILURE;
		}
		(void)printf("mode %0.4o\n", stat_buf.st_mode);
	}

	return EXIT_FAILURE;
}

static int
do_atimes(void)
{
	struct stat stat_buf;

	for(;;) {
		/* yuck again :) */
		(void)fgetc(stdin);

		if (fstat(fd, &stat_buf)) {
			perror("fstat");
			return EXIT_FAILURE;
		}
		(void)printf("atime %d\n", stat_buf.st_atime);
	}

	return EXIT_FAILURE;
}

#define USAGE "signalfd 'r'/'w'/'a'\n"

int
main(int argc, char *argv[])
{
	sigset_t set;

	if (argc != 2) {
		fputs(USAGE, stderr);
		return EXIT_FAILURE;
	}

	if (sigemptyset(&set)) {
		perror("sigemptyset");
		return EXIT_FAILURE;
	}

	if (sigaddset(&set, SIGUSR1)) {
		perror("sigaddset");
		return EXIT_FAILURE;
	}

	fd = signalfd(-1, &set, 8);
	if (fd == -1) {
		perror("signalfd");
		return EXIT_FAILURE;
	}

	switch (*argv[1]) {
		case 'r':
			return do_stats();
		case 'w':
			return do_chmods();
		case 'a':
			return do_atimes();
		default:
			fputs(USAGE, stderr);
			return EXIT_FAILURE;
	}
}
