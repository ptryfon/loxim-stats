#include "ProtocolLayer0.h"

#include "../packages/ASCPingPackage.h"
#include "../packages/ASCPongPackage.h"

#include <stdio.h>

using namespace protocol;

ProtocolLayer0::ProtocolLayer0(AbstractInputStream *a_inputStream, AbstractOutputStream *a_outputStream)
{
	inputStream=new PackageInputStream(a_inputStream);
	priorityOutputStream=new PriorityOutputStream(a_outputStream);
	outputStream=new PackageOutputStream(priorityOutputStream);
	lastError = 0;
}

ProtocolLayer0::ProtocolLayer0(AbstractSocket *a_socket)
{
	inputStream=new PackageInputStream(a_socket->getInputStream());
	priorityOutputStream=new PriorityOutputStream(a_socket->getOutputStream());
	outputStream=new PackageOutputStream(priorityOutputStream);
	lastError = 0;
}

ProtocolLayer0::~ProtocolLayer0()
{	
	if (priorityOutputStream!=NULL)
		delete priorityOutputStream;
	
	if (inputStream!=NULL)
		delete inputStream;
		
	if (outputStream!=NULL)
		delete outputStream;
}
	

Package *ProtocolLayer0::readPackage()
{
	return readPackage(0);
}

Package *ProtocolLayer0::readPackage(long timeout)
{
	bool interpreted=true;
	Package *p=NULL;
	
	do
	{
		/*Czytaj pakiety - a¿ trafisz na taki, którego nie zinterpretujesz
		 * w tej warstwie - wtedy zwróæ go.
  		 */
		p=inputStream->readPackage(timeout);
		if (!p)
		{
			lastError = inputStream->getStatus();
			//printf("dosta³em null'a\n");
			/*TODO: ZALOGUJ O ZERWANIU KOMUNIKACJI*/
			return NULL;
		}
		interpreted=interpretePackage(p);
		if (interpreted)
				delete p;
		
	}while(interpreted);
	lastError = 0;
	return p;
}

bool ProtocolLayer0::interpretePackage(Package *package)
{
	if (package->getPackageType()==ID_ASCPingPackage)
	{
		Package *pong=new ASCPongPackage();
		writePackagePriority(pong);
		delete pong;
		return true;
	};	
	return false;
}
	
bool ProtocolLayer0::writePackage(Package *package)
{
	return outputStream->writePackage(package)==0;
}

bool ProtocolLayer0::writePackagePriority(Package *package)
{
	return outputStream->writePackagePriority(package)==0;
}

int ProtocolLayer0::getLastError()
{
	return lastError;
}
