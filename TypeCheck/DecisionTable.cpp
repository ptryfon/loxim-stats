#include "DecisionTable.h"

#include "Errors/ErrorConsole.h"
#include "QueryExecutor/QueryResult.h"
#include "QueryParser/Stack.h"
#include "TransactionManager/Transaction.h"
#include "TransactionManager/Mutex.h"

#include "Rule.h"
#include "TypeChecker.h"
#include "TypeCheckResult.h"

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>

using namespace Errors;
using namespace QParser;
using namespace std;

//#define INDEX_DEBUG_MODE 

namespace TypeCheck
{
/*
		vector<TcRule> rules;
		vector<CardRule> cardRules;
		vector<TypeNameRule> typeNameRules;
		vector<CollKindRule> collKindRules;
*/
	string DecisionTable::cdAddBands(string lCard, string rCard) {
		stringstream conv;
		//in the code below zeros are subtracted in order to cast (convert) chars to integers.
		if (((lCard[0]-'0')+ (rCard[0]-'0')) > 1) conv << "1";
		else conv << ((lCard[0]-'0') + (rCard[0]-'0'));
		conv << "..";
		if (lCard[3] == '*' || rCard[3] == '*') conv << "*";
		else {
			int rb = ((lCard[3]-'0') + (rCard[3]-'0'));
			if (rb > 1) conv << "*";
			else conv << rb;
		}
		return conv.str();
	}
		
	//Takes left card, replaces left bound with zero, and copies right bound...
	string DecisionTable::cdCopyLeftWithZero(string lCard, string rCard) {
		stringstream conv;
		conv << "0.." << lCard[3];
		return conv.str();
	}	
	
	string DecisionTable::cdMultiplyBands(string lCard, string rCard) {
		stringstream conv;
		if (lCard[0] == '0' || rCard[0] == '0') conv << "0"; 
		else if (lCard[0] == '*' || rCard[0] == '*') conv << "*";
		else conv << ((lCard[0] - '0') * (rCard[0] - '0'));
		conv << "..";
		if (lCard[3] == '0' || rCard[3] == '0') conv << "0";
		else if (lCard[3] == '*' || rCard[3] == '*') conv << "*";
		else conv << ((lCard[3] - '0') * (rCard[3] - '0'));
		return conv.str();
	}
	
	
	/** ************************************************************************ */
	
