#include <stdio.h>
#include <stdlib.h>

#include "../sockets/TCPIPServerSocket.h"
#include "../layers/ProtocolLayer0.h"
#include "../base_packages/ASCPingPackage.h"
#include "TestSuiteFactory.h"

using namespace protocol;

int main(int argc,char** argv)
{
	if(argc<3)
	{
		printf("Exprected args: ./TestRunnerSender [host] [port] [testSuiteName]?");
		fflush(stdout);
		return -1;
	}
	printf("Opening server socket on: %s:%d\n",argv[1],atoi(argv[2]));
	TCPIPClientSocket* clientsocket=new TCPIPClientSocket(argv[1],atoi(argv[2]));
	clientsocket->connect();
	ProtocolLayer0 *pl0=new ProtocolLayer0(clientsocket);
	if(argc==3)
	{
		TestSuiteFactory::callAllSendTests(pl0);
	}else{
		TestSuiteFactory::callSendTest(pl0,argv[3]);
	}		
	
	delete pl0;
	clientsocket->close();
	delete clientsocket;
	printf("Finished");
}
