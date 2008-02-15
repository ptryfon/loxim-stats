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



namespace TypeCheck
{
#define SIG_BASE	"base"
#define SIG_CARD	"card"
#define SIG_CK		"collectionKind"
#define SIG_TN		"typeName"
#define SIG_MUT		"mutability"
	
	class DecisionTableHandler{
	private:
		static DecisionTableHandler *dTableHandler;
		DecisionTableHandler(){}
		map<int, DecisionTable*> algOpDTables;
		map<int, DecisionTable*> nonAlgOpDTables;
		map<int, UnOpDecisionTable*> unOpDTables;
		map<int, DecisionTable*> otherBinaryDTables;
		map<int, UnOpDecisionTable*> otherUnaryDTables;
	
	public:
		static DecisionTableHandler *getHandle();
		DecisionTable *getDTable(int algOrNonAlg, int op);
		UnOpDecisionTable *getUnOpDTable(int treeType, int op);
		~DecisionTableHandler();
	};

	class DTable {
	
	public: 
		enum DTableKind {ALG, NONALG, UNOP};
		enum CoerceAction {CARD_TO_11, BS_TO_STR, BS_TO_BOOL, BS_TO_INT, BS_TO_DBL, CK_TO_BAG, CK_TO_SEQ };
		enum CoerceArg { BOTH, LEFT, RIGHT, SINGLE };
		enum CoerceKind {STATIC, DYNAMIC};
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
		enum ResultGenerator {CD_ADDBANDS, CD_MULTBANDS, BS_COPY_L, BS_COPY_R, ARG_COPY_L, ARG_COPY_R, 
			CD_COPY_L_ZR, CK_COPY_L, BS_STRUCT};
		
		//{CD_COERCE_11, CD_COERCE_11_L, CD_COERCE_11_R, CD_COERCE_11_B, BS_TOSTR, BS_TOSTR_L, BS_TOSTR_R, BS_TOSTR_B, BS_TOBOOL, BS_TOBOOL_L, BS_TOBOOL_R, BS_TOBOOL_B, BS_TODBL, BS_TODBL_L, BS_TODBL_R, BS_TODBL_B, BS_TOINT, BS_TOINT_L, BS_TOINT_R, BS_TOINT_B};
			//{to_string, to_double, to_bool, to_int, element, to_bag, to_seq};
		
		DecisionTable() {}
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
		
		virtual int applyRuleset (vector<TCRule> ruleSet, string atr, Signature *lSig, Signature *rSig, string leftArg, string rightArg, TypeCheckResult &finalResult);
		virtual int getResult(TypeCheckResult &finalResult, Signature *lSig, Signature *rSig);
		
		virtual void initAlgOpRules(int op);
		virtual void initNonAlgOpRules(int op);
		//in case new operators are added...
		virtual void initOtherBinaryRules(int treeType, int op) {};
		
		void addTcRule (string lArg, string rArg, string result);
		void addTcRule (string specArg, string lArg, string rArg, string result);
		void addTcRule (string specArg, string lArg, string rArg, int resultGen);
		void addTcRule (string specArg, string result);
		void addTcRule (string lArg, string rArg, string result, int act, int actArg, int dynStat);
		void addTcRule (string specArg, string lArg, string rArg, string result, int act, int actArg, int dynStat);
		void addTcRule (string specArg, string lArg, string rArg, int resultGen, int act, int actArg, int dynStat);
		
		void addCdRule (string lArg, string rArg, string result);
		void addCdRule (string specArg, string lArg, string rArg, string result);
		void addCdRule (string specArg, string lArg, string rArg, int resultGen);
		void addCdRule (string specArg, string result);
		void addCdRule (string lArg, string rArg, string result, int act, int actArg, int dynStat);
		void addCdRule (string specArg, string lArg, string rArg, string result, int act, int actArg, int dynStat);
		void addCdRule (string specArg, string lArg, string rArg, int resultGen, int act, int actArg, int dynStat);
		
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
	public:
		enum ResultGenerator {BS_COPY, ARG_COPY, BS_NAMEAS, CD_NAMEAS, CK_NAMEAS, BS_REF_DEREF};
		UnOpDecisionTable(){ }
		UnOpDecisionTable(int treeType, int oper) : op(oper) { initUnOpRules(treeType, op); }

		virtual void provideReturnLinks();
		virtual void provideReturnLinksForVector(vector<UnOpRule> &v);
		
		virtual int getResult(TypeCheckResult &finalResult, Signature *sig);
		// getResult for complex treeNodes (eg. name as / group as)...
		virtual int getResult(TypeCheckResult &finalResult, Signature *sig, string param, int option); 
		virtual int applyRuleset (vector<UnOpRule> ruleSet, string atr, Signature *sig, string textArg, TypeCheckResult &finalResult, string param, int option);
		virtual void initUnOpRules(int treeType, int op);
		
		/** methods being result generators.. */ //or should these be in Rule/TCResult?
		virtual Signature *nameAsSig(Signature *sig, string name, int grouped);
		virtual Signature *refDerefSig(Signature *sig);
		virtual Signature *copySig(Signature *sig);
//		Signature *leftSig(Signature *lSig, Signature *rSig);
//		Signature *rightSig(Signature *lSig, Signature *rSig);
//		Signature *doStruct(Signature *lSig, Signature *rSig);
		virtual string cdNameAs(string card, int grouped);
		virtual string ckNameAs(string card, int grouped);
//		string cdAddBands(string lCard, string rCard);
//		string cdMultiplyBands(string lCard, string rCard);
//		string copyLeftArg(string l, string r);
//		string copyRightArg(string l, string r);
//		string cdCopyLeftWithZero(string lCard, string rCard);
		/** end of result generating methods..*/
		
		virtual Signature *doSig(int method, Signature *sig, string param, int option);
		virtual string doAttr(int method, string arg, string param, int option);
		
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
