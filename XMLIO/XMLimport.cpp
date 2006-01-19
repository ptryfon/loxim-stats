#include "XMLimport.h"
#include <iostream>
#include <fstream>
#include <list>


namespace XMLIO {
	
	enum TagType {
		UNKNOWN=0,
		ROOT=1,
		ELEMENT=2,		
		ATOM=3,
	};
	
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
	
	class Node
	{
		public:
		virtual void addData(string s) {};
		virtual void addChild(Node *n) {};
		virtual string toString() { return "";};
	};
	
	class Value : public Node
	{
		private:
		string value;
		DataType type;
		public:
		Value(DataType _type) {
			type = _type;
			value = "";
		}
		void addData(string s) {
			value = value + s;
		}	
		string toString() {			
			if (type == STRING) {
				return "\"" + value + "\"";
			} else {
				/* TODO - weryfikacja danych */
				return value;
			}
		}
	};
	
	class Root : public Node
	{
		private:
		list<Node*> childNodes;
		public:
		Root() {
		}
		void addChild(Node *n) {			
			childNodes.push_back(n);
		}
		string toString() {
			/* TYLKO W CELACH TESTOWYCH */
			string result;			
			for (list<Node*>::iterator i = childNodes.begin(); i != childNodes.end(); i++) {
				result = result + "CREATE " + (*i)->toString() + ";\n";
			}
			return result;
		}
		int execute(QueryExecutor* executor) {
			int result;
			for (list<Node*>::iterator i = childNodes.begin(); i != childNodes.end(); i++) {
				result = executor->execute("CREATE " + (*i)->toString() + ";");
				if (result != 0) return result;
			}
			return 0;
		}		
	};
	
	class Vector : public Node
	{
		private:
		string name;
		list<Node*> childNodes; 
		public:
		Vector(string _name) {
			name = _name;
		}
		void addChild(Node *n) {
			childNodes.push_back(n);
		}		
		string toString() {
			string result = name + "(";
			for (list<Node*>::iterator i = childNodes.begin(); i != childNodes.end(); i++) {
				if (i != childNodes.begin()) result = result + ";";
				result = result + (*i)->toString();
			}
			result = result + ")";
			return result;
		}		
	};
				
	class SAXDatabaseHandler : public HandlerBase
	{
		private:
		stack <Node*> elementStack;
		ostream* out;

		public:
		
		SAXDatabaseHandler(Root* dataRoot) {
			elementStack.push(dataRoot);
		}
		
		void startDocument() {
		}
		
		void endDocument() {
		}
		
		void characters(const XMLCh* const chars, const unsigned int length) {
			elementStack.top()->addData(XMLString::transcode(chars));
		}
		
		void startElement(const XMLCh* const name, AttributeList &attributes) {					
			string prettyName = XMLString::transcode(name);
			if (prettyName == "ELEMENT") {
				Node* newNode = new Vector(XMLString::transcode(attributes.getValue("name")));
				/* UWAGA - potencjalny memory leak */
				elementStack.top()->addChild(newNode);
				elementStack.push(newNode);
			} else if (prettyName == "ATOM") {
				string prettyType = XMLString::transcode(attributes.getValue("type"));
				Node* newNode;
				if (prettyType == "string") {
					newNode = new Value(STRING);
				} else if (prettyType == "integer") {
					newNode = new Value(INT);
				} else if (prettyType == "boolean") {
					newNode = new Value(BOOL);
				} else if (prettyType == "double") {
					newNode = new Value(DOUBLE);
				} else if (prettyType == "reference") {
					newNode = new Value(REFERENCE);
				} else {
					newNode = new Value(VOID);
				}
				elementStack.top()->addChild(newNode);
				elementStack.push(newNode);				
			} else if (prettyName == "DATA") {
				/* nie robimy nic - bo element DATA juz mamy */
			} else elementStack.push(NULL);
		}
		
		void endElement(const XMLCh *const name) {
			string prettyName = XMLString::transcode(name);			
			if ((prettyName == "ELEMENT") || (prettyName == "ATOM")) {
				elementStack.pop();				
			}
		}
	};
	
	QueryExecutorFile::QueryExecutorFile(const string &fileName) {
		fileStream = new ofstream(fileName.c_str());
	}
	
	QueryExecutorFile::~QueryExecutorFile() {
		fileStream->close();
		delete fileStream;
	}
	
	int QueryExecutorFile::execute(const string &query) {
		(*fileStream) << query << "\n";
		/* TODO - jakies sprawdzanie bledow */
		return 0;
	}
	
	QueryExecutorDatabase::QueryExecutorDatabase(const string &host, unsigned int port) {
		char c_host[host.length()];
		strcpy(c_host, host.c_str());
	    con = Driver::DriverManager::getConnection(c_host, port);
	}
	
	QueryExecutorDatabase::~QueryExecutorDatabase() {
		con->disconnect();
		delete con;
	}
	
	int QueryExecutorDatabase::execute(const string &query) {
		Driver::Result* result = con->execute(query.c_str());
		/* TODO - zrobic cos z wynikiem */
		return 0;
	}
	
	int XMLImporter::dump()
	{
		try {
			XMLPlatformUtils::Initialize();	
		} catch (const XMLException& e) {
			cerr << "XMLException\n";
			return -1;
		}
		Root* rootNode = new Root();
		
		SAXDatabaseHandler* handler = new SAXDatabaseHandler(rootNode);
		SAXParser* parser = new SAXParser();
		parser->setDocumentHandler(handler);				
		try {
			parser->parse(inputFile.c_str());
			rootNode->execute(executor);
		} catch (const XMLException& e) {
			cerr << "XML exception\n";
			return -1;
		}			
		delete parser;
		delete handler;
		delete rootNode;						
		XMLPlatformUtils::Terminate();
		return 0;		
	}
	
	int XMLImporter::dumpToFile(string outPath) 
	{
		executor = new QueryExecutorFile(outPath);
		int result = dump();
		delete executor;
		return result;
	}
	int XMLImporter::dumpToServer(string host, unsigned int port) {
		executor = new QueryExecutorDatabase(host, port);
		int result = dump();
		delete executor;
		return result;
	}
};
