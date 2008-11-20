#include <protocol/packages/PackagesFactory.h>

#include <stdlib.h>
#include <stdio.h>

#include <protocol/packages/ASCByePackage.h>
#include <protocol/packages/ASCErrorPackage.h>
#include <protocol/packages/ASCOkPackage.h>
#include <protocol/packages/ASCPingPackage.h>
#include <protocol/packages/ASCPongPackage.h>
#include <protocol/packages/ASCSetOptPackage.h>
#include <protocol/packages/WCHelloPackage.h>
#include <protocol/packages/WCLoginPackage.h>
#include <protocol/packages/WCModePackage.h>
#include <protocol/packages/WCPasswordPackage.h>
#include <protocol/packages/WSAuthorizedPackage.h>
#include <protocol/packages/WSHelloPackage.h>

#include <protocol/packages/QCStatementPackage.h>
#include <protocol/packages/QSStmtParsedPackage.h>
#include <protocol/packages/QCExecutePackage.h>
#include <protocol/packages/QSExecutionFinishedPackage.h>

#include <protocol/packages/VSCSendValuesPackage.h>
#include <protocol/packages/VSCSendValuePackage.h>
#include <protocol/packages/VSCFinishedPackage.h>
#include <protocol/packages/VSCAbortPackage.h>


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
