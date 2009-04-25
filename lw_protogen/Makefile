CXX=g++ -c -g -O0
CXXFLAGS=-Wall -g -O0
LD=g++

all: lw_protogen

lw_protogen: lw_protogen.o
	${LD} -o lw_protogen lw_protogen.o -lxerces-c

lw_protogen.o: lw_protogen.cpp
	${CXX} ${CFLAGS} -o lw_protogen.o lw_protogen.cpp

clean:
	rm -rf lw_protogen lw_protogen.o

