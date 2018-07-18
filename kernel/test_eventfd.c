// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2018 ANSSI. All Rights Reserved.
/* Test to check that the clip-specific read-only, immutable anonymous fs inode
 * is still functional for use with eventfd / signalfd.
 *
 * Usage:  ./test will perform one read, one write, and one read on an eventfd fd.
 *         It should display 'read 0x8' then 'read 0xdeadbeef'.
 *
 * NB : the eventfd is supposed to still be writable, that is a *feature* not a
 *      bug. Those writes are private to the fd, and, unlike e.g. chmods, do not
 *      affect the inode, and thus are not visible to other processes.
 *
 * Author: Vincent Strubel <clipos@ssi.gouv.fr>
 */
#include <stdlib.h>
#include <stdio.h>
#include <syscall.h>
#include <stdint.h>
#include <unistd.h>

#define SYS_eventfd 323

static inline int
eventfd(unsigned int initval)
{
	return syscall(SYS_eventfd, initval);
}

int main(void)
{
	uint64_t count;
	int fd = eventfd(8);
	if (fd == -1) {
		perror("eventfd");
		return EXIT_FAILURE;
	}
	if (8 != read(fd, &count, 8)) {
		perror("read");
		return EXIT_FAILURE;
	}
	printf("read 0x%llx\n", count);
	count=0xdeadbeef;
	if (8 != write(fd, &count, 8)) {
		perror("write");
		return EXIT_FAILURE;
	}
	if (8 != read(fd, &count, 8)) {
		perror("read");
		return EXIT_FAILURE;
	}
	printf("read 0x%llx\n", count);

	return EXIT_SUCCESS;
}
