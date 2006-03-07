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
	
	class SAXExtendedHandler : public HandlerBase
	{
		private:
		DBStoreManager* store;
		stack <DataValue* > elementValues;
		stack <string> elementNames;
		ostream* out;
		TransactionID* tid;

		public:
		
		SAXExtendedHandler(DBStoreManager* _store, TransactionID *_tid) {
			store = _store;
			tid = _tid;
		}
		
		void startDocument() {			
		}
		
		void endDocument() {
		}
		
		void characters(const XMLCh* const chars, const unsigned int length) {
			if (elementValues.empty()) return;
			string s = XMLString::transcode(chars);			
			switch (elementValues.top()->getType()) {
				case Integer:
					elementValues.top()->setInt(atoi(s.c_str()));
					break;
				case Double:
					elementValues.top()->setDouble(atof(s.c_str()));
					break;
				case String:
					elementValues.top()->setString(s);
					break;
				case Pointer:
					/* TODO */
					break;
				case Vector:
					/* IGNORUJEMY */
					break;
				default:
					/* docelowo wykrywany przez DTD*/
					throw new SAXException(X("Nieznany typ danych"));	
			}
		}
		
		void startElement(const XMLCh* const name, AttributeList &attributes) {
			string nodeName = XMLString::transcode(name);
								
			if (nodeName == "NODE" ) {
				if (attributes.getValue("name") == NULL) throw new SAXException(X("Nie znaleziono atrybutu 'name'"));
				string realName = XMLString::transcode(attributes.getValue("name"));				
				if (attributes.getValue("type") == NULL) throw new SAXException(X("Nie znaleziono atrybutu 'type'"));
				string type = XMLString::transcode(attributes.getValue("type"));			
				elementNames.push(realName);
				
				if (type == "integer") {
					elementValues.push(store->createIntValue(0));
				} else if (type == "double") {
					elementValues.push(store->createDoubleValue(0.0));				
				} else if (type == "string") {
					elementValues.push(store->createStringValue(""));				
				} else if (type == "pointer") {
					/* TODO */
				} else if (type == "vector") {
					elementValues.push(store->createVectorValue(new vector<LogicalID*>()));
				} else {
					/* TRAKTUJEMY ELEMENTY BEZ WSKAZANEGO TYPU JAKO vector */
					elementValues.push(store->createVectorValue(new vector<LogicalID*>()));
				}			
				
			} else if (nodeName == "DATA") {
				/* jakastam weryfikacja by sie przydala */
			} else {
				/* docelowo wykrywany przez DTD*/
				throw new SAXException(X("Nieznany tag XML"));
			}
			 
		}
		
		void endElement(const XMLCh *const name) {
			string nodeName = XMLString::transcode(name);
			if (nodeName == "NODE") {			
				string elementName = elementNames.top();
				DataValue* elementValue = elementValues.top();
				elementNames.pop();
				elementValues.pop();
				
				ObjectPointer* element;
				store->createObject(tid, elementName, elementValue, element);
				
				if (elementValues.empty()) {
					 store->addRoot(tid, element);
				} else {
					if (elementValues.top()->getType() == Vector) {
						vector<LogicalID* >* vec = elementValues.top()->getVector();
						vec->push_back(element->getLogicalID());
					} else {
						throw new SAXException(X("endElement() - to nie powinno nigdy zajsc"));
					}
				}
			} else if (nodeName == "DATA") {
				/* nic nie robimy */
			}
		}
	};
	
		
	int XMLImporter::make(string xmlPath)
	{				
		try {
			XMLPlatformUtils::Initialize();	
		} catch (const XMLException& e) {
			cerr << "XMLException" << e.getMessage() << "\n";			
			return -1;
		}
		
		TransactionID* tid = new TransactionID(1);
		
		HandlerBase* handler;
		
		if (fileFormat == FORMAT_SIMPLE) {		
			handler = new SAXDatabaseHandler(store, tid);
		} else {
			handler = new SAXExtendedHandler(store, tid);
		}		
		SAXParser* parser = new SAXParser();
		parser->setDocumentHandler(handler);
		ErrorHandler* errorHandler = new HandlerBase();
		parser->setErrorHandler(errorHandler);
		try {
			parser->parse(xmlPath.c_str());
		} catch (const SAXParseException& e) {
			cerr << "[ERROR] Parse exception (line " << e.getLineNumber() << " /column "<< e.getColumnNumber() << " )\n";
		} catch (const SAXException& e) {
			cerr << "[ERROR] Parse exception: " << e.getMessage() << "\n";
		} catch (const XMLException& e) {
			cerr << "[ERROR] XML exception: " << e.getMessage() << "\n";
			return -1;
		} catch (...) {
			cerr << "[ERROR] Parse exception\n";			
		};
		delete parser;
		delete handler;
		delete errorHandler;
		XMLPlatformUtils::Terminate();
		return 0;		
	}	
};		
