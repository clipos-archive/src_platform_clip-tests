// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright Â© 2018 ANSSI. All Rights Reserved.
/* Test for signal delivery to a privileged task through sigio.
 * Author: Olivier Grumelard <clipos@ssi.gouv.fr>
 */
#define _BSD_SOURCE
#define _GNU_SOURCE
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/prctl.h>

/* NB: exit_signal, défini par les flags de clone (& CSIGNAL) si !CLONE_THREAD,
 * est quant à lui ignoré suite à un exec (sauf si CAP_KILL) */

static void pere_sigio(int fd)
{
	sleep(2);
	write(fd, "a", 1);
	sleep(30);
	exit(0);
}

static void fils_sigio(int fd)
{
	fcntl(fd, F_SETFL, O_ASYNC);
	fcntl(fd, F_SETOWN, getpid());
	fcntl(fd, F_SETSIG, SIGSEGV);
	execl("/bin/su", "/bin/su", "-", NULL);
	exit(1);
}

/* su recoit bien le SIGSEGV ! */
static void test_sigio(void)
{
	int f, fds[2];
	pipe(fds);
	f = fork();
	if (f > 0)
		pere_sigio(fds[1]);
	else if (f == 0)
		fils_sigio(fds[0]);
}

/* En cas de succes: faire "stty echo" pour restaurer le tty */
int main(void)
{
	test_sigio();
	return 0;
}
