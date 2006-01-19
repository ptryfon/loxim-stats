#ifndef XMLIMPORT_H_
#define XMLIMPORT_H_

#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <string>
#include <stack>
#include "../Driver/Connection.h"
#include "../Driver/Result.h"
#include "../Driver/DriverManager.h"

using namespace std;

XERCES_CPP_NAMESPACE_USE

namespace XMLIO {
	class QueryExecutor {
		public:
		virtual int execute(const string &query) {return -1;};		
		virtual ~QueryExecutor() {};
	};
	
	class QueryExecutorFile : public QueryExecutor {
		ofstream *fileStream;
		public:
		QueryExecutorFile(const string &fileName);
		virtual ~QueryExecutorFile();
		int execute(const string &query);
	};
	
	class QueryExecutorDatabase : public QueryExecutor {
		Driver::Connection *con;
		public:
		QueryExecutorDatabase(const string &host, unsigned int port);
		virtual ~QueryExecutorDatabase();
		int execute(const string &query);
	};
	
	class XMLImporter {
		private:
			string inputFile;
			int verboseLevel;
			QueryExecutor* executor;			
			int dump();
		public:
			XMLImporter(string _inputFile, int _verboseLevel = 0) {
				inputFile = _inputFile;
				verboseLevel = _verboseLevel;
			}
			int dumpToFile(string outPath);
			int dumpToServer(string host, unsigned int port);
	};
};

#endif /*XMLIMPORT_H_*/
