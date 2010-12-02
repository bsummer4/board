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
Image *red, *blue, *green;
int numtiles=-1;
Image **ts=nil;
Display *d;
Point selected = {0,0};

void fuck () { fprint(2, "fuck: %r"); threadexits("fuck"); }

int layout[8][8] = {
	{ 0,  1,  2,  3,  4,  5,  6,  7},
	{ 8,  9, 10, 11,  0,  1,  2,  3},
	{-1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1},
	{ 0,  0,  0,  0,  0,  0,  0,  0},
	{11, 11, 11, 11, 11, 11, 11, 11}};

void drawsquare (int x, int y) {
	int color = (x+y)%2;
	int sel = selected.x==x && selected.y==y;
#	define XX(i) ((i)*X)/nx
#	define YY(i) ((i)*Y)/ny
	Point rect[5] = {
		{XX(x), YY(y)},         {XX(x+1)+1, YY(y)},
		{XX(x+1)+1, YY(y+1)+1}, {XX(x), YY(y+1)+1},
      {XX(x), YY(y)}};
	fillpoly(screen, rect, 4, 0, color?blue:red, zero);
	poly(screen, rect, 5, 0, Enddisc, Enddisc, sel?green:d->black, zero);
	Rectangle r={rect[0], rect[2]};
	int p = layout[y][x];
	if (-1!=p) draw(screen, r, ts[p], ts[p], zero); }

void tileset_ (char *buf, int dirlen, int ii) {
	if (99<ii) goto nomore;
	sprint(buf+dirlen, "/%d", ii);
	int fd = open(buf, OREAD);
	if (0>fd) goto nomore;
	Image *img = readimage(d, fd, 0);
	close(fd);
	tileset_ (buf, dirlen, 1+ii);
	ts[ii] = img;
	return;
nomore:
	if (!ii) errx(1, "No valid images");
	fprint(2, "hi %d\n", ii);
	numtiles = ii;
	ts = malloc(ii*sizeof(Image*));
	if (!ts) err(1, "malloc"); }

void tileset (char *dirname) {
	int dirlen = strlen(dirname);
	char buf[dirlen+4];
	strcpy(buf, dirname);
	tileset_(buf, dirlen, 0); }

void threadmain (int argc, char *argv[]) {
	if (-1 == initdraw(nil, nil, "board")) {
		fprint(2, "board: can't open display: %r");
		threadexits("initdraw"); }
	d=display;
	if (argc != 2)  {
		fprint(2, "usage: %s %s\n", *argv, "tileset");
		threadexits("invalid arguments"); }
	tileset (argv[1]);
	red = allocimage(d, Rect(0, 0, 1, 1), RGB24, 1, DRed);
	blue = allocimage(d, Rect(0, 0, 1, 1), RGB24, 1, DBlue);
	green = allocimage(d, Rect(0, 0, 1, 1), RGB24, 1, DGreen);
	mousectl = initmouse(nil, screen);
	drawsquare(3, 3);
	for (int ii=0; ii<nx; ii++)
		for (int jj=0; jj<ny; jj++)
			drawsquare(ii, jj);
	drawsquare(selected.x, selected.y);
	for (;;) { readmouse(mousectl); }
	threadexits(nil); }
