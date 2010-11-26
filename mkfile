all:V: board ts.chess
test:V: all
	./board ts.chess

clean:V:
	rm -f board board.o tc.chess

ts.%: eps.%
	mkdir -p $target; ./compile eps.$stem/* | (cd $target; pax -r)

board: board.c
	gcc -std=gnu99 -lcairo -I/usr/include/cairo/ board.c -o board

board: board.c
	9c -std=gnu99 board.c
	9l -lthread board.o -o $target
