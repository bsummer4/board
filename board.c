#include <cairo.h>
#include <cairo-svg.h>
#include <cairo-xlib.h>
#include <X11/Xlib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#define SIZEX 500
#define SIZEY 500
#define C(x) cairo_##x
double x=8, y=8;

C(surface_t) *svg (char *fn) { return C(svg_surface_create)(fn, 72, 72); }

void grid (C(t) *c) {
	double X=SIZEX, Y=SIZEY;
	double dy=1/y, dx=1/x;
	int hi = 0;
	for (double ii=0.0; ii<=1-(dx/2); ii+=dx,hi++) {
		for (double jj=0.0; jj<=1-(dy/2); jj+=dy) {
			printf("%lf:%lf\n", ii, jj);
			cairo_rectangle (c, X*ii, Y*jj, X*(ii+dx), Y*(jj+dy));
			if (++hi%2) cairo_set_source_rgba (c, ii, jj, 0, 0.5);
			else cairo_set_source_rgba (c, 0, 1-ii, 1-jj, 0.5);
			cairo_fill (c); }}
	C(set_source_rgb)(c, 0.0, 0.0, 0.0);
	C(set_line_width)(c, 2.0);
	for (double ii=0.0; ii<=1+(dx/2); ii+=dx) {
		C(move_to)(c, ii*X, 0);
		C(line_to)(c, ii*X, Y);
		C(stroke)(c); }
	for (double ii=0.0; ii<=1+(dy/2); ii+=dy) {
		C(move_to)(c, 0, ii*Y);
		C(line_to)(c, X, ii*Y);
		C(stroke)(c); }}

void bg (C(t) *c) {
	C(set_source_rgb)(c, 0.3, 0.3, 0.3);
	C(rectangle)(c, 0.0, 0.0, SIZEX*2, SIZEY*2);
	C(fill)(c); }

void line (C(t) *c) {
	C(move_to)(c, 0, 0);
	C(set_line_width)(c, 8.0);
	C(scale) (c, 0.5, 1);
	C(set_source_rgb)(c, 1.0, 0.0, 0.0);
	C(arc) (c, 0.5, 0.5, 0.40, 0, 2 * M_PI);
	C(stroke) (c);
	C(set_source_rgb)(c, 0.0, 1.0, 0.0);
	// C(translate) (c, 20, 20);
	C(move_to) (c, 0, 0);
	C(line_to) (c, 10, 10);
	C(arc) (c, 0.5, 0.5, 0.40, 0, 2 * M_PI);
	C(stroke) (c); }

void showxlib () {
	Display *dpy;
	Window rootwin;
	XEvent e;
	cairo_surface_t *cs;
	if (!(dpy=XOpenDisplay(NULL))) errx(1, "Could not open display");
	int scr=DefaultScreen(dpy);
	rootwin = RootWindow(dpy, scr);
	Window win=XCreateSimpleWindow(dpy, rootwin, 1, 1, SIZEX, SIZEY, 0,
	                               BlackPixel(dpy, scr), BlackPixel(dpy, scr));
	XStoreName(dpy, win, "hello");
	XSelectInput(dpy, win, ExposureMask|ButtonPressMask);
	XMapWindow(dpy, win);
	cs=cairo_xlib_surface_create(dpy, win, DefaultVisual(dpy, 0), SIZEX, SIZEY);
	for (;;) {
		XNextEvent(dpy, &e);
		if (e.type==Expose && e.xexpose.count<1) {
			C(t) *c=C(create)(cs);
			bg(c);
			grid(c);
			C(show_page)(c);
			C(destroy)(c); }
		else if (e.type==ButtonPress) break; }
	cairo_surface_destroy(cs);
	XCloseDisplay(dpy); }

int main (int argc, char *argv[]) {
	showxlib(); return 0; }
