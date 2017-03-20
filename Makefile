all:	check

default: check
	
clean-files:
	rm -rfv mymalloc.so
	rm -rfv myfree.so
	rm -rfv myrealloc.so
	rm -rfv mycalloc.so

clean: clean-files
	rm -rf *.so
	rm -rf *.o

malloc.o: mymalloc.c
	gcc -fno-stack-protector -c -fpic -o malloc.o mymalloc.c

free.o: myfree.c
	gcc -fno-stack-protector -c -fpic -o free.o myfree.c	

calloc.o: mycalloc.c
	gcc -fno-stack-protector -c -fpic -o calloc.o mycalloc.c

realloc.o: myrealloc.c
	gcc -fno-stack-protector -c -fpic -o realloc.o myrealloc.c

libmalloc.so: malloc.o
	gcc -fno-stack-protector -shared -o libmalloc.so malloc.o

libfree.so: free.o
	gcc -fno-stack-protector -shared -o libfree.so free.o

libcalloc.so: calloc.o
	gcc -fno-stack-protector -shared -o libcalloc.so calloc.o

librealloc.so: realloc.o
	gcc -fno-stack-protector -shared -o librealloc.so realloc.o

test.o: test.c
	gcc -c -fno-stack-protector -g -fpic -o test.o test.c

test:	test.o
	gcc -g -fno-stack-protector -g -o test test.o

check:	clean-files libmalloc.so libfree.so libcalloc.so librealloc.so test t-test1
	    LD_PRELOAD="`pwd`/libmalloc.so `pwd`/libfree.so `pwd`/libcalloc.so `pwd`/librealloc.so" ./test

dist:
	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar