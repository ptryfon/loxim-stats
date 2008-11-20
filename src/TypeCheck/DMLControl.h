#ifndef _TC_DMLCONTROL_
#define _TC_DMLCONTROL_

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <string>
//#include <QueryParser/Stack.h>
#include <TypeCheck/ClassNames.h>
//#include <Store/DBStoreManager.h>
//#include <Store/DBObjectPointer.h>
#include <TransactionManager/Transaction.h>
#include <QueryParser/Deb.h>

using namespace TManager;

namespace TypeCheck {

	class DMLControl {

	public: enum MdnRootKind {absent, object, type};
		private:
			Transaction *tr;	//always received from executor
			map<string, vector<string>* > typeDeps;//defined type dependencies (names only),to prevent from infinite recurrence
			map<string, MdnRootKind> rootMdns;	//each elt: if present - either object declaration or type definition

			int registerRootMdns(vector<ObjectPointer *> *v, MdnRootKind mdnKind);
			int registerRootMdnsRec(vector<LogicalID*> *v, string rootName, MdnRootKind mdnKind, bool topLevel);
		public:
			DMLControl(Transaction *t);
			DMLControl(){};
			int init(int sessionId);
			void setTransaction(Transaction *t);
			int findDependency (string masterType, string subType);
			int findRoot(string name);
			void addRoot(string name, MdnRootKind mdnKind);
			void markDependency(string masterType, string subType);
			string depsToString();
			string rootMdnsToString();
			~DMLControl();
	};

}

#endif
