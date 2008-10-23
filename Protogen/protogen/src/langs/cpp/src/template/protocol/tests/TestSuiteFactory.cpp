#include "TestSuiteFactory.h"
#include "../ptools/Package.h"
#include "TestPackagesFactory.h"

using namespace protocol;

bool TestSuiteFactory::callAllRecTests(ProtocolLayer0* socket)
{
	callRecTest(socket,"main");
	return true;
}

bool TestSuiteFactory::callRecTest(ProtocolLayer0* socket,char* testSuiteName)
{
	Package *p;
	int i=0;
	int failures=0;
	
	while((p=TestPackagesFactory::createPackage(i))!=NULL)
	{
		fprintf(stdout,"Waiting for package: %d",i);
		fflush(stdout);
		
		Package *r=socket->readPackage();
		
		fprintf(stdout,"..received");
		fflush(stdout);
		
		if(p->equals(r))
			fprintf(stdout,"[equal]\n");
		else
		{
			fprintf(stdout,"[NOT EQUAL] !!!\n");
			failures++;
		}
		fflush(stdout);
		
		i++;
	}
	fprintf(stdout,"FOUND %d FAILURES\n",failures);
	return true;
}

bool TestSuiteFactory::callAllSendTests(ProtocolLayer0* socket)
{
	callSendTest(socket,"main");
	return true;
}

bool TestSuiteFactory::callSendTest(ProtocolLayer0* socket,char* testSuiteName)
{
	Package *p;
	int i=0;
	while((p=TestPackagesFactory::createPackage(i))!=NULL)
	{
		fprintf(stdout,"Sending package: %d",i);
		fflush(stdout);
		if (i%2)
			socket->writePackage(p);
		else
			socket->writePackagePriority(p);
		fprintf(stdout,"..sent\n");
		fflush(stdout);
		i++;
	}
	return true;
}
