#ifndef __TC_RULE_H__
#define __TC_RULE_H__

#include <TypeCheck/ClassNames.h>
#include <Errors/ErrorConsole.h>
#include <QueryExecutor/QueryResult.h>
#include <Store/Store.h>
#include <TransactionManager/Transaction.h>
#include <TypeCheck/TypeCheckResult.h>
#include <TypeCheck/DecisionTable.h>
#include <QueryParser/Deb.h>
#include <QueryParser/Stack.h>
#include <Util/Concurrency.h>

#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>


using namespace Errors;
using namespace QExecutor;
using namespace QParser;
using namespace TManager;
using namespace Util;
using namespace std;



namespace TypeCheck
{
	
	class Rule {	//all rule instances are members of a subclass..?
	protected:
		string specialArg;		//could add another attr: bool specialCondition, which - if present - 
								//would have higher priority than args & specialArg.
		string result;
		int sigGen;
		int attrGen;
		int action;
		bool dynCtrl;
		int ruleType;	// "BASE", "CARD", "TYPENAME", "COLLKIND", ... see public enumerator
		
	public:
		enum RuleType { 
			BASE,
			CARD,
			TYPENAME,
			COLLKIND };

		Rule() { ruleType = Rule::BASE;}
		Rule(int rType) {ruleType = rType;}
		virtual void setDTable(DTable */*dt*/) {};
		//both appliesTo() methods to be implemented in subclasses..
		virtual bool appliesTo(string /*unArg*/) {return false;}
		virtual bool appliesTo(string /*lArg*/, string /*rArg*/) {return false;}
		virtual int getRuleType() {return ruleType;}
		virtual void setAction(int actId, string dynStr) {action = actId; dynCtrl = (dynStr == "DYN");}
		virtual bool isEmpty (string s) {return (s == "");}//TODO: what about NULLs...?
		virtual bool needsResultGenerator() {return ((ruleType == Rule::BASE && sigGen >= 0) || attrGen >= 0);}
		virtual bool needsAction() {return (action >= 0);}
		virtual ~Rule() {}
	};
	
	//type check 2-arg. rule... 
	class TCRule : public Rule {
	private: 
		DecisionTable *dTable;
		string leftArg;
		string rightArg;
		int actionArg;
		
	public:
		TCRule();
		// constructors used for simple rules 
		TCRule(int rType, string lArg, string rArg, string res);
		// constructors for more complicated rules		
		TCRule (int rType, string specArg, string lArg, string rArg, string resStr, int resGen, int actId, int actArg, int crcKind);
		TCRule (int rType, string specArg, string lArg, string rArg, string resStr, int resGen);
		virtual void setDTable(DTable *dt) {dTable = (DecisionTable *)dt;};
		//appliesTo(string arg); - makes no sense here, so returns false as in Rule.
		virtual bool appliesTo(string lArg, string rArg);
		virtual bool appliesToBase(Signature *lSig, Signature *rSig); //for structural equality..
		virtual int getResult(string atr, Signature *lSig, Signature *rSig, TypeCheckResult &retResult);		
		virtual int getSimpleResult(Signature *lSig, Signature *rSig, TypeCheckResult &retResult);
		virtual int getGeneratedResult(Signature *lSig, Signature *rSig, TypeCheckResult &retResult);
		virtual void coutMe();
		virtual ~TCRule(){};
	};

	class UnOpRule : public Rule {
	private:
		UnOpDecisionTable *dTable;
		string arg;

	public:
		UnOpRule (string _arg, string res) {
			arg = _arg;
			result = res;
		}
		UnOpRule(int rType, string arg, string res);
		// constructors for more complicated rules		
		UnOpRule (int rType, string specArg, string arg, string resStr, int resGen, int actId, int dStr);

		bool appliesTo(string lArg, string /*rArg*/) {
			Deb::ug("(!!!) Two-arg UnOpRule::appliesTo() used only for first param.");
			return appliesTo(lArg);
		}
		
		bool appliesTo(string arg);
		
		virtual void setDTable(DTable *dt) {dTable = (UnOpDecisionTable *)dt;};
		virtual int getResult(string atr, Signature *sig, TypeCheckResult &retResult, string param, int option);
		virtual int getSimpleResult(Signature *sig, TypeCheckResult &retResult); 
		virtual int getGeneratedResult(Signature *sig, TypeCheckResult &retResult, string param, int option);
		virtual void coutMe() {cout << "unoprule, (" << arg << ")->" << result << "." << endl;}
		virtual ~UnOpRule() {}
	};

}

#endif /* __TC_RULE_H__ */
