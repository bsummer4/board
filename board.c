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

int layout[8][8] = {
	{ 0,  1,  2,  3,  4,  5,  6,  7},
	{ 8,  9, 10, 11,  0,  1,  2,  3},
	{-1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1},
	{ 0,  0,  0,  0,  0,  0,  0,  0},
	{11, 11, 11, 11, 11, 11, 11, 11}};

void grid () {
	double dx=1/nx, dy=1/ny;
	int color=0;
	for (double ii=0; ii<1-(dx/2); ii+=dx,++color)
		for (double jj=0; jj<1-(dy/2); jj+=dy,++color) {
			color%=2;
			Point rect[4] = {{ii*X,jj*Y}, {(ii+dx)*X,jj*Y},
			                 {(ii+dx)*X, (jj+dy)*Y}, {ii*X,(jj+dy)*Y}};
			fillpoly(screen, rect, 4, 0, color?blue:red, zero); }
	for (int ii=0; ii<nx; ii++)
		for (int jj=0; jj<ny; jj++) {
			int x=(X*ii)/nx, y=(Y*jj)/ny;
			Rectangle r={(Point){2+x,2+y}, (Point){100+x,100+y}};
			int piece = layout[jj][ii];
			if (-1!=piece) draw(screen, r, ts[piece], ts[piece], zero); }
	for (double ii=0; ii<1+(dx/2); ii+=dx) {
		Point p1={ii*X,0}, p2={ii*X, Y};
		line(screen, p1, p2, Enddisc, Enddisc, 1, display->black, zero); }
	for (double ii=0; ii<1+(dy/2); ii+=dy) {
		Point p1={0,ii*Y}, p2={X, ii*Y};
		line(screen, p1, p2, Enddisc, Enddisc, 1, display->black, zero); }}

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
	mousectl = initmouse(nil, screen);
	for (;;) getwindow(display, Refnone),grid(),readmouse(mousectl);
	threadexits(nil); }
