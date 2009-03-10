#ifndef _QUERYEXECUTOR_H
#define _QUERYEXECUTOR_H

#include <string>
#include <vector>
#include <map>

#include "Signatures.h"
#include "QueryResult.h"
#include "QueryParser/DataRead.h"

namespace QExecutor { class EnvironmentStack; class ClassGraph; class ResultStack; class ExecutorViews;}
namespace Server { class Session; }
namespace QParser {	class QueryParser; }
namespace Errors { class ErrorConsole; }
namespace Schemas { class Schema; class InterfaceMaps; class OuterSchemas; }
namespace TManager { class Transaction; }
namespace Store { class LogicalID; class DBDataValue; class ObjectPointer;}

using namespace QParser;
using namespace TManager;
using namespace Errors;
using namespace Store;
using namespace std;

namespace QExecutor
{
	class QueryExecutor
	{
	protected:   
		Signatures sigs;
		ExecutorViews *exViews;
		ErrorConsole *ec;
		Transaction *tr;
		ClassGraph *cg;
		Schemas::InterfaceMaps *im;
		Schemas::OuterSchemas *os;
		EnvironmentStack *envs;
		ResultStack *qres;
		map<string, QueryResult*> *prms;
		
		int stop;
		int transactionNumber;
		bool antyStarve;
		
		bool evalStopped() { return ( stop != 0 ); };
		int executeRecQuery(TreeNode *tree, bool checkPrivilieges = true);
		int combine(NonAlgOpNode::nonAlgOp op, QueryResult *curr, QueryResult *lRes, QueryResult *&partial);
		int unOperate(UnOpNode::unOp op, QueryResult *arg, QueryResult *&final, bool checkPrivilieges);
		int coerceOperate(int cType, QueryResult *arg, QueryResult *&final, TreeNode *tree);
		int algOperate(AlgOpNode::algOp op, QueryResult *lArg, QueryResult *rArg, QueryResult *&final, AlgOpNode *tn, bool checkPrivilieges);
		int derefQuery(QueryResult *arg, QueryResult *&res);
		int refQuery(QueryResult *arg, QueryResult *&res);
        int nameofQuery(QueryResult *arg, QueryResult *&res);	
				
		int createObjectAndPutOnQRes(DBDataValue* dbValue, string objectName, Store::ExtendedType type, LogicalID*& newLid);
		int executeObjectDeclarationOrTypeDefinition(DeclareDefineNode *obdNode, bool isTypedef);
		int execRecursivQueryAndPop(TreeNode *query, QueryResult*& execResult);	
		
		int lidFromVector( string bindName, vector<QueryResult*> value, LogicalID*& lid);
		int lidFromString( string bindName, string value, LogicalID*& lid);		
		int lidFromReference( string bindName, LogicalID* value, LogicalID*& lid);
		int lidFromInt(string bindName, int value, LogicalID*& lid);	
		int lidFromBinder( string bindName, QueryResult* result, LogicalID*& lid);		
		void pushStringToLIDs(string name, string bindName, vector<LogicalID*>& lids);	  
		void pushStringsToLIDs(set<string>* names, string bindName, vector<LogicalID*>& lids);
		int objectFromBinder(QueryResult *res, ObjectPointer *&newObject);
		
		int persistDelete(QueryResult* bagArg, bool checkPrivilieges = false);		
		int persistDelete(LogicalID *lid, bool checkPrivilieges = false);	
		int persistDelete(vector<LogicalID*>* lids);
		int persistStaticMembersDelete(const string& object_name);
	
    	void set_user_data(ValidationNode *node);
    	int execute_locally(string query, QueryResult **result);
		int execute_locally(string query, QueryResult **result, QueryParser &parser); //for multi-queries
    	bool assert_grant_priv(string priv_name, string name);
    	bool assert_revoke_priv(string priv_name, string name);
    	bool assert_create_user_priv();        	
    	bool assert_remove_user_priv();        	
    	bool assert_bool_query(string query);
    	bool assert_privilige(string priv, string object);
    	bool priviliged_mode;

	public:
        Server::Session *session;
		bool inTransaction;
		
		QueryExecutor(Server::Session *session);
		~QueryExecutor();
		
		EnvironmentStack* getEnvs() { return envs; }
		ClassGraph* getCg() { return cg; }
		ExecutorViews* getExViews() { return exViews;}
		const Schemas::InterfaceMaps* getIm() {return im; }
		
		bool is_dba();
		int initCg();				
		int executeQuery(TreeNode *tree, map<string, QueryResult*>* params, QueryResult **result);
		int executeQuery(TreeNode *tree, QueryResult **result);
		void stopExecuting() { stop = 65535; };
		void contExecuting() { stop = 0; };
		void antyStarveFunction(int errcode);
		int abort();
		void set_priviliged_mode(bool mode);
		int procCheck(unsigned int qSize, LogicalID *lid, string &code, vector<string> &prms, int &founded);
		int callProcedure(string code, vector<QueryBagResult*> sections);
		int createOnView(string objectName, bool &found, QueryResult *binder, QueryResult *&result);
		int pop_qres(QueryResult *&r);
			
		int trErrorOccur( string msg, int errcode );	
		int qeErrorOccur( string msg, int errcode );	
		int otherErrorOccur( string msg, int errcode );		
	};
}

#endif
