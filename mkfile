all:V: board
test:V: all
	./board
board: board.c
	gcc -std=gnu99 -lcairo -I/usr/include/cairo/ board.c -o board
