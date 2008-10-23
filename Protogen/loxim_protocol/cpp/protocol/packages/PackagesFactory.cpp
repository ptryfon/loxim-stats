#include <stdlib.h>
#include <stdio.h>

#include "PackagesFactory.h"


#include "../base_packages/ASCPingPackage.h"
#include "../base_packages/ASCPongPackage.h"

#include "W_c_helloPackage.h"
#include "W_s_helloPackage.h"
#include "W_c_modePackage.h"
#include "W_c_loginPackage.h"
#include "W_c_authorizedPackage.h"
#include "W_c_passwordPackage.h"
#include "Q_c_statementPackage.h"
#include "Q_s_stmtparsedPackage.h"
#include "Q_c_executePackage.h"
#include "Q_s_executingPackage.h"
#include "Q_s_execution_finishedPackage.h"
#include "A_sc_okPackage.h"
#include "A_sc_errorPackage.h"
#include "A_sc_byePackage.h"
#include "A_sc_setoptPackage.h"
#include "V_sc_sendvaluesPackage.h"
#include "V_sc_sendvaluePackage.h"
#include "V_sc_finishedPackage.h"
#include "V_sc_abortPackage.h"
using namespace protocol;

Package *PackagesFactory::createPackageNotInit(uint8_t type)
{
	switch(type)
	{
	
	

		case ID_ASCPingPackage: return new ASCPingPackage();
		case ID_ASCPongPackage: return new ASCPongPackage();

		case ID_W_c_helloPackage: return new W_c_helloPackage();
		case ID_W_s_helloPackage: return new W_s_helloPackage();
		case ID_W_c_modePackage: return new W_c_modePackage();
		case ID_W_c_loginPackage: return new W_c_loginPackage();
		case ID_W_c_authorizedPackage: return new W_c_authorizedPackage();
		case ID_W_c_passwordPackage: return new W_c_passwordPackage();
		case ID_Q_c_statementPackage: return new Q_c_statementPackage();
		case ID_Q_s_stmtparsedPackage: return new Q_s_stmtparsedPackage();
		case ID_Q_c_executePackage: return new Q_c_executePackage();
		case ID_Q_s_executingPackage: return new Q_s_executingPackage();
		case ID_Q_s_execution_finishedPackage: return new Q_s_execution_finishedPackage();
		case ID_A_sc_okPackage: return new A_sc_okPackage();
		case ID_A_sc_errorPackage: return new A_sc_errorPackage();
		case ID_A_sc_byePackage: return new A_sc_byePackage();
		case ID_A_sc_setoptPackage: return new A_sc_setoptPackage();
		case ID_V_sc_sendvaluesPackage: return new V_sc_sendvaluesPackage();
		case ID_V_sc_sendvaluePackage: return new V_sc_sendvaluePackage();
		case ID_V_sc_finishedPackage: return new V_sc_finishedPackage();
		case ID_V_sc_abortPackage: return new V_sc_abortPackage();


		default:
		{
			printf("Nie znany typ pakietu");
			//TODO: Zapisz do logu informacjê o nieznanym typie pakietu 
			return NULL;
		}
	}
}

Package *PackagesFactory::createPackage(uint8_t type, char* data)
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

Package *PackagesFactory::createPackageContent(uint8_t type,PackageBufferReader* reader)
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

