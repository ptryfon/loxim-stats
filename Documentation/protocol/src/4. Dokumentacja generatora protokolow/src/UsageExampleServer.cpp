#include <stdio.h>
#include <stdlib.h>

#include "./ptools/Package.h"
#include "./sockets/TCPIPServerSocket.h"
#include "./layers/ProtocolLayer0.h"
#include "./packages/W_c_helloPackage.h"
#include "./packages/A_sc_byePackage.h"

using namespace protocol;

int main(int argc,char** argv)
{
	printf("Opening server socket on port: %d\n",atoi(argv[1]));
	
	TCPIPServerSocket* serversocket=new TCPIPServerSocket(NULL,atoi(argv[1]));
	serversocket->bind();
	AbstractSocket *socket=serversocket->accept();
	
	ProtocolLayer0 *pl0=new ProtocolLayer0(socket);
	
	Package* p=pl0->readPackage();
	if((p)&&(p->getPackageType()==ID_W_c_helloPackage))
	{
		printf("Received package\n");
		delete p;
		
		printf("Sending response...\n");		
		p=new A_sc_byePackage();
		pl0->writePackage(p);		
		delete p;
	}else{
		printf("Unexpected package\n");
		if (p)
			delete p;
	}
	
	delete pl0;
	socket->close();
	delete socket;
	serversocket->close();
	delete serversocket;
	printf("Finished");
}
