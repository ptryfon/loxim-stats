#include <string.h>
#include <protocol/packages/WSHelloPackage.h>
#include <protocol/ptools/PackageBufferWriter.h>
#include <protocol/ptools/PackageBufferReader.h>
#include <protocol/constants.h>

#include <stdlib.h>

using namespace protocol;

WSHelloPackage::WSHelloPackage( 
			uint8_t a_p_major,\
			uint8_t a_p_minor,\
			uint8_t a_s_major,\
			uint8_t a_s_minor,\
			uint32_t a_maxPackageSize,\
			uint64_t a_featuresMap,\
			uint64_t a_authMethodsMap,\
			char a_salt[20])
{
	p_minor=a_p_minor;
	p_major=a_p_major;
	s_minor=a_s_minor;
	s_major=a_s_major;
	maxPackageSize=a_maxPackageSize;
	featuresMap=a_featuresMap;
	authMethodsMap=a_authMethodsMap;
	memcpy(salt,a_salt,20);		
}


WSHelloPackage::WSHelloPackage()
{
	p_minor=PROTO_VERSION_MAJOR;
	p_major=PROTO_VERSION_MINOR;
	s_minor=LOXIM_VERSION_MAJOR;
	s_major=LOXIM_VERSION_MINOR;
	maxPackageSize=64*1024;
	featuresMap=0;
	authMethodsMap=0;
	for(int i=0; i<20; i++)
		salt[i]=(rand()%256)-128;	
}

WSHelloPackage::WSHelloPackage(\
			uint32_t a_maxPackageSize,\
			uint64_t a_featuresMap,\
			uint64_t a_authMethodsMap)
{
	p_minor=PROTO_VERSION_MAJOR;
	p_major=PROTO_VERSION_MINOR;
	s_minor=LOXIM_VERSION_MAJOR;
	s_major=LOXIM_VERSION_MINOR;
	featuresMap=a_featuresMap;
	maxPackageSize=a_maxPackageSize;
	authMethodsMap=a_authMethodsMap;
	for(int i=0; i<20; i++)
		salt[i]=(rand()%256)-128;	
}
			
void WSHelloPackage::serializeW(PackageBufferWriter *writer)
{
	writer->writeUint8(p_major);
	writer->writeUint8(p_minor);
	writer->writeUint8(s_major);
	writer->writeUint8(s_minor);
	writer->writeUint32(maxPackageSize);
	writer->writeUint64(featuresMap);
	writer->writeUint64(authMethodsMap);
	for(int i=0; i<20; i++)
		writer->writeUint8(salt[i]);
}

bool  WSHelloPackage::deserializeR(PackageBufferReader *reader)
{
	p_major=reader->readUint8();
	p_minor=reader->readUint8();
	s_major=reader->readUint8();
	s_minor=reader->readUint8();
	maxPackageSize=reader->readUint32();
	featuresMap=reader->readUint64();
	authMethodsMap=reader->readUint64();
	for(int i=0; i<20; i++)
		salt[i]=reader->readUint8();

	return true;
}

bool WSHelloPackage::equals(Package* package)
 {
 	if (!package)
 		return false;
 	
 	if (package->getPackageType()!=this->getPackageType())
 		return false; 	
   	if (((WSHelloPackage*)package)->getP_minor()!=getP_minor())
   		return false;   		
   	if (((WSHelloPackage*)package)->getP_major()!=getP_major())
   		return false;   		
   	if (((WSHelloPackage*)package)->getS_minor()!=getS_minor())
   		return false;   		
   	if (((WSHelloPackage*)package)->getS_major()!=getS_major())
   		return false;
   	
   	if (((WSHelloPackage*)package)->getMaxPackageSize()!=getMaxPackageSize())
   		return false;
   	if (((WSHelloPackage*)package)->getFeaturesMap()!=getFeaturesMap())
   		return false;
	if (((WSHelloPackage*)package)->getAuthMethodsMap()!=getAuthMethodsMap())
   		return false;
   	
   	if (((WSHelloPackage*)package)->getAuthMethodsMap()!=getAuthMethodsMap())
   		return false;
   	
   	if (memcmp(((WSHelloPackage*)package)->getSalt(),getSalt(),20)!=0)
   		return false;
   		
   	return true;
 }

