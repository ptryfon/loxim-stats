
INSTALL?=/usr/local

CFLAGS+=-Wall
CXXFLAGS+=-Wall -I..

all: clean liblx.a test

test: test.c liblx.a
	$(CC) $(CFLAGS) -g -o $@ test.c liblx.a -lstdc++ -lpthread

access.o: access.c

convert.o: convert.cpp

exports.o: exports.cpp

liblx.a: access.o exports.o convert.o
	ar cru $@ access.o exports.o convert.o ../Client/*.o ../Protocol/*.o ../Protocol/*/*.o ../Protocol/Packages/Data/*.o ../Errors/*.o ../Util/*.o
	ranlib $@

clean:
	rm -f liblx.a *.o

install:
	mkdir -p $(INSTALL)/include
	install -m 0644 liblx.h $(INSTALL)/include
	mkdir -p $(INSTALL)/lib
	install -m 0644 liblx.a $(INSTALL)/lib

