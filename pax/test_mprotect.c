// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2018 ANSSI. All Rights Reserved.
/* Test for PAX_MPROTECT mprotect restrictions
 * Usage: ./test should output 'write ok', then exit with an error
 * "mprotect exec: Permission denied".
 * A 'toto' file needs to be present in the current directory, with
 * an initial size of at least 4.
 *
 * Author: Vincent Strubel <clipos@ssi.gouv.fr>
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void)
{
	int fd;
	void *ptr;
	void (*fn)(void);

	fd = open("toto", O_RDONLY);
	if (fd < 0) {
		perror("open");
		return EXIT_FAILURE;
	}

	ptr = mmap(NULL, 4096, PROT_READ, MAP_PRIVATE, fd, 0);
	if (ptr == MAP_FAILED) {
		perror("mmap");
		return EXIT_FAILURE;
	}

	if (mprotect(ptr, 4096, PROT_READ|PROT_WRITE) != 0) {
		perror("mprotect write");
		return EXIT_FAILURE;
	}
	*(int *)ptr = 0;
	puts("write ok");

	if (mprotect(ptr, 4096, PROT_READ) != 0) {
		perror("mprotect read");
		return EXIT_FAILURE;
	}

	if (mprotect(ptr, 4096, PROT_READ|PROT_EXEC) != 0) {
		perror("mprotect exec");
		return EXIT_FAILURE;
	}
	fputs("mprotect exec ok : this is not good!\n", stderr);
	puts("next call should crash the process");
	fn = (void (*)(void))ptr;
	fn();

	fputs("I really shouldn't have been able to reach this point\n", stderr);

	return EXIT_FAILURE;
}