	void DecisionTable::addTcRule (string lArg, string rArg, string result) {
		this->rules.push_back(TCRule(Rule::BASE, lArg, rArg, result));
	}
	void DecisionTable::addTcRule (string specArg, string lArg, string rArg, string result) {
		this->rules.push_back(TCRule(Rule::BASE, specArg, lArg, rArg, result, -1, -1, "STAT"));
	}
	void DecisionTable::addTcRule (string specArg, string lArg, string rArg, int resultGen) {
		this->rules.push_back(TCRule(Rule::BASE, specArg, lArg, rArg, "", resultGen, -1, "STAT"));
	}
	void DecisionTable::addTcRule (string specArg, string result) {
		this->rules.push_back(TCRule(Rule::BASE, specArg, "", "", result, -1, -1, "STAT"));
	}
	void DecisionTable::addTcRule (string lArg, string rArg, string result, int action, string dynStat) {
		this->rules.push_back(TCRule(Rule::BASE, "ACCURATE", lArg, rArg, result, -1, action, dynStat));
	}
	void DecisionTable::addTcRule (string specArg, string lArg, string rArg, string result, 
							int action, string dynStat) {
		this->rules.push_back(TCRule(Rule::BASE, specArg, lArg, rArg, result, -1, action, dynStat));
	}
	void DecisionTable::addTcRule (string specArg, string lArg, string rArg, int resultGen, 
							int action, string dynStat) {
		this->rules.push_back(TCRule(Rule::BASE, specArg, lArg, rArg, "", resultGen, action, dynStat));
	}
	/* ---------------------  CARD rules --------------------- */	
	void DecisionTable::addCdRule (string lArg, string rArg, string result) {
		this->cardRules.push_back(TCRule(Rule::CARD, lArg, rArg, result));
	}
	void DecisionTable::addCdRule (string specArg, string lArg, string rArg, string result) {
		this->cardRules.push_back(TCRule(Rule::CARD, specArg, lArg, rArg, result, -1, -1, "STAT"));
	}
	void DecisionTable::addCdRule (string specArg, string lArg, string rArg, int resultGen) {
		this->cardRules.push_back(TCRule(Rule::CARD, specArg, lArg, rArg, "", resultGen, -1, "STAT"));
	}
	void DecisionTable::addCdRule (string specArg, string result){
		this->cardRules.push_back(TCRule(Rule::CARD, specArg, "", "", result, -1, -1, "STAT"));
	}
	void DecisionTable::addCdRule (string lArg, string rArg, string result, int action, string dynStat) {
		this->cardRules.push_back(TCRule(Rule::CARD, "ACCURATE", lArg, rArg, result, -1, action, dynStat));
	}
	void DecisionTable::addCdRule (string specArg, string lArg, string rArg, string result, 
							int action, string dynStat) {
		this->cardRules.push_back(TCRule(Rule::CARD, specArg, lArg, rArg, result, -1, action, dynStat));
	}
	void DecisionTable::addCdRule (string specArg, string lArg, string rArg, int resultGen, 
							int action, string dynStat) {
		this->cardRules.push_back(TCRule(Rule::CARD, specArg, lArg, rArg, "", resultGen, action, dynStat));
	}
	/* ---------------------  TYPENAME rules --------------------- */	
	void DecisionTable::addTnRule (string lArg, string rArg, string result) {
		this->typeNameRules.push_back(TCRule(Rule::TYPENAME, lArg, rArg, result));
	}
	void DecisionTable::addTnRule (string specArg, string lArg, string rArg, string result) {
		this->typeNameRules.push_back(TCRule(Rule::TYPENAME, specArg, lArg, rArg, result, -1, -1, "STAT"));
	}
	void DecisionTable::addTnRule (string specArg, string lArg, string rArg, int resultGen) {
		this->typeNameRules.push_back(TCRule(Rule::TYPENAME, specArg, lArg, rArg, "", resultGen, -1, "STAT"));
	}
	void DecisionTable::addTnRule (string specArg, string result) {
		this->typeNameRules.push_back(TCRule(Rule::TYPENAME, specArg, "", "", result, -1, -1, "STAT"));
	}
	/* ---------------------  COLLECTION KIND  rules --------------------- */	
	void DecisionTable::addCkRule (string lArg, string rArg, string result) {
		this->collKindRules.push_back(TCRule(Rule::COLLKIND, lArg, rArg, result));
	}
	void DecisionTable::addCkRule (string specArg, string lArg, string rArg, string result) {
		this->collKindRules.push_back(TCRule(Rule::COLLKIND, specArg, lArg, rArg, result, -1, -1, "STAT"));
	}
	void DecisionTable::addCkRule (string specArg, string lArg, string rArg, int resultGen) {
		this->collKindRules.push_back(TCRule(Rule::COLLKIND, specArg, lArg, rArg, "", resultGen, -1, "STAT"));
	}
	void DecisionTable::addCkRule (string specArg, string result) {
		this->collKindRules.push_back(TCRule(Rule::COLLKIND, specArg, "", "", result, -1, -1, "STAT"));
	}
	

	
//		void addTcRule (string arg, string result);
	void UnOpDecisionTable::addTcRule (string specArg, string arg, string result) {
		this->rules.push_back(UnOpRule(Rule::BASE, specArg, arg, result, -1, -1, "STAT"));
	}
	void UnOpDecisionTable::addTcRule (string specArg, string arg, int resultGen) {
		this->rules.push_back(UnOpRule(Rule::BASE, specArg, arg, "", resultGen, -1, "STAT"));
	}
//		void addTcRule (string arg, string result, int action, string dynStat);
//		void addTcRule (string specArg, string arg, string result, int action, string dynStat);
//		void addTcRule (string specArg, string arg, int resultGen, int action, string dynStat);
//		void addCdRule (string arg, string result);
	void UnOpDecisionTable::addCdRule (string specArg, string arg, string result) {
		this->cardRules.push_back(UnOpRule(Rule::CARD, specArg, arg, result, -1, -1, "STAT"));
	}
	void UnOpDecisionTable::addCdRule (string specArg, string arg, int resultGen) {
		this->cardRules.push_back(UnOpRule(Rule::CARD, specArg, arg, "", resultGen, -1, "STAT"));
	}
//		void addCdRule (string arg, string result, int action, string dynStat);
//		void addCdRule (string specArg, string arg, string result, int action, string dynStat);
//		void addCdRule (string specArg, string arg, int resultGen, int action, string dynStat);
				
//		void addTnRule (string arg, string result);
	void UnOpDecisionTable::addTnRule (string specArg, string arg, string result) {
		this->typeNameRules.push_back(UnOpRule(Rule::TYPENAME, specArg, arg, result, -1, -1, "STAT"));
	}
	void UnOpDecisionTable::addTnRule (string specArg, string arg, int resultGen) {
		this->typeNameRules.push_back(UnOpRule(Rule::TYPENAME, specArg, arg, "", resultGen, -1, "STAT"));
	}
		
//		void addCkRule (string arg, string result);
	void UnOpDecisionTable::addCkRule (string specArg, string arg, string result) {
		this->collKindRules.push_back(UnOpRule(Rule::COLLKIND, specArg, arg, result, -1, -1, "STAT"));
	}
	void UnOpDecisionTable::addCkRule (string specArg, string arg, int resultGen) {
		this->collKindRules.push_back(UnOpRule(Rule::COLLKIND, specArg, arg, "", resultGen, -1, "STAT"));
	}
	
