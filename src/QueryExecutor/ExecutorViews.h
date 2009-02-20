#ifndef _EXECUTOR_VIEWS_H_
#define _EXECUTOR_VIEWS_H_

#include <string>
#include "HashMacro.h"

namespace Store {class LogicalID;}
namespace Errors {class ErrorConsole;}
namespace TManager {class Transaction;}
namespace QExecutor {class QueryResult; class QueryVirtualResult; class QueryBinderResult; class QueryBagResult; class QueryExecutor;}

using namespace Store;
using namespace TManager;
using namespace Errors;
using namespace std;

namespace QExecutor
{
	class QueryResult;

	class ExecutorViews
	{
		#define QE_SELF_KEYWORD					"self"
		#define QE_SUPER_KEYWORD				"super"
		#define QE_NOTROOT_VIEW_PARENT_NAME		"ViewParent"
		#define QE_VIRTUALS_TO_SEND_MIN_ID		0xFE000000
		#define QE_VIRTUALS_TO_SEND_MAX_COUNT	0x01000000


		class hashOfString {
			private:
				hash<const char*> hasher;
		
			public:
				size_t operator()(const string& s) const {
					return hasher(s.c_str());
				}
		};
	
		struct eqOfString{
			bool operator()(const string& s1, const string& s2) const{
				return s1 == s2;
			}
		};

		typedef hash_map<string, LogicalID*, hashOfString, eqOfString> QVirtualsToFakeLidMap;
	
		private:
			QVirtualsToFakeLidMap fakeLid_map;
			vector<QueryResult*> sent_virtuals;
			ErrorConsole *ec;
			
		public:
			ExecutorViews();
			int deVirtualize(QueryResult *arg, QueryResult *&res);
			int reVirtualize(QueryResult *arg, QueryResult *&res);	    
		    int checkViewAndGetVirtuals(LogicalID *lid, string &name, string &code, Transaction *tr);
			int getSubviews(LogicalID *lid, string vo_name, vector<LogicalID *> &subviews, Transaction *tr);
			int getSubview(LogicalID *lid, string name, LogicalID *&subview_lid, Transaction *tr);
			int getOn_procedure(LogicalID *lid, string procName, string &code, string &param, Transaction *tr);
			int createNewSections(QueryVirtualResult *qvirt, QueryBinderResult *param, LogicalID *viewdef, vector<QueryBagResult*> &sections, Transaction *tr, QueryExecutor *qe);    	
	};
}

#endif //_EXECUTOR_VIEWS_H_
