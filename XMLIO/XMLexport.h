#ifndef __XMLEXPORT_H__
#define __XMLEXPORT_H__

#include <string>
#include <sstream>

#include "../Store/Store.h"
#include "../Store/Roots.h"
#include "../Store/Map.h"
#include "../Store/DBStoreManager.h"
#include "../Store/DBLogicalID.h"
#include "../Config/SBQLConfig.h"
#include "../Log/Logs.h"
#include "../TransactionManager/Transaction.h"
#include "../Errors/ErrorConsole.h"
#include "XMLcommon.h"

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMWriter.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#if defined(XERCES_NEW_IOSTREAMS)
#include <iostream>
#else
#include <iostream.h>
#endif
#include <xercesc/util/OutOfMemoryException.hpp>

using namespace std;
using namespace Store;
using namespace Errors;

XERCES_CPP_NAMESPACE_USE

namespace XMLIO 
{
	static int ERR_CONNECTION = -1;
	static int ERR_NULL_RESULT = -2;
	static int ERR_UNKNOWN_RESULT = -3;
	static int ERR_XERCES = -4;

    class XMLExporter
    {
	protected:
		DBStoreManager* store;
		TransactionID* tid;
		FileFormat fileFormat;		
		int verboseLevel;
		int printObject(ObjectPointer* obj, DOMElement* parent);
	public:
		int make(string xmlPath);
		XMLExporter(DBStoreManager* _store, int _VerboseLevel, FileFormat _fileFormat)
		{
			store = _store;
			tid = new TransactionID(1);
		    verboseLevel = _VerboseLevel;
		    fileFormat = _fileFormat;
		}	
		
	};
};

#endif
