/* See LICENSE file for copyright and license details. */

#include <poll.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/XInput2.h>

/* macros */
#define arg(A) (!strcmp(argv[i], A))

static void genericevent(XEvent *e);
static void hide_cursor(void);
static void quit(int unused);
static void run(void);
static void setup(void);
static void show_cursor(void);
static void usage(void);

static volatile int running = 1;
static int cursor_hidden = 0;
static int xi_opcode;
static Display *dpy;
static Window root;
static void (*handler[LASTEvent]) (XEvent *) = {
	[GenericEvent] = genericevent,
};

void
genericevent(XEvent *e)
{
	if (e->xcookie.extension != xi_opcode)
		return;

	if (!XGetEventData(dpy, &e->xcookie))
		return;

	switch (e->xcookie.evtype) {
	case XI_RawMotion:
		show_cursor();
		break;
	case XI_RawTouchBegin:
	case XI_RawTouchEnd:
	case XI_RawTouchUpdate:
	case XI_RawKeyPress:
		hide_cursor();
		break;
	}

	XFreeEventData(dpy, &e->xcookie);
}

void
hide_cursor(void)
{
	if (cursor_hidden)
		return;
	XFixesHideCursor(dpy, root);
	cursor_hidden = 1;
}

void
quit(int unused)
{
	running = 0;
}

void
run(void)
{
	XEvent ev;

	/* Tell XInput to send us all RawMotion events. */
	unsigned char mask_bytes[XIMaskLen(XI_LASTEVENT)];
	memset(mask_bytes, 0, sizeof(mask_bytes));
	XISetMask(mask_bytes, XI_RawMotion);
	XISetMask(mask_bytes, XI_RawKeyPress);
	XISetMask(mask_bytes, XI_RawTouchBegin);
	XISetMask(mask_bytes, XI_RawTouchEnd);
	XISetMask(mask_bytes, XI_RawTouchUpdate);

	XIEventMask mask;
	mask.deviceid = XIAllMasterDevices;
	mask.mask_len = sizeof(mask_bytes);
	mask.mask = mask_bytes;
	XISelectEvents(dpy, root, &mask, 1);

	/* main event loop */
	while (running) {
		struct pollfd pfd = {
			.fd = ConnectionNumber(dpy),
			.events = POLLIN,
		};
		int pending = XPending(dpy) > 0 || poll(&pfd, 1, -1) > 0;

		if (!running)
			break;
		if (!pending)
			continue;

		XNextEvent(dpy, &ev);
		if (handler[ev.type])
			handler[ev.type](&ev); /* call handler */
	}
}

void
setup(void)
{
	signal(SIGINT, quit);
	signal(SIGTERM, quit);
	root = RootWindow(dpy, DefaultScreen(dpy));
}

void
show_cursor(void)
{
	if (!cursor_hidden)
		return;
	XFixesShowCursor(dpy, root);
	cursor_hidden = 0;
}

void
usage(void)
{
	fprintf(stdout, "usage: xban [-hvf]\n\n" );
	fprintf(stdout, "xban hides the X mouse cursor on keypress and reveals it again when the ");
	fprintf(stdout, "mouse cursor moves.\n\n");

	char ofmt[] = "   %-10s%s\n";
	fprintf(stdout, "Options:\n");
	fprintf(stdout, ofmt, "-h", "print this help section");
	fprintf(stdout, ofmt, "-v", "print version information and exit");
	fprintf(stdout, ofmt, "-f", "fork the process (i.e run in the background)");

	fprintf(stdout, "\nSee the man page for more details.\n\n");
	exit(0);
}

int
main(int argc, char *argv[])
{
	int junk, i;

	for (i = 1; i < argc; i++) {
		if (arg("-v") || arg("--version")) { /* prints version information */
			puts("xban-"VERSION);
			exit(0);
		} else if (arg("-f") || arg("--fork")) {
			if (fork() != 0)
				exit(0);
		} else if (arg("-h") || arg("--help")) {
			usage();
		} else {
			fprintf(stderr, "Unknown argument: %s\n", argv[i]);
			usage();
		}
	}

	if (!(dpy = XOpenDisplay(NULL))) {
		fprintf(stderr, "xban: cannot open display");
		exit(1);
	}
	if (!XQueryExtension(dpy, "XInputExtension", &xi_opcode, &junk, &junk)) {
		fprintf(stderr, "xban: XInput is not available.");
		exit(1);
	}

	setup();
	run();
	show_cursor();
	XCloseDisplay(dpy);
	return EXIT_SUCCESS;
}