	/** Specific rules. This part may be replaced with reading the data from a flat file..*/
	void DecisionTable::initAlgOpRules(int op) {
		cout << "[TC]: " << "initint AlgOpRules: " << op << " operator." << endl;
		switch (op) {
			case AlgOpNode::bagIntersect: 
			case AlgOpNode::bagMinus: 
			case AlgOpNode::bagUnion: { //PL: suma mnogosciowa..?
/*BASE*/		addTcRule("M_SAME",	"X",	"X",	DecisionTable::BS_COPY_L);
				addTcRule("ELSE",	"ERROR");
/*CARD*/		addCdRule("M_BOTH",	"X",	"Y",	DecisionTable::CD_ADDBANDS);
/*T-NAME*/		addTnRule("M_BOTH",	"NONE", "NONE", "");
				addTnRule("M_SAME",	"X",	"X",	"");
				addTnRule("ELSE",	"ERROR");
/*C-KIND*/		addCkRule("M_BOTH",	"NONE",	"NONE", "bag");
				addCkRule("M_LEFT",	"NONE",	"bag",	"bag");
				addCkRule("M_RIGHT",	"bag",	"NONE",	"bag");
				addCkRule("bag", "bag",	"bag");
				addCkRule("ELSE","ERROR");
				break;
			}
			case AlgOpNode::plus: { 
/*BASE*/		addTcRule("integer", "integer", "integer");
				addTcRule("double", "double", "double");
				addTcRule("string", "string", "string");
				addTcRule("boolean", "boolean", "boolean"); //works like...  OR ? 
				addTcRule("integer", "double", "double");
				addTcRule("double", "integer", "double");
				cout << "[TC]: " << "Added basic tc rules" << endl;
				addTcRule("integer", "string", "string", TypeChecker::BS_TOSTR_L, "STAT");
				addTcRule("string", "integer", "string", TypeChecker::BS_TOSTR_R, "STAT");
				cout << "[TC]: " << "Added tcrules with tcactions... " << endl;
				addTcRule("ELSE", "", "", "ERROR");
/*CARD*/		addCdRule("1..1", "1..1", "1..1");
				addCdRule("M_BOTH", "", "", "1..1", TypeChecker::CD_COERCE_11, "DYN");//DYN_CTRL!
				addCdRule("ELSE", "", "", "ERROR");
/*T-NAME*/		addTnRule("M_BOTH", "NONE", "NONE", "");
				addTnRule("ELSE", "", "", "ERROR");
				break;
			}
			case AlgOpNode::minus: {
/*BASE*/		addTcRule("integer", "integer", "integer");
				addTcRule("double", "double", "double");
				addTcRule("integer", "double", "double");
				addTcRule("double", "integer", "double");
				addTcRule("ELSE", "", "", "ERROR");
/*CARD*/		addCdRule("1..1", "1..1", "1..1");
				addCdRule("M_BOTH", "", "", "1..1", TypeChecker::CD_COERCE_11, "DYN"); //DYN
/*T-NAME*/		addTnRule("M_BOTH", "NONE", "NONE", "");
				addTnRule("ELSE", "", "", "ERROR");
				break;
			}
			case AlgOpNode::times: {
/*BASE*/		addTcRule("integer", "integer", "integer");
				addTcRule("double", "double", "double");
				addTcRule("integer", "double", "double");
				addTcRule("double", "integer", "double");
				addTcRule("boolean", "boolean", "boolean"); //works like...  AND ? 
				addTcRule("ELSE", "", "", "ERROR");
/*CARD*/		addCdRule("1..1", "1..1", "1..1");
				addCdRule("M_BOTH", "", "", "1..1", TypeChecker::CD_COERCE_11, "DYN"); //DYN
/*T-NAME*/		addTnRule("M_BOTH", "NONE", "NONE", "");
				addTnRule("ELSE", "", "", "ERROR");
				break;
			}
			case AlgOpNode::divide: { 
/*BASE*/		addTcRule("integer", "integer", "integer");
				addTcRule("double", "double", "double");
				addTcRule("integer", "double", "double");
				addTcRule("double", "integer", "double");
				addTcRule("ELSE", "", "", "ERROR");
/*CARD*/		addCdRule("1..1", "1..1", "1..1");
				addCdRule("M_BOTH", "", "", "1..1", TypeChecker::CD_COERCE_11, "DYN"); //DYN
/*T-NAME*/		addTnRule("M", "NONE", "NONE", "");
				addTnRule("ELSE", "", "", "ERROR");
				break;
			}
			case AlgOpNode::eq: {
/*BASE*/		addTcRule("integer", "integer", "boolean");
				addTcRule("string", "string", "boolean");
				//addTcRule("double", "double", "boolean");	// comparing two doubles...! are yousure?
				addTcRule("string", "integer", "boolean", TypeChecker::BS_TOSTR_R, "STAT");
				addTcRule("integer", "string", "boolean", TypeChecker::BS_TOSTR_L, "STAT");
				addTcRule("ELSE", "", "", "ERROR");
/*CARD*/		addCdRule("1..1", "1..1", "1..1");
				addCdRule("M_BOTH", "", "", "1..1", TypeChecker::CD_COERCE_11, "DYN"); //DYN
/*T-NAME*/		addTnRule("M_BOTH", "NONE", "NONE", "");
				addTnRule("M_SAME", "", "", "");
				addTnRule("ELSE", "", "", "ERROR");
				break;
			}
			case AlgOpNode::neq: { 
/*BASE*/		addTcRule("integer", "integer", "boolean");
				addTcRule("string", "string", "boolean");
				//addTcRule("double", "double", "boolean");	// comparing two doubles...!?
				addTcRule("string", "integer", "boolean", TypeChecker::BS_TOSTR_R, "STAT");
				addTcRule("integer", "string", "boolean", TypeChecker::BS_TOSTR_L, "STAT");
				addTcRule("ELSE", "", "", "ERROR");
/*CARD*/		addCdRule("1..1", "1..1", "1..1");
				addCdRule("M_BOTH", "", "", "1..1", TypeChecker::CD_COERCE_11, "DYN");
/*T-NAME*/		addTnRule("M_BOTH", "NONE", "NONE", "");
				addTnRule("M_SAME", "", "", "");
				addTnRule("ELSE", "", "", "ERROR");
				break;
			}
			case AlgOpNode::lt: 
			case AlgOpNode::gt:
			case AlgOpNode::le:
			case AlgOpNode::ge:{ 
/*BASE*/		addTcRule("integer", "integer", "boolean");
				addTcRule("string", "string", "boolean");
				addTcRule("double", "double", "boolean");
				//suggest not to compare strings vs. integers, OR...! to try and coerce str TO_INTEGER!:
				//addTcRule("string", "integer", "boolean", TypeChecker::BS_TOINT_L, "DYN");
				addTcRule("ELSE", "", "", "ERROR"); 
/*CARD*/		addCdRule("1..1", "1..1", "1..1");
				addCdRule("M_BOTH", "", "", "1..1", TypeChecker::CD_COERCE_11, "STAT"); //DYN
/*T-NAME*/		addTnRule("M_BOTH", "NONE", "NONE", "");
				addTnRule("M_SAME", "", "", "");
				addTnRule("ELSE", "", "", "ERROR");
				break;
			}
			//TODO: fill the rulesets...:
			case AlgOpNode::boolAnd:
			case AlgOpNode::boolOr: {
/*BASE*/		addTcRule("bool", "bool", "bool");
				addTcRule("ELSE", "", "", "ERROR");
/*CARD*/		addCdRule("1..1", "1..1", "1..1");
				addCdRule("M_BOTH", "", "", "1..1", TypeChecker::CD_COERCE_11, "DYN");//DYN_CTRL!
				addCdRule("ELSE", "", "", "ERROR");				break;
			}
			case AlgOpNode::comma: {
/*BASE*/		addTcRule("M_BOTH", "X", "Y", DecisionTable::BS_STRUCT);
/*CARD*/		addCdRule("M_BOTH", "X", "Y", DecisionTable::CD_MULTBANDS);
/*T-NAME*/		addTnRule("M_BOTH", "X", "Y", "");	//struct always gets an empty distinct typename... 
/*C-KIND*/		addCkRule("M_BOTH",	"NONE",	"NONE", "bag");
				addCkRule("M_LEFT",	"NONE",	"bag",	"bag");
				addCkRule("M_RIGHT",	"bag",	"NONE",	"bag");
				addCkRule("bag", "bag",	"bag");
				addCkRule("ELSE","ERROR");
				break;			
			} //structure constructor ... ??? 
			case AlgOpNode::insertInto: { }					// impertative
			case AlgOpNode::assign: { }						// impertative
			case AlgOpNode::semicolon: { break;}			// ?? does nth. 
			case AlgOpNode::insert: { }						// impertative
			case AlgOpNode::refeq: { }	//arguments must be references, else - error ? 
			default: break;
		}
		provideReturnLinks();
	}
	
