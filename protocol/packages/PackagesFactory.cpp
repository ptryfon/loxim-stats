#include "PackagesFactory.h"

#include <stdlib.h>
#include <stdio.h>

#include "ASCByePackage.h"
#include "ASCErrorPackage.h"
#include "ASCOkPackage.h"
#include "ASCPingPackage.h"
#include "ASCPongPackage.h"
#include "ASCSetOptPackage.h"
#include "WCHelloPackage.h"
#include "WCLoginPackage.h"
#include "WCModePackage.h"
#include "WCPasswordPackage.h"
#include "WSAuthorizedPackage.h"
#include "WSHelloPackage.h"

#include "QCStatementPackage.h"
#include "QSStmtParsedPackage.h"
#include "QCExecutePackage.h"
#include "QSExecutionFinishedPackage.h"

#include "VSCSendValuesPackage.h"
#include "VSCSendValuePackage.h"
#include "VSCFinishedPackage.h"
#include "VSCAbortPackage.h"


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
 

Package *PackagesFactory::createPackage(uint8_t type, uint32_t size, char* data)
{
	switch(type)
	{
	
		PACKAGE_INIT_CASE(ASCByePackage)
		PACKAGE_INIT_CASE(ASCErrorPackage)
		PACKAGE_INIT_CASE(ASCOkPackage)
		PACKAGE_INIT_CASE(ASCPingPackage)
		PACKAGE_INIT_CASE(ASCPongPackage)
		PACKAGE_INIT_CASE(ASCSetOptPackage)
		PACKAGE_INIT_CASE(WCHelloPackage)
		PACKAGE_INIT_CASE(WCLoginPackage)
		PACKAGE_INIT_CASE(WCModePackage)
		PACKAGE_INIT_CASE(WCPasswordPackage)
		PACKAGE_INIT_CASE(WSAuthorizedPackage)
		PACKAGE_INIT_CASE(WSHelloPackage)
		
		PACKAGE_INIT_CASE(QCStatementPackage)
		PACKAGE_INIT_CASE(QSStmtParsedPackage)
		PACKAGE_INIT_CASE(QCExecutePackage)
		PACKAGE_INIT_CASE(QSExecutionFinishedPackage)
		
		PACKAGE_INIT_CASE(VSCSendValuesPackage)
		PACKAGE_INIT_CASE(VSCSendValuePackage)
		PACKAGE_INIT_CASE(VSCFinishedPackage)
		PACKAGE_INIT_CASE(VSCAbortPackage)			
		
		default:
		{
			printf("Nie znany typ pakietu");
			//TODO: Zapisz do logu informacjê o nieznanym typie pakietu 
			return NULL;
		}
	}
}
