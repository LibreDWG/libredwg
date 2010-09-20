all:
	swig -verbose -debug-classes -includeall -Wall -python -outdir . -o dwg_wrap_python.c ../dwg.i
	gcc -v -I /usr/include/python2.6 -shared -fPIC dwg_wrap_python.c -lredwg -o _libredwg.so