	void DecisionTable::initNonAlgOpRules(int op) {
		switch (op) {//TODO: fill the rulesets
			case NonAlgOpNode::dot: {
/*BASE*/		addTcRule("M_BOTH", "whatever", "X", DecisionTable::BS_COPY_R);
/*CARD*/		addCdRule("M_BOTH", "X", "Y", DecisionTable::CD_MULTBANDS);
/*T-NAME*/		addTnRule("M_BOTH", "whatever", "X", DecisionTable::ARG_COPY_R);
/*C-KIND*/		addCkRule("M_BOTH",	"NONE",	"NONE", "bag");
				addCkRule("M_LEFT",	"NONE",	"bag",	"bag");
				addCkRule("M_RIGHT",	"bag",	"NONE",	"bag");
				addCkRule("bag", "bag",	"bag");
				addCkRule("M_LEFT", "NONE", "sequence", "sequence");
				addCkRule("M_RIGHT", "sequence", "NONE", "sequence");
				addCkRule("sequence", "sequence", "sequence");
				addCkRule("ELSE","ERROR");				
				break;
			}
			case NonAlgOpNode::join: { //same rules as in struct, collKind taken from dot...
/*BASE*/		addTcRule("M_BOTH", "X", "Y", DecisionTable::BS_STRUCT);
/*CARD*/		addCdRule("M_BOTH", "X", "Y", DecisionTable::CD_MULTBANDS);
/*T-NAME*/		addTnRule("M_BOTH", "X", "Y", "");	//struct always gets an EMPTY distinct typename... 
/*C-KIND*/		addCkRule("M_BOTH",	"NONE",	"NONE", "bag");
				addCkRule("M_LEFT",	"NONE",	"bag",	"bag");
				addCkRule("M_RIGHT",	"bag",	"NONE",	"bag");
				addCkRule("bag", "bag",	"bag");
				addCkRule("M_LEFT", "NONE", "sequence", "sequence");
				addCkRule("M_RIGHT", "sequence", "NONE", "sequence");
				addCkRule("sequence", "sequence", "sequence");
				addCkRule("ELSE","ERROR");
				break;
			}
			case NonAlgOpNode::where: {
/*BASE*/		addTcRule("M_LEFT", "", "boolean", DecisionTable::BS_COPY_L);
				addTcRule("ELSE", 	"ERROR");
/*CARD*/		addCdRule("M_LEFT", "X..Y", "1..1", DecisionTable::CD_COPY_L_ZR);
				addCdRule("ELSE", "", "", DecisionTable::CD_COPY_L_ZR, TypeChecker::CD_COERCE_11_R, "DYN");
/*T-NAME*/		addTnRule("M_BOTH", "X", "whatever", DecisionTable::ARG_COPY_L);
/*C-KIND*/		addCkRule("M_BOTH", "X", "whatever", DecisionTable::ARG_COPY_L);
				break;
			}
			case NonAlgOpNode::closeBy: { }
			case NonAlgOpNode::closeUniqueBy: { }
			case NonAlgOpNode::leavesBy: { }
			case NonAlgOpNode::leavesUniqueBy: { }
			case NonAlgOpNode::orderBy: { }
			case NonAlgOpNode::exists: { }
			case NonAlgOpNode::forAll: { }
			case NonAlgOpNode::forEach: { }
			default: break;
		}
		provideReturnLinks();
	}

