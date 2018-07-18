// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2018 ANSSI. All Rights Reserved.
/* List available extensions from X11 server.
 * Run as is to check for old (supposedly gone) memory corruption bug in Xorg-7.1
 * Run in a non-trusted domain to check available extensions.
 * Run in VNC server to check available extensions.
 *
 * Author: Vincent Strubel <clipos@ssi.gouv.fr>
 */
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>


int
main(int argc, char *argv[])
{
	int i,n;
	char **extnames;
	Display *dpy;

	if (!(dpy = XOpenDisplay (":0"))) {
		fputs("Can't open display\n", stderr);
		exit(1);
	}

	extnames = XListExtensions(dpy, &n);
	while (--n >= 0) {
		printf("extname: %s\n", extnames[n]);
	}
	XFreeExtensionList(extnames);
	XCloseDisplay(dpy);
	exit (0);
}


