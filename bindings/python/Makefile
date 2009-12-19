all:
	gcc -shared -fPIC -I/usr/include/python2.5 libredwgmodule.c -o libredwg.so -lredwg

install:
	cp libredwg.so /usr/lib/python2.5/site-packages/
