#include "XMLimport.h"
#include <iostream>

using namespace XMLIO;

namespace XMLIO {
		
	enum DataType {
		NA=0,
		STRING=1,
		INT=2,
		DOUBLE=3,
		BOOL=4, 
		REFERENCE=5,
		VOID=6
//		UNKNOWN=10;
	};
				
	class SAXDatabaseHandler : public HandlerBase
	{
		private:
		DBStoreManager* store;
		stack <vector<LogicalID*>* > childElements;
		ostream* out;
		TransactionID* tid;

		public:
		
		SAXDatabaseHandler(DBStoreManager* _store, TransactionID *_tid) {
			store = _store;
			tid = _tid;
		}
		
		void startDocument() {			
		}
		
		void endDocument() {
		}
		
		void characters(const XMLCh* const chars, const unsigned int length) {
			//elementStack.top()->addData(XMLString::transcode(chars));
		}
		
		void startElement(const XMLCh* const name, AttributeList &attributes) {			
			ObjectPointer* op;
			childElements.push(new vector<LogicalID*>());
		}
		
		void endElement(const XMLCh *const name) {
			DataValue* newData = store->createVectorValue(childElements.top());		
			childElements.pop();
						
			ObjectPointer* optr;
			store->createObject(tid, XMLString::transcode(name), newData, optr);			
			if (childElements.empty()) {
				 store->addRoot(tid, optr);
			} else {
				childElements.top()->push_back(optr->getLogicalID());
			}
		}
	};
		
	int XMLImporter::make(string xmlPath)
	{
		try {
			XMLPlatformUtils::Initialize();	
		} catch (const XMLException& e) {
			cerr << "XMLException\n";
			return -1;
		}
		
		TransactionID* tid = new TransactionID(1);
		
		SAXDatabaseHandler* handler = new SAXDatabaseHandler(store, tid);
		SAXParser* parser = new SAXParser();
		parser->setDocumentHandler(handler);				
		try {
			parser->parse(xmlPath.c_str());
		} catch (const XMLException& e) {
			cerr << "XML exception\n";
			return -1;
		}			
		delete parser;
		delete handler;
		XMLPlatformUtils::Terminate();
		return 0;		
	}
	
	
};		
