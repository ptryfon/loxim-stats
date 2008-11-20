#ifndef __TC_RESTORE_ALG_H__
#define __TC_RESTORE_ALG_H__

#include <TypeCheck/ClassNames.h>
#include <TypeCheck/TypeChecker.h>
#include <TypeCheck/DecisionTable.h>
#include <TypeCheck/TCConstants.h>
#include <Errors/Errors.h>
#include <QueryParser/TreeNode.h>
#include <QueryParser/Stack.h>

#include <sstream>
#include <string>

using namespace QParser;
using namespace Errors;

namespace TypeCheck {
	
	
	//Abstract interface for restore algorithms. Add a subclass to define new algorithms of restoring TC process.
	class RestoreAlgorithm 
	{
	public:	
		virtual int restoreOnBadArg(TreeNode *tn, int tType, int op, TypeCheckResult &tc, Signature *l, Signature *r, RestoreParam *rp) = 0;
		virtual int restoreOnBadArg(TreeNode *tn, int tType, int op, TypeCheckResult &tc, Signature *sig, RestoreParam *rp) = 0;
		virtual int restoreOnMisspelledName(TreeNode *tn, string name, vector<BinderWrap*> *vec) = 0;
		virtual ~RestoreAlgorithm(){};
	};
	
	class RestoreParam {
	public:
		virtual string toString() = 0;
		virtual ~RestoreParam(){}
	};
	
	/* *****************	Specific implementation - with own restore DecisionTable handler	**************** */
	
	//Algorithms based on TypeChecker's structures (DecisionTables and stacks)
	class DTCRestoreAlgorithm : public RestoreAlgorithm
	{
	private:
		StatQResStack *sQres;		//	static query result stack - of parent typechecker
		StatEnvStack *sEnvs;		//	static environment stack = of parent typechecker
		
	public:	
		DTCRestoreAlgorithm();
		DTCRestoreAlgorithm(StatQResStack *sq, StatEnvStack *se);
		virtual int restoreOnBadArg(TreeNode *tn, int tType, int op, TypeCheckResult &tc, Signature *l, Signature *r, RestoreParam *rp);
		virtual int restoreOnBadArg(TreeNode *tn, int tType, int op, TypeCheckResult &tc, Signature *sig, RestoreParam *rp);
		virtual int restoreOnMisspelledName(TreeNode *tn, string name, vector<BinderWrap*> *vec);
		virtual ~DTCRestoreAlgorithm(){}
	};
	
		//DecisionTable & UnOpDecisionTable, with different rules - just override the init methods
	class RDecisionTable : public DecisionTable {
		public: 
			RDecisionTable(int treeType, int oper);
			virtual void initAlgOpRules(int op);
			virtual void initNonAlgOpRules(int op);
		//in case new operators are added...
			virtual void initOtherBinaryRules(int treeType, int op);
			virtual ~RDecisionTable(){}
	};

	class RUnOpDecisionTable : public UnOpDecisionTable {
		public:
			RUnOpDecisionTable(int treeType, int oper);
			virtual void initUnOpRules(int treeType, int op);
			virtual ~RUnOpDecisionTable(){}
	};
	
	class DTCRestoreParam : public RestoreParam
	{
	public:
		string param;
		int option;
		//May need additional parameters.
		DTCRestoreParam(string p, int o) : param(p), option(0) {}
		virtual string toString(); 
		virtual ~DTCRestoreParam(){}
	};
	
	//DT handler similar to normal decisionTableHandler.
	class RestoreDecisionTableHandler {
		private:
			static RestoreDecisionTableHandler *rdTableHandler;
			RestoreDecisionTableHandler(){}
			map<int, RDecisionTable*> algOpRestoreDTables;
			map<int, RDecisionTable*> nonAlgOpRestoreDTables;
			map<int, RDecisionTable*> otherBinaryRestoreDTables;
			map<int, RUnOpDecisionTable*> unOpRestoreDTables;
			map<int, RUnOpDecisionTable*> otherUnaryRestoreDTables;
	
		public:
			static RestoreDecisionTableHandler *getHandle();
			RDecisionTable *getDTable(int algOrNonAlg, int op);
			RUnOpDecisionTable *getUnOpDTable(int treeType, int op);
			~RestoreDecisionTableHandler();
	};
	
	/* ******************************************************************************************************** */
	
	//An example of replacing the standard restore algorithm with one that handles bad names in a different manner.
// 	class AlternativeNameRestoreAlgorithm : public RestoreAlgorithm
// 	{
// 	private :
// 		StatEnvStack *sEnvs;
// 	public :
// 		AlternativeNameRestoreAlgorithm(StatEnvStack *sEnvStack) : sEnvs(sEnvStack) {};
// 		virtual int restoreOnMisspelledName(TreeNode *tn, string name, vector<BinderWrap*> *vec);
// 		virtual ~AlternativeNameRestoreAlgorithm(){};
// 	};

}



#endif /* __TC_RESTORE_ALG_H__ */



