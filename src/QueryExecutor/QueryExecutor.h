#ifndef _QUERYEXECUTOR_H
#define _QUERYEXECUTOR_H

#include <stdio.h>
#include <string>
#include <vector>
#include <map>

#include <QueryExecutor/ClassGraph.h>
#include <QueryExecutor/QueryResult.h>
#include <QueryExecutor/EnvironmentStack.h>
#include <TransactionManager/Transaction.h>
#include <Store/Store.h>
#include <Store/DBDataValue.h>
#include <Store/DBLogicalID.h>
#include <QueryParser/Privilige.h>
#include <QueryParser/TreeNode.h>
#include <QueryParser/DataRead.h>
#include <Errors/Errors.h>
#include <Errors/ErrorConsole.h>
#include <TypeCheck/TCConstants.h>
#include <TypeCheck/TypeChecker.h>

namespace Server{
	class Session;
}

namespace QParser {
	class QueryParser;
}

namespace Schemas { class Schema; class InterfaceMaps; class OuterSchemas; }

using namespace QParser;
using namespace TManager;
using namespace Errors;
using namespace Store;
using namespace std;

namespace QExecutor
{

#define QE_SELF_KEYWORD				"self"
#define QE_SUPER_KEYWORD			"super"
#define QE_VIRTUALS_TO_SEND_MIN_ID		0xFE000000
#define QE_VIRTUALS_TO_SEND_MAX_COUNT		0x01000000
#define QE_VIEWDEF_VIRTUALOBJECTS_NAME		"VirtualObjects"
#define QE_NOTROOT_VIEW_PARENT_NAME		"ViewParent"

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

//     class ProcedureInfo
//     {
//     public:
//         string ProcCode;
//         vector<string> Params;
//         virtual ~ProcedureInfo(){};
//     };
	
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
    
		ErrorConsole *ec;
		Transaction *tr;
		ClassGraph *cg;
		Schemas::InterfaceMaps *im;
		Schemas::OuterSchemas *os;
		EnvironmentStack *envs;
		ResultStack *qres;
		map<string, QueryResult*> *prms;
		vector<QueryResult*> sent_virtuals;
		QVirtualsToFakeLidMap fakeLid_map;
		int stop;
		int transactionNumber;
		bool antyStarve;
		
		bool evalStopped() { return ( stop != 0 ); };
		int executeRecQuery(TreeNode *tree, bool checkPrivilieges = true);
		int combine(NonAlgOpNode::nonAlgOp op, QueryResult *curr, QueryResult *lRes, QueryResult *&partial);
		int merge(NonAlgOpNode::nonAlgOp op, QueryResult *partial, QueryResult *&final);
		int unOperate(UnOpNode::unOp op, QueryResult *arg, QueryResult *&final, bool checkPrivilieges);
		int coerceOperate(int cType, QueryResult *arg, QueryResult *&final, TreeNode *tree);
		int algOperate(AlgOpNode::algOp op, QueryResult *lArg, QueryResult *rArg, QueryResult *&final, AlgOpNode *tn, bool checkPrivilieges);
		int derefQuery(QueryResult *arg, QueryResult *&res);
		int refQuery(QueryResult *arg, QueryResult *&res);
        int nameofQuery(QueryResult *arg, QueryResult *&res);
		int isIncluded(QueryResult *elem, QueryResult *set, bool &score);
		int sortBag(QueryBagResult *inBag, QueryBagResult *&outBag);
		int objectFromBinder(QueryResult *res, ObjectPointer *&newObject);
		
		int nameTaken(string name, bool &taken);
		int implementationNameTaken(string impName, bool &taken);
		int classExists(string className, bool& exist);
		int viewExists(string viewName, bool& exists);
		int interfaceNameTaken(string name, bool& taken);
		int getInterfaceLID(string name, LogicalID*& lid);
		
		/** Create new object in store and puts reference on stack result */		
		int createObjectAndPutOnQRes(DBDataValue* dbValue, string objectName, Store::ExtendedType type, LogicalID*& newLid);

		int createOnView(string objectName, bool &found, QueryResult *binder, QueryResult *&result);
		
		int lidFromVector( string bindName, vector<QueryResult*> value, LogicalID*& lid);
		
		int lidFromString( string bindName, string value, LogicalID*& lid);
		
		int lidFromReference( string bindName, LogicalID* value, LogicalID*& lid);

		int lidFromInt(string bindName, int value, LogicalID*& lid);
		
		int lidFromBinder( string bindName, QueryResult* result, LogicalID*& lid);
		
		int classesLIDsFromNames(set<string>* names, vector<LogicalID*>& lids);
		
