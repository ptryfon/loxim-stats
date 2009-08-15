CXX=g++
CXXFLAGS=-Wall -g -O0 -c -g -O0 -I/usr/local/include
LD=g++
LDFLAGS=-L/usr/local/lib

all: lw_protogen

lw_protogen: lw_protogen.o
	${LD} ${LDFLAGS} -o lw_protogen lw_protogen.o -lxerces-c

lw_protogen.o: lw_protogen.cpp
	${CXX} ${CXXFLAGS} -o lw_protogen.o lw_protogen.cpp

clean:
	rm -rf lw_protogen lw_protogen.o

