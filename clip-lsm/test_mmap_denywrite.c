// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2018 ANSSI. All Rights Reserved.
/* Test for clip-lsm write protection on files that are mapped executable.
 * Usage: 	test <fname> 1 will open <fname> for writing, and perform several writes.
 * 		test <fname> 2 will map <fname> executable, then unmap it.
 * Run both simultaneously to verify:
 *  - that writing to <fname> is not allowed while it is mapped, and allowed before and after.
 *  - that mapping <fname> is not allowed while it is opened for writing.
 * Run 2 alone to check that only executable files can be mapped executable.
 *
 * Author: Vincent Strubel <clipos@ssi.gouv.fr>
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>


int do1(const char *fname)
{
	int fd1;
	int ret;

	fd1 = open(fname, O_RDWR|O_CREAT|O_TRUNC);
	if (fd1 == -1) {
		perror("open 1");
		return EXIT_FAILURE;
	}
	puts("open1 OK");
	puts("press to write");
	(void)getchar();

	ret = write(fd1, "AA", 2);
	if (ret > 0)
		puts("write 1 OK");

	puts("press to write");
	(void)getchar();
	ret = write(fd1, "BB", 2);
	if (ret > 0)
		puts("write 2 OK");
	else
		perror("write 2");

	puts("press to write");
	(void)getchar();

	ret = write(fd1, "CC", 2);
	if (ret > 0)
		puts("write 3 OK");
	else
		perror("write 3");

	close(fd1);
	return 0;
}

int do2(const char *fname)
{
	int fd2;
	void *map;
	int ret;

	fd2 = open(fname, O_RDONLY);
	if (fd2 == -1) {
		perror("open 2");
		return EXIT_FAILURE;
	}
	puts("open2 OK");
	puts("press to map");
	(void)getchar();

	map = mmap(0, 1000, PROT_READ|PROT_EXEC, MAP_PRIVATE, fd2, 0);
	if (map == MAP_FAILED)
		perror("map");

	puts("press to unmap");
	(void)getchar();

	ret = munmap(map, 1000);
	if (ret)
		perror("munmap");

	close(fd2);
	return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
	if (argc < 3) {
		fputs("Usage: <test> <filename> 1/2 \n", stderr);
		return EXIT_FAILURE;
	}
	if (*argv[2] == '1')
		return do1(argv[1]);
	else if (*argv[2] == '2')
		return do2(argv[1]);
	else {
		puts("bad argv[2]");
		return EXIT_FAILURE;
	}
}
