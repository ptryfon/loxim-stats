#ifndef _QUERYEXECUTOR_H
#define _QUERYEXECUTOR_H

#include <stdio.h>
#include <string>
#include <vector>
#include <map>
   
#include "QueryResult.h"
#include "EnvironmentStack.h"
#include "SessionData.h"
#include "TransactionManager/Transaction.h"
#include "Store/Store.h"
#include "Store/DBDataValue.h"
#include "Store/DBLogicalID.h"
#include "QueryParser/QueryParser.h"
#include "QueryParser/Privilige.h"
#include "QueryParser/TreeNode.h"
#include "Errors/Errors.h"
#include "Errors/ErrorConsole.h"

using namespace QParser;
using namespace SessionDataNms;
using namespace TManager;
using namespace Errors;
using namespace Store;
using namespace std;

namespace QExecutor
{
    
    class ProcedureInfo
    {
    public:
        string ProcCode;
        vector<string> Params;
        virtual ~ProcedureInfo(){};
    };
	
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
    		string toString() {
          stringstream c;
          string sizeS;
          c << rs.size(); c >> sizeS;
          string result = "[ResultStack] size=" + sizeS + "\n";

          for( unsigned int i = 0; i < rs.size(); i++ ) {
            result += rs[i]->toString( 1, true, "rs_elem" );
          }

          return result;
        }
	};

	class QueryExecutor
	{
	protected:
    
//[sk153407, pk167277 - BEGIN]
/*
nie wiem gdzie zaincjalizowac
        static const std::string ProcBody  = "ProcBody";
        static const std::string ParamName = "ParamName";
        static const std::string ParamKind = "Kind";
*/
//[sk153407, pk167277 - END]
    
		ErrorConsole *ec;
		Transaction *tr;
		EnvironmentStack *envs;
		ResultStack *qres;
		map<string, QueryResult*> *prms;
		int stop;
		int transactionNumber;
		bool inTransaction;
		bool antyStarve;
		
		void antyStarveFunction(int errcode);
		
		bool evalStopped() { return ( stop != 0 ); };
		int executeRecQuery(TreeNode *tree);
		int combine(NonAlgOpNode::nonAlgOp op, QueryResult *curr, QueryResult *lRes, QueryResult *&partial);
		int merge(NonAlgOpNode::nonAlgOp op, QueryResult *partial, QueryResult *&final);
		int unOperate(UnOpNode::unOp op, QueryResult *arg, QueryResult *&final);
		int algOperate(AlgOpNode::algOp op, QueryResult *lArg, QueryResult *rArg, QueryResult *&final);
		int derefQuery(QueryResult *arg, QueryResult *&res);
		int refQuery(QueryResult *arg, QueryResult *&res);
		int isIncluded(QueryResult *elem, QueryResult *set, bool &score);
		int sortBag(QueryBagResult *inBag, QueryBagResult *&outBag);
		int objectFromBinder(QueryResult *res, ObjectPointer *&newObject);
        	
        	void set_user_data(ValidationNode *node);
        	int execute_locally(string query, QueryResult **result);
        	bool is_dba();
        	bool assert_grant_priv(string priv_name, string name);
        	bool assert_revoke_priv(string priv_name, string name);
        	bool assert_create_user_priv();        	
        	bool assert_remove_user_priv();        	
        	bool assert_bool_query(string query);
        	bool assert_privilige(string priv, string object);
        	bool priviliged_mode;
        	bool system_privilige_checking;
//[sk153407, pk167277 - BEGIN]

		int getProcedureInfo(TreeNode *tree, ProcedureInfo *&pinf);

//[sk153407, pk167277 - END]
        
	public:
		SessionData *session_data;
		QueryExecutor() { 
			envs = new EnvironmentStack(); 
			qres = new ResultStack(); 
			prms = NULL; 
			tr = NULL; 
			inTransaction = false;
			antyStarve = false;
			transactionNumber = 0;
			ec = new ErrorConsole("QueryExecutor"); 
			stop = 0; 
			session_data = new SessionData(); 
			system_privilige_checking = false; 
		};
		~QueryExecutor();
		int executeQuery(TreeNode *tree, map<string, QueryResult*>* params, QueryResult **result);
		int executeQuery(TreeNode *tree, QueryResult **result);
		void stopExecuting() { stop = 65535; };
		int abort();
		void set_priviliged_mode(bool mode);
	};
	
	/*
	 *	Singleton design pattern
	 */
	class QueryBuilder {
	    private:
		static QueryBuilder *builder;
		QueryBuilder();		

	    public:
		static QueryBuilder* getHandle();
		
		~QueryBuilder();
		/* query builder functions */
		string create_user_query(string user, string passwd);
		string remove_user_query(string user);
		string grant_priv_query(string priv, string object, string user, int grant_option = 0);
		string revoke_priv_query(string priv, string object, string user);
		string query_for_privilige(string user, string privilige, string object);
		string query_for_privilige_grant(string user, string privilige, string object);
		string query_for_password(string user, string password);
	};
}

#endif
