// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2018 ANSSI. All Rights Reserved.
/* Test for Stack Smashing Protection.
 * Usage: run, check that it gets killed by SSP, check logs if glibc>=2.5.
 * Author: Vincent Strubel <clipos@ssi.gouv.fr>
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int crash(void)
{
	char buf[64];

	memset(buf, 'a', 128);
	puts("Here we go");
	return EXIT_FAILURE;
}

int main (void)
{
	return crash();
}