	void UnOpDecisionTable::initUnOpRules(int op) {
		if (op >= 0) {
			switch (op) {//TODO: fill the rulesets
				case UnOpNode::count: { }
				case UnOpNode::sum: { }
				case UnOpNode::avg:	{ }
				case UnOpNode::min: { }
				case UnOpNode::max: { }
				case UnOpNode::deref:{
				//must fill'em in so that augmentTreeDeref works out, huh? 
					addTcRule("", "ref", UnOpDecisionTable::BS_REF_DEREF);
					addTcRule("", "string", "string");
					addTcRule("", "integer", "integer");
					addTcRule("", "double", "double");
					addTcRule("", "boolean", "boolean");
					// struct(s1,s2..): return struct(deref(s1), deref(s2), ...); 
					// binder (n, S): return binder (n, deref(S));
					addTcRule("ELSE", "", "ERROR"); // for now - should also consider structs/binders/.
					addCdRule("M", "x", UnOpDecisionTable::ARG_COPY);
					addTnRule("M", "x", "");	//derefed sig has no distinct typename...? 
					addCkRule("M", "x", UnOpDecisionTable::ARG_COPY);
					break;
				}
				case UnOpNode::ref: {  }
				case UnOpNode::distinct: { }
				case UnOpNode::unMinus: { }
				case UnOpNode::boolNot: { }
				case UnOpNode::deleteOp: { }
				case UnOpNode::nameof: { }
				default: break;
			}
		} else {
			op = (-1) * op;
			switch (op) {
				case TreeNode::TNAS: {
					addTcRule("M", "X", UnOpDecisionTable::BS_NAMEAS);
					addCdRule("M", "X", UnOpDecisionTable::CD_NAMEAS);
					addTnRule("M", "X", "");
					addCkRule("M", "X", UnOpDecisionTable::CK_NAMEAS);
					break;
				}
				case TreeNode::TNCREATE: {
					
					break;
				}				
				default: break;
			}
		}
		provideReturnLinks();
	}
	
	
	/** *********************************************************** */

