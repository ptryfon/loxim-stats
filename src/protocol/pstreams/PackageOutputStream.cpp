#include <protocol/pstreams/PackageOutputStream.h>

#include <stdlib.h>

using namespace protocol;
	
PackageOutputStream::PackageOutputStream(PriorityOutputStream *a_outputStream)
{
	outputStream=a_outputStream;
}

PackageOutputStream::~PackageOutputStream()
{
}
	
int PackageOutputStream::writePackage(Package *package)
{
	char *buf=NULL;
	int len=package->serialize(&buf);
	if ((len<=0)||(buf==NULL))
		return -1; //TODO: Kody b³êdów
	
	int res=outputStream->write(buf,0,len);
	free(buf);
	if(res<=0)
		return res;
	else return 0;/*SUKCES*/
}

int PackageOutputStream::writePackagePriority(Package *package)
{
	char *buf=NULL;
	int len=package->serialize(&buf);
	if ((len<=0)||(buf==NULL))
		return -1; //TODO: Kody b³êdów
	
	int res=outputStream->writePriority(buf,0,len);
	free(buf);
	if(res<=0)
		return res;
	else return 0;/*SUKCES*/
}
