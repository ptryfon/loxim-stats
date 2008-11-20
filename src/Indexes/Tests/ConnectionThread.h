#ifndef CONNECTIONTHREAD_H_
#define CONNECTIONTHREAD_H_

#include <string>

#include <QueryParser/QueryParser.h>
#include <QueryExecutor/QueryExecutor.h>

using namespace QParser;
using namespace QExecutor;

namespace IndexTesting {
	
	class ConnectionThread {
		private:
			auto_ptr<QueryParser> qPa;
			auto_ptr<QueryExecutor> qEx;
		
		public:
			
			ConnectionThread();
		
			int process(string query);

			int process(string query, QueryResult* &qResult);

			int process(string query, QueryResult::QueryResultType resultType);

			int begin();

			int end();

			int abort();
	};
	
}

#endif /*CONNECTIONTHREAD_H_*/
