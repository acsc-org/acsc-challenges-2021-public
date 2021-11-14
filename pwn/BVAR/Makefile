all:
	gcc distfiles/bvar.c -o bvar -g -Wl,-z,norelro
	strip bvar
	cp bvar challenge/bvar
	mv bvar distfiles/bvar
