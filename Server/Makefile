#ifndef CC
CC=g++
#endif

all: Server

Server: Server.o ConnectionThread.o
	$(CC) Server.o ConnectionThread.o -o Server
	
Server.o: Server.cpp Server.h
	$(CC) -c Server.cpp
	
ConnectionThread.o: ConnectionThread.cpp ConnectionThread.h
	$(CC) -c ConnectionThread.cpp

clean:
	rm -rf *.o a.out

	
	