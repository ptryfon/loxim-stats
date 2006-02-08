#include "XMLexport.h"


using namespace XMLIO;
namespace XMLIO {
	
int XMLExporter::printObject(ObjectPointer* obj, DOMElement* parent)
{
	DOMDocument* doc = parent->getOwnerDocument();
	DOMElement* elem = doc->createElement(X(obj->getName().c_str()));
	
	DataValue* val = obj->getValue();
	switch (val->getType()) {
		case Store::Integer:
		case Store::Double:
		case Store::String:
			{
				DOMText* data = doc->createTextNode(X(val->toString().c_str())); 
				elem->appendChild(data);
			}
			break;
		case Store::Pointer:
			/* TODO */
			{
				DOMText* data = doc->createTextNode(X(val->toString().c_str())); 
				elem->appendChild(data);
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
					else cerr << "ERR\n";
				}
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
                    X("data"),	          // root element name
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