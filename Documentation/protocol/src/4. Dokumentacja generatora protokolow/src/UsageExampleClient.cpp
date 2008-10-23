#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "sockets/TCPIPClientSocket.h"
#include "layers/ProtocolLayer0.h"
#include "./packages/W_c_helloPackage.h"
#include "./packages/A_sc_byePackage.h"
#include "./enums/CollationsFactory.h"

using namespace protocol;

int main(int argc, char** argv) {
	printf("Opening server socket on: %s:%d\n", argv[1], atoi(argv[2]));

	TCPIPClientSocket* clientsocket=new TCPIPClientSocket(argv[1],atoi(argv[2]));
	clientsocket->connect();

	ProtocolLayer0 *pl0=new ProtocolLayer0(clientsocket);

	printf("Sending invitation...\n");
	Package * p=new W_c_helloPackage(getpid(),new CharArray("testClient"),
			NULL,NULL,NULL, coll_unicode_collation_alghorithm_uts10,2);
	pl0->writePackage(p);
	delete pl0;

	printf("Waiting for package...");
	p=pl0->readPackage();
	printf("... got it");
	if ((p)&&(p->getPackageType()==ID_A_sc_byePackage)) {
		printf("Received package\n");
		delete p;
	} else {
		printf("Unexpected package\n");
		if (p)
			delete p;
	}

	delete pl0;
	clientsocket->close();
	delete clientsocket;
	printf("Finished");
}