	DecisionTable::DecisionTable(string opType, int oper) {
		this->op = oper; 
		this->algOrNonAlg = opType;
		if (this->algOrNonAlg == "ALG") initAlgOpRules(op);
		else initNonAlgOpRules(op);
	}
	
	int DecisionTable::getResult(TypeCheckResult &finalResult, Signature *lSig, Signature *rSig) {
		int result = 0;
		result += applyRuleset(rules, lSig, rSig, lSig->textType(), rSig->textType(), finalResult);
		if (!finalResult.isError()) {
			result += applyRuleset(cardRules, lSig, rSig, lSig->getCard(), rSig->getCard(), finalResult);
			cout << "[TC]: after card rules" << endl; 
			result += applyRuleset(typeNameRules, lSig, rSig, lSig->getTypeName(), rSig->getTypeName(), finalResult);
			cout << "[TC]: after TYPeNMAME" << endl; 
			result += applyRuleset(collKindRules, lSig, rSig, lSig->getCollKind(), rSig->getCollKind(), finalResult);
			cout << "[TC]: after COLLKIND" << endl; 
		}
		return result;
	}	
	
	Signature *DecisionTable::doSig(int method, Signature *lSig, Signature *rSig) {
		cout << "doing sig based on lSig and rSig: " << endl;
		lSig->putToString();
		rSig->putToString();
		switch(method) {
		case DecisionTable::BS_COPY_L : return leftSig(lSig, rSig);
		case DecisionTable::BS_COPY_R : return rightSig(lSig, rSig);
		case DecisionTable::BS_STRUCT : return doStruct(lSig, rSig);
		default: return NULL;
		}
	}
	
	string DecisionTable::doAttr(int method, string lArg, string rArg) {
		switch(method) {
		case DecisionTable::CD_ADDBANDS : return cdAddBands(lArg, rArg);
		case DecisionTable::CD_MULTBANDS : return cdMultiplyBands(lArg, rArg);
		case DecisionTable::ARG_COPY_L : return copyLeftArg(lArg, rArg);		
		case DecisionTable::CD_COPY_L_ZR : return cdCopyLeftWithZero(lArg, rArg);
		case DecisionTable::ARG_COPY_R : return copyRightArg(lArg, rArg);
		default: return "";
		}
	}
	
	int DecisionTable::applyRuleset (vector<TCRule> ruleSet, Signature *lSig, Signature *rSig, string leftArg, string rightArg, TypeCheckResult &finalResult) {
		cout << "[TC]: " << "APPLYING RULESET WITH 	" << ruleSet.size() << " rules..." << endl; 
		for (unsigned int i = 0; i < ruleSet.size(); i++) {
			if (ruleSet.at(i).appliesTo(leftArg, rightArg)) {
				cout << "[TC]: " << " applied rule: nr "<< i << endl; 
				ruleSet.at(i).coutMe();
				cout << "[TC]: gonna getResult of this rule" << endl;
				return ruleSet.at(i).getResult(lSig, rSig, finalResult);
			} else {
				cout << "[TC]: " << " ....... REJECTED RULE :"; ruleSet.at(i).coutMe();
			}
		}
		if (ruleSet.size() == 0) return 0;
		return -1;
	}
	
