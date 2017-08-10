#include <sys/ioctl.h>

#include <err.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <machine/apmvar.h>
#include <X11/Xlib.h>

static void
pop(Display *d)
{
	const char *m = "Battery is low.", *n = "batp";
	int s = DefaultScreen(d), ww = 128, wh = 64;
    Window w = XCreateSimpleWindow(d, RootWindow(d, s), DisplayWidth(d, s) / 2,
        DisplayHeight(d, s) / 2, ww, wh, 1, BlackPixel(d, s), 0x202020L);
    Atom ad = XInternAtom(d, "WM_DELETE_WINDOW", False),
         an = XInternAtom(d, "_NET_WM_NAME", False),
         ai = XInternAtom(d, "_NET_WM_ICON_NAME", False),
         au = XInternAtom(d, "UTF8_STRING", False);
    XGCValues gcv;
    GC gc = XCreateGC(d, w, 0, &gcv);
    XFontStruct *f = XLoadQueryFont(d, "-*-terminus-medium-*");
    XEvent e;

    XChangeProperty(d, w, an, au, 8, 0, (unsigned char *)n, strlen(n));
    XChangeProperty(d, w, ai, au, 8, 0, (unsigned char *)n, strlen(n));
    XSetTransientForHint(d, w, RootWindow(d, s));
    XSetWMProtocols(d , w, &ad, 1);
    XSelectInput(d, w, ExposureMask);
    if (f)
        XSetFont(d, gc, f->fid);
    XSetForeground(d, gc, 0xc0c0c0L);
    XMapWindow(d, w);
    for (;;) {
        XNextEvent(d, &e);
        if (e.type == Expose)
            XDrawString(d, w, gc, (ww - XTextWidth(f, m, strlen(m))) / 2,
                (wh + f->ascent + f->descent) / 2, m, strlen(m));
        else if (e.type == ClientMessage)
            break;
    }
    if (f)
        XUnloadFont(d, f->fid);
    XFreeGC(d, gc);
    XDestroyWindow(d, w);
}

static void
bat(int fd, Display *d)
{
	static int w = 0;
	struct apm_power_info api;

	if (ioctl(fd, APM_IOC_GETPOWER, &api) == -1)
		warn("ioctl failed");
	if (w == 0 && api.ac_state != APM_AC_ON && api.minutes_left <= 10)
		pop(d), w = 1;
	if (api.ac_state == APM_AC_ON)
		w = 0;
}

int 
main()
{
	Display *d;
	int a = -1;

	if ((d = XOpenDisplay(NULL)) == NULL)
		err(1, "XOpenDisplay failed");
	if ((a = open("/dev/apm", O_RDONLY)) == -1)
		err(1, "open failed");

	for (;;) {
		bat(a, d);
		sleep(60);
	}

	close(a);
	XCloseDisplay(d);
}

