include ../make.defs

LIBS= ../Config/Config.a \
	  ../Errors/Errors.a \
	  ../Log/Logs.a \
	  ../QueryParser/QueryParser.a \
	  ../Store/Store.a \
	  ../TransactionManager/TransactionManager.a \
	  ../QueryExecutor/QueryExecutor.a \
	  
#SBQLCli: SBQLCli.o ResultSet.o DriverManager.o Connection.o
#	$(CC) SBQLCli.o ResultSet.o DriverManager.o Connection.o -o SBQLCli

#SBQLCli.o: SBQLCli.cpp
#	$(CC) -c SBQLCli.cpp
	
#lib/libws2_32.a
#$(CPP) -c client.cpp -o client.o $(CXXFLAGS)

client: client.o
	$(CPP) client.o $(LIBS)  -o client 
	
client.o: client.cpp
	$(CPP) -c $(CXXFLAGS) client.cpp
	
clean:
	rm -rf *.o a.out

	