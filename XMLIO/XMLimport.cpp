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
		FileFormat fileFormat;

		public:
		
		SAXDatabaseHandler(DBStoreManager* _store, TransactionID *_tid, FileFormat _fileFormat) {
			store = _store;
			tid = _tid;
			fileFormat = _fileFormat;
		}
		
		void startDocument() {			
		}
		
		void endDocument() {
		}
		
		void characters(const XMLCh* const chars, const unsigned int length) {
			string s = XMLString::transcode(chars);
			DataValue* newData = store->createStringValue(s);						
			ObjectPointer* optr;
			store->createObject(tid, "TEXT", newData, optr);			
			if (childElements.empty()) {
				 store->addRoot(tid, optr);
			} else {
				childElements.top()->push_back(optr->getLogicalID());
			}							
		}
		
		void startElement(const XMLCh* const name, AttributeList &attributes) {	
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
		if (fileFormat != FORMAT_SIMPLE) {
			cerr << "Format rozszerzony nie jest jeszcze zaimplementowany\n";
			return -1;
		}
				
		try {
			XMLPlatformUtils::Initialize();	
		} catch (const XMLException& e) {
			cerr << "XMLException\n";
			return -1;
		}
		
		TransactionID* tid = new TransactionID(1);
		
		SAXDatabaseHandler* handler = new SAXDatabaseHandler(store, tid, fileFormat);
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