	/** methods being result generators.. */ 

	Signature *DecisionTable::leftSig(Signature *lSig, Signature *rSig) {
		return lSig->clone();
	}
	
	Signature *DecisionTable::rightSig(Signature *lSig, Signature *rSig) {
		return rSig->clone();
	}
	
	Signature *DecisionTable::doStruct(Signature *lSig, Signature *rSig) {
		//RETURN: STRUCT( lSig[card=1..1], rSig[card=1..1] )...
		Signature *sig = new SigColl(Signature::SSTRUCT);	
		lSig->setCard("1..1");
		rSig->setCard("1..1");
		((SigColl *)sig)->setElts(lSig);
		((SigColl *)sig)->addToMyList(rSig);
		return sig;
	}
	
	
	string DecisionTable::copyLeftArg(string l, string r) { 
		return l;
	}
	
	string DecisionTable::copyRightArg(string l, string r) {
		return r;
	}
	
	/** end of result generating methods..*/
	/** *********************************************************** */
	/** ***************		UnOpDecisionTable methods	   ******************** */	
	
	int UnOpDecisionTable::applyRuleset (vector<UnOpRule> ruleSet, Signature *sig, string textArg, TypeCheckResult &finalResult) {
		cout << "[TC]: " << "APPLYING RULESET WITH 	" << ruleSet.size() << " rules..." << endl; 
		for (unsigned int i = 0; i < ruleSet.size(); i++) {
			if (ruleSet.at(i).appliesTo(textArg)) {
				cout << "[TC]: " << " applied UNOP rule: nr "<< i << endl; 
				ruleSet.at(i).coutMe();
				return ruleSet.at(i).getResult(sig, finalResult);
			} else {
				cout << "[TC]: " << " ....... REJECTED RULE :"; ruleSet.at(i).coutMe();
			}
		}
		if (ruleSet.size() == 0) {Deb::ug("empty ruleSet"); return 0;}
		return -1;
	}
	
	TypeCheckResult UnOpDecisionTable::getResult(int op, Signature arg) {
		cout << "Doing nothing in unOpTable:: getResult(op, sigArg);..." << endl;
		return TypeCheckResult();
	}
	
	int UnOpDecisionTable::getResult(TypeCheckResult &finalResult, Signature *sig) {
		cout << "Doing unOpTable:: getResult(finResult, sig);..." << endl;
		int result = 0;
		result += applyRuleset(rules, sig, sig->textType(), finalResult);
		if (!finalResult.isError()) {
			result += applyRuleset(cardRules, sig, sig->getCard(), finalResult);
			result += applyRuleset(typeNameRules, sig, sig->getTypeName(), finalResult);
			result += applyRuleset(collKindRules, sig, sig->getCollKind(),finalResult);
		}
		cout << "applied all rulesets (UNOP), with total result: " << result << endl;
		cout << "and my finalResult is...: \n"<< endl;
		cout << finalResult.printAllInfo();
		cout << "ok, returning";
		return result;
	}
	
	Signature *UnOpDecisionTable::doSig(int method, Signature *sig) {
		switch(method) {
			case UnOpDecisionTable::BS_COPY : return sig->clone();
			case UnOpDecisionTable::BS_NAMEAS : return nameAsSig(sig);
			case UnOpDecisionTable::BS_REF_DEREF : return refDerefSig(sig);
			default: return NULL;
		}
	}
	
	string UnOpDecisionTable::doAttr(int method, string arg) {
		switch(method) {
			case UnOpDecisionTable::CD_NAMEAS : return cdNameAs(arg);
			case UnOpDecisionTable::CK_NAMEAS : return ckNameAs(arg);
			case UnOpDecisionTable::ARG_COPY : return arg;
			default: return "";
		}
	}
		
	/** methods being UnOpDecisionTable result generators.. */ 
	
	Signature *UnOpDecisionTable::nameAsSig(Signature *sig) {
		//this->param - the name to be assigned to sig...
		//this->optparam - 1 - grouped, 0 - not grouped. ('..group as..' OR '..as..')
		if (this->optParam == 0) {
			sig->setCard("1..1");
		}
		Signature *resultSig = new StatBinder(this->param, sig);
		return resultSig;
	}
	
