#include <u.h>
#include <libc.h>
#include <thread.h>
#include <draw.h>
#include <mouse.h>
#include <err.h>

Mousectl *mousectl;
const int X=400, Y=400;
double nx=8, ny=8;
Point zero={0,0};
Image *red, *blue;
int numtiles=-1;
Image **ts=nil;

void derror (Display * d, char* s) {
	fprint(2, "board: fuck\n");
	threadexits("display"); }

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
		line(screen, p1, p2, Enddisc, Enddisc, 1, display->black, zero); }
	Rectangle r = {(Point){2,2}, (Point){100,100}};
	draw(screen, r, ts[0], display->opaque, zero); }

void fuck() { fprint(2, "fuck"); exit(1); }

void tileset_ (char *buf, int dirlen, int ii) {
	if (99<ii) goto nomore;
	sprint(buf+dirlen, "/%d", ii);
	warnx("tile: %d=%s", ii, buf);
	int fd = open(buf, OREAD);
	if (0>fd) goto nomore;
	Image *img = readimage(display, fd, 0);
	close(fd);
	tileset_ (buf, dirlen, 1+ii);
	ts[ii] = img;
	return;
nomore:
	warnx("tile %d=%s didn't make it.", ii, buf);
	if (!ii) errx(1, "No valid images");
	numtiles = ii;
	ts = malloc(ii*sizeof(Image*));
	if (!ts) err(1, "malloc"); }

void tileset (char *dirname) {
	int dirlen = strlen(dirname);
	char buf[dirlen+4];
	strcpy(buf, dirname);
	tileset_(buf, dirlen, 0); }

void threadmain (int argc, char *argv[]) {
	if (-1 == initdraw(derror, nil, "board")) {
		fprint(2, "board: can't open display: %r\n");
		threadexits("initdraw"); }
	drawsetdebug(10);
	red = allocimage(display, Rect(0, 0, 1, 1), RGB24, 1, DRed);
	blue = allocimage(display, Rect(0, 0, 1, 1), RGB24, 1, DBlue);

	if (argc != 2)  {
		fprint(2, "usage: %s %s\n", *argv, "tileset");
		threadexits("invalid arguments"); }
	char *dirname = argv[1];
	tileset (dirname);

	// TODO Do we actually need this?
	if (-1 == getwindow(display, Refnone)) {
		fprint(2, "board: can't get window: %f\n");
		threadexits("getwindow"); }

	mousectl = initmouse(nil, screen); grid();
	for (;;) readmouse(mousectl),grid();
	threadexits(nil); }
