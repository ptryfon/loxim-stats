#include "XMLexport.h"


using namespace XMLIO;

int main(int argc, char* argv[]) {	
	return 0;
}

namespace XMLIO {
	
int QuerySequenceExporter::produce(DOMElement* parent)
{
	DOMDocument* doc = parent->getOwnerDocument();
	DOMElement* elem = doc->createElement(X("sequence"));
	for (int i = 0; i < result->size(); i++) 
	{
		QueryExporterFactory f;
		QueryExporter* exporter = f.makeExporter(result->get(i));
		exporter->produce(elem);
		delete exporter;
	}
	parent->appendChild(elem);
	return 0;
}

int QueryBagExporter::produce(DOMElement* parent)
{
	DOMDocument* doc = parent->getOwnerDocument();
	DOMElement* elem = doc->createElement(X("bag"));
	for (int i = 0; i < result->size(); i++) 
	{
		QueryExporterFactory f;
		QueryExporter* exporter = f.makeExporter(result->get(i));
		exporter->produce(elem);
		delete exporter;
	}
	parent->appendChild(elem);
	return 0;
}

int QueryStructExporter::produce(DOMElement* parent)
{
	DOMDocument* doc = parent->getOwnerDocument();
	DOMElement* elem = doc->createElement(X("struct"));
	for (int i = 0; i < result->size(); i++) 
	{
		QueryExporterFactory f;
		QueryExporter* exporter = f.makeExporter(result->get(i));
		exporter->produce(elem);
		delete exporter;
	}
	parent->appendChild(elem);
	return 0;
}

/* TODO - zalezne od postepow w Driverze
int QueryBinderExporter::produce(DOMElement* parent)
{
	DOMDocument* doc = parent->getOwnerDocument();
	DOMElement* elem = doc->createElement(X("binder"));
	elem->setAttribute(X("name"),X(qr->getName()));
	
	QueryExporterFactory f;
	QueryExporter* exporter = f.makeExporter(qr[i]);
	exporter->produce(elem);
	delete exporter;

	parent->appendChild(elem);
	return 0;
}
*/

int QueryStringExporter::produce(DOMElement* parent)
{
	DOMDocument* doc = parent->getOwnerDocument();
	DOMElement* elem = doc->createElement(X("string"));
	DOMCDATASection* data = doc->createCDATASection(X(result->getValue()));
	elem->appendChild(data);
	parent->appendChild(elem);
	return 0;
}

int QueryIntExporter::produce(DOMElement* parent)
{	
	DOMDocument* doc = parent->getOwnerDocument();
	DOMElement* elem = doc->createElement(X("int"));
	stringstream s;
	result->toStream(s);
	DOMText* data = doc->createTextNode(X(s.str()));
	elem->appendChild(data);
	parent->appendChild(elem);	
	return 0;
}
/*
int QueryDoubleExporter::produce(DOMElement* parent)
{
	DOMDocument* doc = parent->getOwnerDocument();
	DOMElement* elem = doc->createElement(X("double"));
	stringstream s;
	result->toStream(s);
	DOMText* data = doc->createTextNode(X(s));
	elem->appendChild(data);
	parent->appendChild(elem);	
	return 0;
}
*/
int QueryBoolExporter::produce(DOMElement* parent)
{
	DOMDocument* doc = parent->getOwnerDocument();
	DOMElement* elem = doc->createElement(X("bool"));
	stringstream s;
	result->toStream(s);
	DOMText* data = doc->createTextNode(X(s.str()));
	elem->appendChild(data);
	parent->appendChild(elem);	
	return 0;	
}

int QueryReferenceExporter::produce(DOMElement* parent)
{
	DOMDocument* doc = parent->getOwnerDocument();
	DOMElement* elem = doc->createElement(X("reference"));
	stringstream s;
	result->toStream(s);
	DOMText* data = doc->createTextNode(X(s.str()));
	elem->appendChild(data);
	parent->appendChild(elem);	
	return 0;		
	return 0;
}

int QueryVoidExporter::produce(DOMElement* parent)
{
	DOMDocument* doc = parent->getOwnerDocument();
	DOMElement* elem = doc->createElement(X("void"));
	parent->appendChild(elem);	
	return 0;
}

QueryExporter* QueryExporterFactory::makeExporter(Result* result) 
{
	switch (result->getType()) 
	{			
		case Result::SEQUENCE:		
			return new QuerySequenceExporter(dynamic_cast<ResultSequence*>(result));	
			break;
		case Result::BAG:		
			return new QueryBagExporter(dynamic_cast<ResultBag*>(result));
			break;
		
		case Result::STRUCT:
			return new QueryStructExporter(dynamic_cast<ResultStruct*>(result));
			break;
/*		
		case Result::BINDER:
			return new QueryBinderExporter(dynamic_cast<ResultBinder*>(result));
			break;
*/		
		case Result::STRING:
			return new QueryStringExporter(dynamic_cast<ResultString*>(result));
			break;		
			
		case Result::INT:
			return new QueryIntExporter(dynamic_cast<ResultInt*>(result));
			break;				
/*		
		case Result::DOUBLE:
			return new QueryDoubleExporter(dynamic_cast<QueryDoubleResult*>(result));
			break;				
*/		
		case Result::BOOL:
			return new QueryBoolExporter(dynamic_cast<ResultBool*>(result));
			break;						
		
		case Result::REFERENCE:
			return new QueryReferenceExporter(dynamic_cast<ResultReference*>(result));
			break;						
		
		case Result::VOID:
			return new QueryVoidExporter(dynamic_cast<ResultVoid*>(result));
			break;						
		
		default:
			return NULL;
	}
}	

int XMLExporter::produceXMLfile(Result *result)
{
	int returnVal = 0;
	try
    {
        XMLPlatformUtils::Initialize();
		DOMImplementation* impl =  DOMImplementationRegistry::getDOMImplementation(X("Core"));
        DOMWriter *serializer = impl->createDOMWriter();	
        /* TODO - moze trzeba ustalic jakies opcje serializera? */
		if (impl == NULL) {
			returnVal = ERR_XERCES;
		} else {
			try
			{
            	DOMDocument* doc = impl->createDocument(
                	0,                    // root element namespace URI. /* TODO - ustalic */
                    X("data"),	          // root element name
                    0);                   // document type object (DTD). /* TODO - ustalic */
	        	DOMElement* rootElem = doc->getDocumentElement();
				QueryExporterFactory factory;
			    QueryExporter* rootExporter = factory.makeExporter(result);
				if (rootExporter == NULL) {
					returnVal = ERR_UNKNOWN_RESULT;
				} else {
					returnVal = rootExporter->produce(rootElem);
					if (returnVal == 0) {
			            XMLFormatTarget* formatTarget = new LocalFileFormatTarget(xmlPath.c_str());
						serializer->writeNode(formatTarget, *doc);
						delete formatTarget;
					}
				}               
			}
			catch(...)
			{
				returnVal = ERR_XERCES;
			}
		}
       delete serializer;		
        XMLPlatformUtils::Terminate();
    }
    catch(...)
    {
    	/* TODO - sensowna obsluga wyjatkow Xercesa*/
        return ERR_XERCES;
    }
	return returnVal;
}

int XMLExporter::exec()
{
	int returnVal = 0;
	char cHost[255];
	char cQuery[255];
	strcpy(cHost, host.c_str());
	strcpy(cQuery, host.c_str());
	
	Connection* con;
	con = DriverManager::getConnection(cHost, port);	
	if (con == NULL) {
		returnVal = ERR_CONNECTION;
	} else {
	    Result* result = con->execute(cQuery);
	    if (result == NULL) {
	    	returnVal = ERR_NULL_RESULT;
	    } else {
			returnVal = produceXMLfile(result);	    	
	    }
	    con->disconnect();	    
    }
    return returnVal;
}

};
