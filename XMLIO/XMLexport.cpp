#include "XMLexport.h"


using namespace XMLIO;
namespace XMLIO {
	
int XMLExporter::printObject(ObjectPointer* obj, DOMElement* parent)
{
	DOMDocument* doc = parent->getOwnerDocument();
	DOMElement* elem;
	switch (fileFormat) {
		case FORMAT_SIMPLE:
			elem = doc->createElement(X(obj->getName().c_str()));		
			break;
		case FORMAT_EXTENDED:
			elem = doc->createElement(X("NODE"));
			elem->setAttribute(X("name"), X(obj->getName().c_str()));			
			break;
		default:
			return -1;
	}
	
	DataValue* val = obj->getValue();
	switch (val->getType()) {
		case Store::Integer:
			{
				DOMText* data = doc->createTextNode(X(val->toString().c_str())); 
				elem->appendChild(data);
				if (fileFormat == FORMAT_EXTENDED) elem->setAttribute(X("type"), X("integer"));
			}
			break;		
		case Store::Double:
			{
				DOMText* data = doc->createTextNode(X(val->toString().c_str())); 
				elem->appendChild(data);
				if (fileFormat == FORMAT_EXTENDED) elem->setAttribute(X("type"), X("double"));
			}
			break;		
		case Store::String:
			{
				DOMText* data = doc->createTextNode(X(val->getString().c_str())); 
				elem->appendChild(data);
				if (fileFormat == FORMAT_EXTENDED) elem->setAttribute(X("type"), X("string"));				
			}
			break;
		case Store::Pointer:
			/* TODO */
			{
				DOMText* data = doc->createTextNode(X(val->toString().c_str())); 
				elem->appendChild(data);
				if (fileFormat == FORMAT_EXTENDED) elem->setAttribute(X("type"), X("pointer"));					
			}
			break;
		case Store::Vector:
			{
				vector<LogicalID*>::iterator pi;
				for(pi=val->getVector()->begin(); pi!=val->getVector()->end(); pi++)
				{
					ObjectPointer* obj;
					store->getObject(tid, (*pi), Read, obj);
					if (obj != NULL) printObject(obj, elem);
					else return -2;
				}
				if (fileFormat == FORMAT_EXTENDED) elem->setAttribute(X("type"), X("vector"));
			}							
			break;
		default:
			break;
	}		
	parent->appendChild(elem);	
	return 0;
}

int XMLExporter::make(string xmlPath)
{
	int returnVal = 0;
	cerr << "[DEBUG] Producing XML file\n";
	try
    {    	
		cerr << "[DEBUG] Initializing XMLPlatfomUtils\n";    	
        XMLPlatformUtils::Initialize();
        cerr << "[DEBUG] Initializing DOM\n";
		DOMImplementation* impl =  DOMImplementationRegistry::getDOMImplementation(X("Core"));
		cerr << "[DEBUG] Createing DOMWriter\n";
        DOMWriter *serializer = impl->createDOMWriter();	
        /* TODO - moze trzeba ustalic jakies opcje serializera? */
		if (impl == NULL) {
			returnVal = ERR_XERCES;
		} else {
			try
			{
				cerr << "[DEBUG] Creating document\n";
            	DOMDocument* doc = impl->createDocument(
                	0,                    // root element namespace URI. 
                    X("DATA"),	          // root element name
                    0);                   // document type object (DTD).
	        	DOMElement* rootElem = doc->getDocumentElement();
	        	
	        	vector<ObjectPointer*>* vec;
				cerr << "[DEBUG] getRoots(ALL)" << endl;
				store->getRoots(tid, vec);
	
				vector<ObjectPointer*>::iterator oi;
				for(oi=vec->begin(); oi!=vec->end(); oi++) {
					returnVal = printObject(*oi, rootElem);
					if (returnVal > 0) break;
				}
	        	delete vec;
	        	
				if (returnVal == 0) {
					cerr << "[DEBUG] Serializing XML\n";
		            XMLFormatTarget* formatTarget = new LocalFileFormatTarget(xmlPath.c_str());
					serializer->writeNode(formatTarget, *doc);						
					delete formatTarget;
					cerr << "[DEBUG] Done.\n";
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

};