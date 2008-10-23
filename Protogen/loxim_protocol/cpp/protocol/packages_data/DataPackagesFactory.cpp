#include <stdlib.h>
#include <stdio.h>

#include "DataPackagesFactory.h"

#include "Uint8Package.h"
#include "Sint8Package.h"
#include "Uint16Package.h"
#include "Sint16Package.h"
#include "Uint32Package.h"
#include "Sint32Package.h"
#include "Uint64Package.h"
#include "Sint64Package.h"
#include "BoolPackage.h"
#include "DatePackage.h"
#include "TimePackage.h"
#include "DatetimePackage.h"
#include "TimetzPackage.h"
#include "DatetimetzPackage.h"
#include "BobPackage.h"
#include "VarcharPackage.h"
#include "DoublePackage.h"
#include "VoidPackage.h"
#include "LinkPackage.h"
#include "BindingPackage.h"
#include "CollectionPackage.h"
#include "StructPackage.h"
#include "BagPackage.h"
#include "SequencePackage.h"
#include "RefPackage.h"
#include "External_refPackage.h"
using namespace protocol;

Package *DataPackagesFactory::createPackageNotInit(uint8_t type)
{
	switch(type)
	{
	
	
		case ID_Uint8Package: return new Uint8Package();
		case ID_Sint8Package: return new Sint8Package();
		case ID_Uint16Package: return new Uint16Package();
		case ID_Sint16Package: return new Sint16Package();
		case ID_Uint32Package: return new Uint32Package();
		case ID_Sint32Package: return new Sint32Package();
		case ID_Uint64Package: return new Uint64Package();
		case ID_Sint64Package: return new Sint64Package();
		case ID_BoolPackage: return new BoolPackage();
		case ID_DatePackage: return new DatePackage();
		case ID_TimePackage: return new TimePackage();
		case ID_DatetimePackage: return new DatetimePackage();
		case ID_TimetzPackage: return new TimetzPackage();
		case ID_DatetimetzPackage: return new DatetimetzPackage();
		case ID_BobPackage: return new BobPackage();
		case ID_VarcharPackage: return new VarcharPackage();
		case ID_DoublePackage: return new DoublePackage();
		case ID_VoidPackage: return new VoidPackage();
		case ID_LinkPackage: return new LinkPackage();
		case ID_BindingPackage: return new BindingPackage();
		case ID_StructPackage: return new StructPackage();
		case ID_BagPackage: return new BagPackage();
		case ID_SequencePackage: return new SequencePackage();
		case ID_RefPackage: return new RefPackage();
		case ID_External_refPackage: return new External_refPackage();


		default:
		{
			printf("Nie znany typ pakietu");
			//TODO: Zapisz do logu informacjê o nieznanym typie pakietu 
			return NULL;
		}
	}
}

Package *DataPackagesFactory::createPackage(uint8_t type, char* data)
{
	Package* p=createPackageNotInit(type);
	if(!p)
		return NULL;
	if(!p->deserialize(data))
	{
		delete p;
		return NULL;
	}
	return p;
}

Package *DataPackagesFactory::createPackageContent(uint8_t type,PackageBufferReader* reader)
{
	Package* p=createPackageNotInit(type);
	if(!p)
		return NULL;
	if(!p->deserializeContent(reader))
	{
		delete p;
		return NULL;
	}
	return p;
};