		void pushStringToLIDs(string name, string bindName, vector<LogicalID*>& lids);
		
		void pushStringsToLIDs(set<string>* names, string bindName, vector<LogicalID*>& lids);
		
		int execRecursivQueryAndPop(TreeNode *query, QueryResult*& execResult);
		
		int persistDelete(QueryResult* bagArg, bool checkPrivilieges = false);
		
		int persistDelete(LogicalID *lid, bool checkPrivilieges = false);
		
		int persistDelete(vector<LogicalID*>* lids);
		
		int removePersistFromSubclasses(ObjectPointer *optr);
		
		int removePersistFromSuperclasses(ObjectPointer *optr);
		
		int persistStaticMembersDelete(const string& object_name);
		
		
		int deVirtualize(QueryResult *arg, QueryResult *&res);
		int reVirtualize(QueryResult *arg, QueryResult *&res);
        	
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
		/** typedefs   - type definitions / declarations */
	
		int executeObjectDeclarationOrTypeDefinition(DeclareDefineNode *obdNode, bool isTypedef);
		//int executeObjectDeclaration(ObjectDeclareNode *tree);
		//int executeTypeDefinition(TypeDefNode *tree);
		int objDeclRec(DeclareDefineNode *obd, string rootName, bool typeDef, string ownerName, vector<string> *queries, bool topLevel);
		/** end of typedefs...  */
		int checkUpInRootCardMap(string optrName, map<string, int> &delRootMap);
		int checkUpInDelSubMap(LogicalID *ptLid, string name, map<std::pair<unsigned int, string>, int> &delRootMap);
		int checkDelCard(QueryResult *arg, QueryResult *&final);
		//int checkCrtCard(QueryResult *arg, QueryResult *&final, TreeNode *tree); - removed, moved to mark_node check.
		int checkSubCardsRec(Signature *sig, ObjectPointer *optr);
		int checkSubCardsRec(Signature *sig, QueryResult *res, bool isBinder, int &obtsSize);
		int checkSingleSubCard(SigColl *sigc, QueryResult *single, map<string, int> &subMap);
			
	public:
        bool is_dba();
		bool inTransaction;
		//SessionData *session_data;
		Server::Session *session;
		QueryExecutor(Server::Session *session);
		~QueryExecutor();
		int initCg();
		ClassGraph* getCg();
		const Schemas::InterfaceMaps* getIm() {return im; }
		int executeQuery(TreeNode *tree, map<string, QueryResult*>* params, QueryResult **result);
		int executeQuery(TreeNode *tree, QueryResult **result);
		void stopExecuting() { stop = 65535; };
		void contExecuting() { stop = 0; };
		void antyStarveFunction(int errcode);
		int abort();
		void set_priviliged_mode(bool mode);
		int procCheck(unsigned int qSize, LogicalID *lid, string &code, vector<string> &prms, int &founded);
		int callProcedure(string code, vector<QueryBagResult*> sections);
		int checkViewAndGetVirtuals(LogicalID *lid, string &name, string &code);
		int getSubviews(LogicalID *lid, string vo_name, vector<LogicalID *> &subviews);
		int getSubview(LogicalID *lid, string name, LogicalID *&subview_lid);
		int getOn_procedure(LogicalID *lid, string procName, string &code, string &param);
		int createNewSections(QueryVirtualResult *qvirt, QueryBinderResult *param, LogicalID *viewdef, vector<QueryBagResult*> &sections);
		int pop_qres(QueryResult *&r) { return qres->pop(r); };
		
		EnvironmentStack* getEnvs() { return envs; }
		
		int trErrorOccur( string msg, int errcode );
		
		int qeErrorOccur( string msg, int errcode );
		
		int otherErrorOccur( string msg, int errcode );
		
		//friend class Schemas::Schema;
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
		static void startup();
		static void shutdown();
		~QueryBuilder();
		/* query builder functions */
		string create_user_query(string user, string passwd);
		string remove_user_query(string user);
		string grant_priv_query(string priv, string object, string user, int grant_option = 0);
		string revoke_priv_query(string priv, string object, string user);
		string query_for_privilige(string user, string privilige, string object);
		string query_for_privilige_grant(string user, string privilige, string object);
		string query_for_password(string user, string password);
		
		string query_create_obj_mdn(string name, string card, string interior, string mainStr);		
		string query_insert_target(int base, string baseName, bool isTgtTypedef, string tgtName);
		string query_insert_owner(string name, string ownerName, int ownerBase);
		string query_insert_subobj(int ownerBase, string ownerName, string subName);
		string query_create_typedef(string name, bool distinct, string interior);
		string baseStr(int baseCd);
	};
}

#endif
