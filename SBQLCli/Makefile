#ifndef CC
CC=g++
#endif

#SBQLCli: SBQLCli.o ResultSet.o DriverManager.o Connection.o
#	$(CC) SBQLCli.o ResultSet.o DriverManager.o Connection.o -o SBQLCli

#SBQLCli.o: SBQLCli.cpp
#	$(CC) -c SBQLCli.cpp
	
#lib/libws2_32.a
#$(CPP) -c client.cpp -o client.o $(CXXFLAGS)

client: client.o
	$(CC) client.o -o client
	
client.o: client.cpp
	$(CC) -c client.cpp
	
clean:
	rm -rf *.o a.out

	