	Signature *UnOpDecisionTable::refDerefSig(Signature *sig) {
		//Signature *ret = DataScheme::dScheme()->signatureOfRef(((SigRef *)sig)->getRefNo());
		//ret->setCard(sig->getCard());//if worksIn(i_3) has card [0..1], and i_3 is EMP, with card[0..*], 
		// we still want the orig. [0..1] card. But decisionTables take care of that, here were just tackling sig. base!
		return DataScheme::dScheme()->signatureOfRef(((SigRef *)sig)->getRefNo());
		//return DataScheme::dScheme()->statNested(((SigRef *)sig)->getRefNo(), NULL)->getBinder()->getValue();
		//this statNested whould have to take sig's ID, which we don't have.
	}
	
	string UnOpDecisionTable::cdNameAs(string arg) {
		if (this->optParam == 0) {
			return arg;
		} else {
			return "1..1";
		}
	}
	
	string UnOpDecisionTable::ckNameAs(string arg) {
		if (this->optParam == 0) {
			return arg;
		} else {
			return "";
		}
	}
	
	
	
	/** end of result generating methods..*/
		
	/** *********************************************************** */
	/** ***************		DTable methods	   ******************** */	
	/** :/ :\ :/ ugly design below... */
	void DTable::provideReturnLinks() {
		cout << "DOING UPPER CLASS PROV. RETURN LINKS, IS THIS CORRECT ? " << endl;
		provideReturnLinksForVector(getRules());
		provideReturnLinksForVector(getCardRules());
		provideReturnLinksForVector(getTypeNameRules());
		provideReturnLinksForVector(getCollKindRules());
	}
	
	void DTable::provideReturnLinksForVector(vector<Rule> v) {
		for (unsigned int i = 0; i < v.size(); i++) {
			v.at(i).setDTable(this);
		}
	}
	
	void DecisionTable::provideReturnLinks() {
		cout << "############################################" << endl;
		provideReturnLinksForVector(rules);
		provideReturnLinksForVector(cardRules);
		provideReturnLinksForVector(typeNameRules);
		provideReturnLinksForVector(collKindRules);
	}
	
	void UnOpDecisionTable::provideReturnLinks() {
		provideReturnLinksForVector(rules);
		provideReturnLinksForVector(cardRules);
		provideReturnLinksForVector(typeNameRules);
		provideReturnLinksForVector(collKindRules);
	}
	
	void DecisionTable::provideReturnLinksForVector(vector<TCRule> &v) {
		cout << "Providing links for vector: ";
		for (unsigned int i = 0; i < v.size(); i++) {
			cout << " " << i;
			v.at(i).setDTable(this);
		}
		cout << endl;
	}
	
	void UnOpDecisionTable::provideReturnLinksForVector(vector<UnOpRule> &v) {
		for (unsigned int i = 0; i < v.size(); i++) {
			v.at(i).setDTable(this);
		}
	}
	/** *********************************************************** */
	/** ***************	  DecisionTableHandler methods	******************** */	
	

	DecisionTableHandler *DecisionTableHandler::dTableHandler = new DecisionTableHandler();
	DecisionTableHandler *DecisionTableHandler::getHandle() {
		return dTableHandler;
	}
	
	DecisionTable *DecisionTableHandler::getDTable(string algOrNonAlg, int op) {//"ALG" or "NONALG"
		if (algOrNonAlg == "ALG") {
			if (algOpDTables.find(op) == algOpDTables.end()) {
				algOpDTables[op] = new DecisionTable(algOrNonAlg, op);
			}
			return algOpDTables[op];
		} else {
			if (nonAlgOpDTables.find(op) == nonAlgOpDTables.end()) {
				cout << "###################################################################################" << endl;
				cout << "###################################################################################" << endl;
				cout << "###################################################################################" << endl;
				cout << "###################################################################################" << endl;
				cout << "###################################################################################" << endl;
				nonAlgOpDTables[op] = new DecisionTable(algOrNonAlg, op);
			}
			return nonAlgOpDTables[op];	
		}
	}
	
	UnOpDecisionTable *DecisionTableHandler::getUnOpDTable(int op) {
		if (unOpDTables.find(op) == unOpDTables.end()) {
			unOpDTables[op] = new UnOpDecisionTable(op);
		}
		return unOpDTables[op];
	}
	
	UnOpDecisionTable *DecisionTableHandler::getUnOpDTable(int op, string param, int optParam) {
		if (unOpDTables.find(op) == unOpDTables.end()) {
			unOpDTables[op] = new UnOpDecisionTable(op, param, optParam);
		}
		return unOpDTables[op];
	}
	
	
	DecisionTableHandler::~DecisionTableHandler(){
	//TODO: do I have to do this? (clear these maps by hand?) 
		unOpDTables.clear();
		algOpDTables.clear();
		nonAlgOpDTables.clear();
	}
	
	/** *********************************************************** */
}

