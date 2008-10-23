#include <stdio.h>
#include <stdlib.h>

#include "../sockets/TCPIPServerSocket.h"
#include "../layers/ProtocolLayer0.h"
#include "../base_packages/ASCPingPackage.h"
#include "TestSuiteFactory.h"

using namespace protocol;

int main(int argc,char** argv)
{
	if(argc<2)
	{
		printf("Exprected args: ./TestRunnerRec [port] [testSuiteName]?");
		fflush(stdout);
		return -1;
	}
	printf("Opening server socket on port: %d\n",atoi(argv[1]));
	TCPIPServerSocket* serversocket=new TCPIPServerSocket(NULL,atoi(argv[1]));
	serversocket->bind();
	AbstractSocket *socket=serversocket->accept();
	ProtocolLayer0 *pl0=new ProtocolLayer0(socket);
	if(argc==2)
	{
		TestSuiteFactory::callAllRecTests(pl0);
	}else{
		TestSuiteFactory::callRecTest(pl0,argv[2]);
	}		
	
	delete pl0;
	socket->close();
	delete socket;
	serversocket->close();
	delete serversocket;
	printf("Finished");
}
