#ifndef _RESULTSTACK_H_
#define _RESULTSTACK_H_

#include <vector>
#include <string>

namespace Errors { class ErrorConsole; }

namespace QExecutor
{
	class QueryResult;

	class ResultStack
	{
		protected: 
			Errors::ErrorConsole *ec;
			std::vector<QueryResult*> rs;
		public:
			ResultStack();
			~ResultStack();
			int push(QueryResult *r);
			int pop(QueryResult *&r);
			bool empty();
			int size();
			void deleteAll();
			std::string toString(); 
	};
}

#endif //_RESULTSTACK_H_
