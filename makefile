all: libutest.so A1SampleMain

libutest.so: A1.o
	gcc -shared -o libutest.so A1.o

A1SampleMain: A1.o A1SampleMain.c
	gcc -Wall -std=gnu99 -g A1.o A1SampleMain.c -o A1SampleMain

A1.o: A1Header.h A1.c
	gcc -Wall -std=gnu99 -c -fpic A1.c -o A1.o

clean:
	rm A1.o libutest.so A1SampleMain