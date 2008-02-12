#ifndef __DECISION_TABLE_H__
#define __DECISION_TABLE_H__

#include "ClassNames.h"
#include "Errors/ErrorConsole.h"
#include "QueryExecutor/QueryResult.h"
#include "Store/Store.h"
#include "TransactionManager/Transaction.h"
#include "TransactionManager/Mutex.h"
#include "TypeCheckResult.h"
#include "Rule.h"
#include "../QueryParser/TreeNode.h"
//#include "../QueryParser/IndexNode.h"

#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

using namespace Errors;
using namespace QExecutor;
using namespace QParser;
using namespace TManager;
using namespace std;

//#define INDEX_DEBUG_MODE 

namespace TypeCheck
{

	class DecisionTableHandler{
	private:
		static DecisionTableHandler *dTableHandler;
		DecisionTableHandler(){}
		map<int, UnOpDecisionTable*> unOpDTables;
		map<int, DecisionTable*> algOpDTables;
		map<int, DecisionTable*> nonAlgOpDTables;
	
	public:
		static DecisionTableHandler *getHandle();
		DecisionTable *getDTable(int algOrNonAlg, int op);
		UnOpDecisionTable *getUnOpDTable(int op);
		UnOpDecisionTable *getUnOpDTable(int op, string param, int optParam);
		~DecisionTableHandler();
	};

	class DTable {
	
	public: 
		enum DTableKind {ALG, NONALG, UNOP};
		DTable(){}
		virtual void provideReturnLinks();
		virtual void provideReturnLinksForVector(vector<Rule> v);
		virtual vector<Rule> getRules() {return vector<Rule>();}
		virtual vector<Rule> getCardRules() {return vector<Rule>();}
		virtual vector<Rule> getTypeNameRules() {return vector<Rule>();}
		virtual vector<Rule> getCollKindRules() {return vector<Rule>();}
		/* result generators common for decisionTables, unOpDecisionTables and others ...*/
		
		
		virtual ~DTable(){}
	};

	class DecisionTable : public DTable {
	private:
		vector<TCRule> rules;
		vector<TCRule> cardRules;
		vector<TCRule> typeNameRules;
		vector<TCRule> collKindRules;
		int algOrNonAlg;
		int op;
		//TypeChecker *tcOwner; use this only if function pointers require it to be here...  
	public: 
		enum ResultGenerator {CD_ADDBANDS, CD_MULTBANDS, BS_COPY_L, BS_COPY_R, ARG_COPY_L, ARG_COPY_R, CD_COPY_L_ZR, CK_COPY_L, BS_STRUCT};
		DecisionTable() {/*tcOwner = NULL;*/}
		DecisionTable(int opType, int op);
		virtual void provideReturnLinks();
		virtual void provideReturnLinksForVector(vector<TCRule> &v);
		/** methods being result generators.. */ //or should these be in Rule/TCResult?
		virtual	Signature *leftSig(Signature *lSig, Signature *rSig);
		virtual	Signature *rightSig(Signature *lSig, Signature *rSig);
		virtual	Signature *doStruct(Signature *lSig, Signature *rSig);
			
		virtual	string cdAddBands(string lCard, string rCard);
		virtual	string cdMultiplyBands(string lCard, string rCard);
		virtual	string copyLeftArg(string l, string r);
		virtual	string copyRightArg(string l, string r);
		virtual	string cdCopyLeftWithZero(string lCard, string rCard);
		/** end of result generating methods..*/
		
		virtual Signature *doSig(int method, Signature *lSig, Signature *rSig);
		virtual string doAttr(int method, string lArg, string rArg);
		
		virtual int applyRuleset (vector<TCRule> ruleSet, Signature *lSig, Signature *rSig, string leftArg, string rightArg, TypeCheckResult &finalResult);
		virtual int getResult(TypeCheckResult &finalResult, Signature *lSig, Signature *rSig);
		
		virtual void initAlgOpRules(int op);
		virtual void initNonAlgOpRules(int op);

		void addTcRule (string lArg, string rArg, string result);
		void addTcRule (string specArg, string lArg, string rArg, string result);
		void addTcRule (string specArg, string lArg, string rArg, int resultGen);
		void addTcRule (string specArg, string result);
		void addTcRule (string lArg, string rArg, string result, int action, string dynStat);
		void addTcRule (string specArg, string lArg, string rArg, string result, int action, string dynStat);
		void addTcRule (string specArg, string lArg, string rArg, int resultGen, int action, string dynStat);
		
