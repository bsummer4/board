gsflags = -dSAFER -dBATCH -dNOPAUSE
MKSHELL = /opt/plan9/bin/rc

all:V: board ts.chess
test:V: all
	./board ts.chess

clean:V:
	rm -rf board board.o ts.chess

board: board.c
	9c '-std='gnu99 board.c
	9l -lthread board.o -o $target

%.png: %.eps
	oflags = ('-sDEVICE=pngalpha' -dEPSCrop -r100x100)
	echo gs $gsflags $oflags '-sOutputFile='$stem.png $stem.eps 
	gs $gsflags $oflags '-sOutputFile='$stem.png $stem.eps 

%.gif: %.png
	convert $stem.png $stem.gif

ts.%: eps.%
	rm -rf tmp
	cp -r eps.$stem tmp
	mk `{ls tmp/*.eps | sed 's/eps$/gif/'}
	cd tmp
	i=0
	for (x in `{ls *.gif}) {
		gif -9 $x >$i
		i=`{expr $i + 1}}
	rm *.*
	cd ..
	mv tmp $target
