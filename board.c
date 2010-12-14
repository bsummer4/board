#include <u.h>
#include <libc.h>
#include <thread.h>
#include <draw.h>
#include <mouse.h>
#include <err.h>

Mousectl *mousectl;
int grid = 50;
int nx=11, ny=8, sep=8;
Point zero={0,0};
Image *bg1, *bg2, *green;
int numtiles=-1;
Image **ts=nil;
Display *d;
Point selected = {0,0};

int layout[8][11] = {
	{ 0,  1,  2,  3,  4,  5,  6,  7, -1,  0,  6},
	{ 8,  9, 10, 11,  0,  1,  2,  3, -1,  1,  7},
	{-1, -1, -1, -1, -1, -1, -1, -1, -1,  2,  8},
	{-1, -1, -1, -1, -1, -1, -1, -1, -1,  3,  9},
	{-1, -1, -1, -1, -1, -1, -1, -1, -1,  4, 10},
	{-1, -1, -1, -1, -1, -1, -1, -1, -1,  5, 11},
	{ 0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1},
	{11, 11, 11, 11, 11, 11, 11, 11, -1, -1, -1}};

void drawsquare (int x, int y) {
	int color = (x+y)%2;
	int sel = selected.x==x && selected.y==y;
#	define XX(i) ((i)*grid)
#	define YY(i) ((i)*grid)
	Point rect[5] = {
		{XX(x), YY(y)},     {XX(x+1), YY(y)},
		{XX(x+1), YY(y+1)}, {XX(x), YY(y+1)},
      {XX(x), YY(y)}};
	fillpoly(screen, rect, 4, 0, color?bg2:bg1, zero);
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
	numtiles = ii;
	ts = malloc(ii*sizeof(Image*));
	if (!ts) err(1, "malloc"); }

void tileset (char *dirname) {
	int dirlen = strlen(dirname);
	char buf[dirlen+4];
	strcpy(buf, dirname);
	tileset_(buf, dirlen, 0); }

void DRAW () {
	for (int ii=0; ii<nx; ii++)
		for (int jj=0; jj<ny; jj++)
			if (ii != sep) drawsquare(ii, jj);
	if (selected.x < nx && selected.y < ny && selected.x != sep)
		drawsquare(selected.x, selected.y);
	flushimage(d,1); }

void threadmain (int argc, char *argv[]) {
	if (-1 == initdraw(nil, nil, "board")) {
		fprint(2, "board: can't open display: %r");
		threadexits("initdraw"); }
	d=display;
	if (argc != 2)  {
		fprint(2, "usage: %s %s\n", *argv, "tileset");
		threadexits("invalid arguments"); }
	tileset (argv[1]);
	bg1 = allocimage(d, Rect(0, 0, 1, 1), RGB24, 1, DDarkgreen);
	bg2 = allocimage(d, Rect(0, 0, 1, 1), RGB24, 1, DBluegreen);
	green = allocimage(d, Rect(0, 0, 1, 1), RGB24, 1, DGreen);
	mousectl = initmouse(nil, screen);
	DRAW();
	readmouse(mousectl);
	for (;;) {
		Mouse m;
		int mem;
		Alt alts[] = {
			{mousectl->c, &m, CHANRCV},
			{mousectl->resizec, &mem, CHANRCV},
			{nil, nil, CHANEND}};

		switch (alt(alts)){
		case 0: {
			int mut=0;
			if (m.buttons&1) {
				Point tmp = selected;
				selected = divpt(m.xy,grid);
				mut=!eqpt(selected, tmp); }
			if (m.buttons&4) {
				Point to = divpt(m.xy, grid);
				if (to.x<sep && to.y<ny &&
				    selected.x < nx && selected.y < ny &&
				    !eqpt(to, selected) &&
				    (selected.x > sep || -1!=layout[selected.y][selected.x])) {
					mut=1;
					int p = layout[selected.y][selected.x];
					if (selected.x < sep) layout[selected.y][selected.x] = -1;
					if (to.x < sep) layout[to.y][to.x] = p; }}
			if (mut) DRAW(); }
			break;
		case 1:
			if (0 > getwindow(display, Refnone))
				err(1, "Couldn't attach to window");
			DRAW(); }}
	threadexits(nil); }
