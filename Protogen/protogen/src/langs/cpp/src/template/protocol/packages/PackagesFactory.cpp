#include "PackagesFactory.h"

#include <stdlib.h>
#include <stdio.h>

#include "../base_packages/ASCPingPackage.h"
#include "../base_packages/ASCPongPackage.h"


#define PACKAGE_INIT_CASE( PACKAGENAME ) case ID_##PACKAGENAME: \
		{  \
			Package *res=new PACKAGENAME();\
			if(!res->deserialize(data))\
			{\
				delete res;\
				return NULL;\
			};\
			return res;\
		}\

using namespace protocol;
 

Package *PackagesFactory::createPackage(uint8_t type/*, uint32_t size*/, char* data)
{
	switch(type)
	{
	
		PACKAGE_INIT_CASE(ASCPingPackage)
		PACKAGE_INIT_CASE(ASCPongPackage)
		
		default:
		{
			printf("Nie znany typ pakietu");
			//TODO: Zapisz do logu informacjê o nieznanym typie pakietu 
			return NULL;
		}
	}
}