		void addCdRule (string lArg, string rArg, string result);
		void addCdRule (string specArg, string lArg, string rArg, string result);
		void addCdRule (string specArg, string lArg, string rArg, int resultGen);
		void addCdRule (string specArg, string result);
		void addCdRule (string lArg, string rArg, string result, int action, string dynStat);
		void addCdRule (string specArg, string lArg, string rArg, string result, int action, string dynStat);
		void addCdRule (string specArg, string lArg, string rArg, int resultGen, int action, string dynStat);
		
		void addTnRule (string lArg, string rArg, string result);
		void addTnRule (string specArg, string lArg, string rArg, string result);
		void addTnRule (string specArg, string lArg, string rArg, int resultGen);
		void addTnRule (string specArg, string result);
		
		void addCkRule (string lArg, string rArg, string result);
		void addCkRule (string specArg, string lArg, string rArg, string result);
		void addCkRule (string specArg, string lArg, string rArg, int resultGen);
		void addCkRule (string specArg, string result);

		virtual ~DecisionTable() {}
	};
	
	class UnOpDecisionTable : public DTable {
	private:
		vector<UnOpRule> rules;
		vector<UnOpRule> cardRules;
		vector<UnOpRule> typeNameRules;
		vector<UnOpRule> collKindRules;
		int op;
		string param;	//often - name, e.g. :name as, group as, create... 
		int optParam;	//when treeNode is defined by more params. Thats rare, but eg. nameAs may be grouped or not.
	public:
		enum ResultGenerator {BS_COPY, ARG_COPY, BS_NAMEAS, CD_NAMEAS, CK_NAMEAS, BS_REF_DEREF};
		UnOpDecisionTable(){ }
		UnOpDecisionTable(int oper) {
			op = oper;
			param = "";
			initUnOpRules(op);	
		}
		UnOpDecisionTable(int oper, string _param, int _optParam) {
			param = _param;
			op = oper;	
			optParam = _optParam;
			initUnOpRules((-1) * op);//to differentiate from normal unOps - these will be compared to TreeNode enum types.
		}
		virtual void provideReturnLinks();
		virtual void provideReturnLinksForVector(vector<UnOpRule> &v);
		
		virtual string getParam() {return param;}
		virtual void setParam(string newParam) {param = newParam;}
		virtual TypeCheckResult getResult(int op, Signature arg);
		virtual int getResult(TypeCheckResult &finalResult, Signature *sig);
		virtual int applyRuleset (vector<UnOpRule> ruleSet, Signature *sig, string textArg, TypeCheckResult &finalResult);
		virtual void initUnOpRules(int op);
		
		/** methods being result generators.. */ //or should these be in Rule/TCResult?
		virtual Signature *nameAsSig(Signature *sig);
		virtual Signature *refDerefSig(Signature *sig);
//		Signature *leftSig(Signature *lSig, Signature *rSig);
//		Signature *rightSig(Signature *lSig, Signature *rSig);
//		Signature *doStruct(Signature *lSig, Signature *rSig);
		virtual string cdNameAs(string card);
		virtual string ckNameAs(string card);
//		string cdAddBands(string lCard, string rCard);
//		string cdMultiplyBands(string lCard, string rCard);
//		string copyLeftArg(string l, string r);
//		string copyRightArg(string l, string r);
//		string cdCopyLeftWithZero(string lCard, string rCard);
		/** end of result generating methods..*/
		
		virtual Signature *doSig(int method, Signature *sig);
		virtual string doAttr(int method, string arg);
		
		
//		void addTcRule (string arg, string result);
		void addTcRule (string specArg, string arg, string result);
		void addTcRule (string specArg, string arg, int resultGen);
//		void addTcRule (string arg, string result, int action, string dynStat);
//		void addTcRule (string specArg, string arg, string result, int action, string dynStat);
//		void addTcRule (string specArg, string arg, int resultGen, int action, string dynStat);
		
//		void addCdRule (string arg, string result);
		void addCdRule (string specArg, string arg, string result);
		void addCdRule (string specArg, string arg, int resultGen);
//		void addCdRule (string arg, string result, int action, string dynStat);
//		void addCdRule (string specArg, string arg, string result, int action, string dynStat);
//		void addCdRule (string specArg, string arg, int resultGen, int action, string dynStat);
				
//		void addTnRule (string arg, string result);
		void addTnRule (string specArg, string arg, string result);
		void addTnRule (string specArg, string arg, int resultGen);
		
//		void addCkRule (string arg, string result);
		void addCkRule (string specArg, string arg, string result);
		void addCkRule (string specArg, string arg, int resultGen);
		
		virtual ~UnOpDecisionTable(){};
	};

	
}

#endif /* __DECISION_TABLE_H__ */
