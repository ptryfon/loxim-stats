#ifndef _RESULTSTACK_H_
#define _RESULTSTACK_H_

#include <vector>

namespace Errors { class ErrorConsole; }

using namespace std;
using namespace Errors;

namespace QExecutor
{
	class QueryResult;

	class ResultStack
	{
		protected: 
			ErrorConsole *ec;
			vector<QueryResult*> rs;
		public:
			ResultStack();
			~ResultStack();
			int push(QueryResult *r);
			int pop(QueryResult *&r);
			bool empty();
			int size();
			void deleteAll();
			string toString(); 
	};
}

#endif //_RESULTSTACK_H_
