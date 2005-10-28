#ifndef CC
CC=g++
#endif

all: #Connection.o ResultSet.o DriverManager.o

Connection.o: Connection.cpp Connection.h
	$(CC) -c Connection.cpp
	
ResultSet.o: ResultSet.cpp ResultSet.h
	$(CC) -c ResultSet.cpp
	
DriverManager.o: DriverManager.cpp DriverManager.h
	$(CC) -c DriverManager.cpp
		
#lib/libws2_32.a
#$(CPP) -c client.cpp -o client.o $(CXXFLAGS)

clean:
	rm -rf *.o a.out

	
	