#ifndef XMLIMPORT_H_
#define XMLIMPORT_H_

#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <string>
#include <stack>

#include "../Store/Store.h"
#include "../Store/Roots.h"
#include "../Store/Map.h"
#include "../Store/DBStoreManager.h"
#include "../Store/DBLogicalID.h"
#include "../Config/SBQLConfig.h"
#include "../Log/Logs.h"
#include "../TransactionManager/Transaction.h"
#include "../Errors/ErrorConsole.h"

using namespace std;
using namespace Store;
using namespace Errors;

XERCES_CPP_NAMESPACE_USE

namespace XMLIO {
	
    class XMLImporter
    {
	protected:
		DBStoreManager* store;
		int verboseLevel;	
	public:
		XMLImporter(DBStoreManager* _store, int aVerboseLevel)
		{
			store = _store;
		    verboseLevel = aVerboseLevel;
		}	
		int make(string xmlPath);		
	};
			
};


#endif /*XMLIMPORT_H_*/
