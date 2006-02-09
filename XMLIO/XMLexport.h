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


/*
 *  Ponizsza klasa to copy&paste z przykladow do Xerces'a - przejrzec
 *  i sprawdzic co z tym lepiej zrobic
 */ 

// ---------------------------------------------------------------------------
//  This is a simple class that lets us do easy (though not terribly efficient)
//  trancoding of char* data to XMLCh data.
// ---------------------------------------------------------------------------
class XStr
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
       XStr(string toTranscode)
    {
	fUnicodeForm = XMLString::transcode(toTranscode.c_str());
    }


    XStr(const char* const toTranscode)
    {
        // Call the private transcoding method
        fUnicodeForm = XMLString::transcode(toTranscode);
    }
     ~XStr()
    {
        XMLString::release(&fUnicodeForm);
    }


    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    const XMLCh* unicodeForm() const
    {
        return fUnicodeForm;
    }

private :
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fUnicodeForm
    //      This is the Unicode XMLCh format of the string.
    // -----------------------------------------------------------------------
    XMLCh*   fUnicodeForm;
};

#define X(str) XStr(str).unicodeForm()


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
