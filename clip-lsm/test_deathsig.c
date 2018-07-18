// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright Â© 2018 ANSSI. All Rights Reserved.
/* Test for signal delivery to a privileged task through pdeath_signal.
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

static void pere_deathsig(void)
{
	sleep(2);
	exit(0);
}

static void fils_deathsig(void)
{
	prctl(PR_SET_PDEATHSIG, SIGSEGV);
	execl("/bin/su", "/bin/su", "-", NULL);
	exit(1);
}

/* su recoit bien le SIGSEGV ! */
static void test_deathsig(void)
{
	int f;
	f = fork();
	if (f > 0)
		pere_deathsig();
	else if (f == 0)
		fils_deathsig();
}

/* En cas de succes: faire "stty echo" pour restaurer le tty */
int main(void)
{
	test_deathsig();
	return 0;
}
