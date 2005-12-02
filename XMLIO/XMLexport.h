#ifndef __XMLEXPORT_H__
#define __XMLEXPORT_H__

#include <string>
#include <sstream>
#include "../Driver/Connection.h"
#include "../Driver/DriverManager.h"
#include "../Driver/Result.h"
#include "../Store/Store.h"
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
XERCES_CPP_NAMESPACE_USE

//using namespace Driver; <- rozumiem ze docelowo bedzie namespace

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
	
	class QueryExporter
	{
		public:
			virtual int produce(DOMElement* parent) = 0;
	};

	class QuerySequenceExporter : public QueryExporter
	{
	protected:
		ResultSequence *result;
	public:
		QuerySequenceExporter(ResultSequence* _result) 
		{
				result = _result;
		}
		
		int produce(DOMElement* parent);
	};

	class QueryBagExporter : public QueryExporter
	{
	protected:
		ResultBag* result;
	public:
		QueryBagExporter(ResultBag* _result) 
		{
				result = _result;
		}
		
		int produce(DOMElement* parent);			
	};

	class QueryStructExporter : public QueryExporter
	{
	protected:
		ResultStruct* result;
	public:
		QueryStructExporter(ResultStruct* _result) 
		{
				result = _result;
		}
		
		int produce(DOMElement* parent);
	};
/*
	class QueryBinderExporter : public QueryExporter
	{
	protected:
		ResultBinder* result;
	public:
		QueryBinderExporter(ResultBinder* _result)
		{
				result = _result;
		}
		
		int produce(DOMElement* parent);
	};
*/
	class QueryStringExporter : public QueryExporter
	{
	protected:
		ResultString* result;
	public:
		QueryStringExporter(ResultString* _result)
		{
				result = _result;
		}
		
		int produce(DOMElement* parent);
	};

	class QueryIntExporter : public QueryExporter
	{
	protected:
		ResultInt *result;
	public:
		QueryIntExporter(ResultInt* _result)
		{
				result = _result;
		}
		
		int produce(DOMElement* parent);			
	};
/*
	class QueryDoubleExporter : public QueryExporter
	{
	protected:
		ResultDouble *result;
	public:
		QueryDoubleExporter(ResultDouble* _result)
		{
				result = _result;
		}
		
		int produce(DOMElement* parent);			
	};
*/	
	class QueryBoolExporter : public QueryExporter
	{
	protected:
		ResultBool *result;
	public:
		QueryBoolExporter(ResultBool* _result)
		{
				result = _result;
		}
		
		int produce(DOMElement* parent);			
	};

	class QueryReferenceExporter : public QueryExporter
	{
	protected:
		ResultReference *result;
	public:
		QueryReferenceExporter(ResultReference *_result)
		{
				result = _result;
		}
		
		int produce(DOMElement* parent);			
	};

	class QueryVoidExporter : public QueryExporter
	{
	protected:
		ResultVoid *result;
	public:
		QueryVoidExporter(ResultVoid *_result)
		{
				result = _result;
		}
		
		int produce(DOMElement* parent);			
	};

	class QueryExporterFactory
	{
	public:
		QueryExporter* makeExporter(Result* result);
	};


    class XMLExporter
    {
	protected:
		string host;
		unsigned int port;
		string xmlPath;
		string query;
		int verboseLevel;
	
		int produceXMLfile(Result *result);
	public:
		XMLExporter(string aHost, unsigned int aPort, string aQuery, string aXmlPath, int aVerboseLevel = 0)
		{
			host = aHost;
			port = aPort;
			query = aQuery;
		    xmlPath = aXmlPath;
		    verboseLevel = aVerboseLevel;
		}	
	
		int exec();
	};
};

#endif
