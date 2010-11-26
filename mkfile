all:V: board
test:V: all
	./board

clean:V:
	rm -f board board.o

board: board.c
	gcc -std=gnu99 -lcairo -I/usr/include/cairo/ board.c -o board

board: board.c
	9c -std=gnu99 board.c
	9l -lthread board.o -o $target
