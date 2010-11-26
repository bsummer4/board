#include <u.h>
#include <libc.h>
#include <thread.h>
#include <draw.h>
#include <mouse.h>

Mousectl *mousectl;

void derror (Display * d, char* s) {
	fprint(2, "board: fuck\n");
	threadexits("display"); }

const int X=400, Y=400;
double nx=8, ny=8;
Point zero={0,0};
Image *red, *blue;
void grid () {
	double dx=1/nx, dy=1/ny;
	int color=0;
	for (double ii=0; ii<1-(dx/2); ii+=dx,++color)
		for (double jj=0; jj<1-(dy/2); jj+=dy,++color) {
			color%=2;
			Point rect[4] = {{ii*X,jj*Y}, {(ii+dx)*X,jj*Y},
			                 {(ii+dx)*X, (jj+dy)*Y}, {ii*X,(jj+dy)*Y}};
			fillpoly(screen, rect, 4, 0, color?blue:red, zero); }
	for (double ii=0; ii<1+(dx/2); ii+=dx) {
		Point p1={ii*X,0}, p2={ii*X, Y};
		line(screen, p1, p2, Enddisc, Enddisc, 1, display->black, zero); }
	for (double ii=0; ii<1+(dy/2); ii+=dy) {
		Point p1={0,ii*Y}, p2={X, ii*Y};
		line(screen, p1, p2, Enddisc, Enddisc, 1, display->black, zero); }}


void threadmain (int argc, char *argv[]) {
	if (-1 == initdraw(derror, nil, "board")) {
		fprint(2, "board: can't open display: %r\n");
		threadexits("initdraw"); }
	drawsetdebug(1);
	red = allocimage(display, Rect(0, 0, 1, 1), RGB24, 1, DRed);
	blue = allocimage(display, Rect(0, 0, 1, 1), RGB24, 1, DBlue);
	if (-1 == getwindow(display, Refnone)) {
		fprint(2, "board: can't get window: %f\n");
		threadexits("getwindow"); }
	mousectl = initmouse(nil, screen);
	grid();
	for (;;) readmouse(mousectl),grid();
	threadexits(nil); }
