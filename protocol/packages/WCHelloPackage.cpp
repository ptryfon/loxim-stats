#include <string.h>
#include "WCHelloPackage.h"
#include "../ptools/PackageBufferWriter.h"
#include "../ptools/PackageBufferReader.h"

#include <stdlib.h>

using namespace protocol;

WCHelloPackage::WCHelloPackage(\
			int64_t a_pid,\
			char* a_programName,\
			char* a_programVersion,\
			char* a_hostname,\
			char* a_language,\
			uint64_t a_collation,\
			int8_t a_timezone )
{
		programName=new CharArray(strdup(a_programName)),
		programVersion=new CharArray(strdup(a_programVersion));
		hostname=new CharArray(strdup(a_hostname));
		language=new CharArray(strdup(a_language));
		pid=a_pid;
		collation=a_collation;
		timezone=a_timezone;
}

WCHelloPackage::WCHelloPackage(
			int64_t a_pid,
			CharArray* a_programName,\
			CharArray* a_programVersion,\
			CharArray* a_hostname,\
			CharArray* a_language,\
			uint64_t a_collation,\
			int8_t a_timezone )
{
		programName=a_programName;
		programVersion=a_programVersion;
		hostname=a_hostname;
		language=a_language;
		pid=a_pid;
		collation=a_collation;
		timezone=a_timezone;
}

WCHelloPackage::WCHelloPackage()
{
		programName=NULL;
		programVersion=NULL;
		hostname=NULL;
		language=NULL;
		pid=0;
		collation=0;
		timezone=0;
}

WCHelloPackage::~WCHelloPackage()
{
	if (programName) delete programName; 
	if (programVersion) delete programVersion;
	if (hostname) delete hostname;
	if (language) delete language;
}
			
void WCHelloPackage::serializeW(PackageBufferWriter *writer)
{
	writer->writeInt8(pid);
	writer->writeSstring(programName);
	writer->writeSstring(programVersion);
	writer->writeSstring(hostname);
	writer->writeSstring(language);//3 bajty kodu jêzyka
	writer->writeUint64(collation);
	writer->writeInt8(timezone);
}

bool  WCHelloPackage::deserializeR(PackageBufferReader *reader)
{			
	pid=reader->readInt8();
	programName=reader->readSstring();
	programVersion=reader->readSstring();
	hostname=reader->readSstring();
	language=reader->readSstring();
	collation=reader->readInt64();
	timezone=reader->readInt8();	
	
	return true;
}

bool WCHelloPackage::equals(Package* package)
 {
 	if (!package)
 		return false;
 	
 	if (package->getPackageType()!=this->getPackageType())
 		return false;
 		
 	WCHelloPackage *wcp=(WCHelloPackage*)package;
 	
 	if ( (getPid()!=wcp->getPid())
 		||(getCollation()!=wcp->getCollation())
 		|| (getTimezone()!=wcp->getTimezone()))
 		return false;
 	
 	if(!CharArray::equal(wcp->getHostname(),getHostname()))
 		return false;
 		
 	if(!CharArray::equal(wcp->getLanguage(),getLanguage()))
 		return false;
 		
 	if(!CharArray::equal(wcp->getProgramName(),getProgramName()))
 		return false;
 		
 	if(!CharArray::equal(wcp->getProgramVersion(),getProgramVersion()))
 		return false;	
 		
 	return true;
 }


