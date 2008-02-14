#include "DecisionTable.h"

#include "Errors/ErrorConsole.h"
#include "QueryExecutor/QueryResult.h"
#include "QueryParser/Stack.h"
#include "QueryParser/TreeNode.h"
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


namespace TypeCheck
{

	/** Decision tables for the type checking procedure. */
	
	void DecisionTable::initAlgOpRules(int op) {
		cout << "[TC]: " << "initint AlgOpRules: " << op << " operator." << endl;
		switch (op) {
			case AlgOpNode::bagIntersect: 
			case AlgOpNode::bagMinus: 
			case AlgOpNode::bagUnion: { //PL: suma mnogosciowa..?
/*BASE*/		addTcRule("M_SAME",	"X",	"X",	DecisionTable::BS_COPY_L);
				addTcRule("ELSE",					"ERROR");
/*CARD*/		
				addCdRule("M_BOTH",	"X",	"Y",	DecisionTable::CD_ADDBANDS);
/*T-NAME*/		
				addTnRule("M_BOTH",	"NONE", "NONE", "");
				addTnRule("M_SAME",	"X",	"X",	"");
				addTnRule("ELSE", 					"ERROR");
/*C-KIND*/		
				addCkRule("M_BOTH",	"NONE",	"NONE", "bag");
				addCkRule("M_LEFT",	"NONE",	"bag",	"bag");
				addCkRule("M_RIGHT","bag",	"NONE",	"bag");
				addCkRule(			"bag",	"bag",	"bag");
				addCkRule("ELSE",					"ERROR");
				break;
			}
			case AlgOpNode::plus: { 
/*BASE*/		addTcRule(			"integer",	"integer",	"integer");
				addTcRule(			"double",	"double",	"double");
				addTcRule(			"string",	"string",	"string");
				addTcRule(			"boolean",	"boolean",	"boolean"); //works like...  OR ? 
				addTcRule(			"integer",	"double",	"double");
				addTcRule(			"double",	"integer",	"double");
				addTcRule(			"integer",	"string",	"string",	TypeChecker::BS_TOSTR_L, "STAT");
				addTcRule(			"string",	"integer",	"string",	TypeChecker::BS_TOSTR_R, "STAT");
				addTcRule("ELSE", 							"ERROR");
/*CARD*/		
				addCdRule(			"1..1",		"1..1",		"1..1");
				addCdRule("M_LEFT", "",			"1..1", 	"1..1", 	TypeChecker::CD_COERCE_11_L, "DYN");
				addCdRule("M_RIGHT","1..1",		"", 		"1..1", 	TypeChecker::CD_COERCE_11_R, "DYN");
				addCdRule("M_BOTH",	"", 		"", 		"1..1", 	TypeChecker::CD_COERCE_11_B, "DYN");//DYN_CTRL!
				addCdRule("ELSE", 							"ERROR");
/*T-NAME*/		
				addTnRule("M_BOTH",	"NONE",		"NONE", 	"");
				addTnRule("ELSE", 							"ERROR");
				break;
			}
			case AlgOpNode::minus: {
/*BASE*/		addTcRule(			"integer", 	"integer", 	"integer");
				addTcRule(			"double", 	"double", 	"double");
				addTcRule(			"integer", 	"double", 	"double");
				addTcRule(			"string", 	"integer", 	"integer",	TypeChecker::BS_TOINT_L, "DYN");
				addTcRule(			"string", 	"double", 	"double",	TypeChecker::BS_TODBL_L, "DYN");
				addTcRule("ELSE", 	"", 		"", 		"ERROR");
/*CARD*/		addCdRule(			"1..1", 	"1..1", 	"1..1");
				addCdRule("M_BOTH",	"", 		"", 		"1..1", TypeChecker::CD_COERCE_11_B, "DYN"); //DYN
/*T-NAME*/		addTnRule("M_BOTH",	"NONE", 	"NONE", 	"");
				addTnRule("ELSE", 	"", 		"", 		"ERROR");
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
				addCdRule("M_BOTH", "", "", "1..1", TypeChecker::CD_COERCE_11_B, "DYN"); //DYN
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
/*CARD*/		
				addCdRule("1..1", "1..1", "1..1");
				addCdRule("M_BOTH", "", "", "1..1", TypeChecker::CD_COERCE_11_B, "DYN"); //DYN
/*T-NAME*/		
				addTnRule("M", "NONE", "NONE", "");
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
/*CARD*/		
				addCdRule("1..1", "1..1", "1..1");
				addCdRule("M_BOTH", "", "", "1..1", TypeChecker::CD_COERCE_11_B, "DYN"); //DYN
/*T-NAME*/		
				addTnRule("M_BOTH", "NONE", "NONE", "");
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
/*CARD*/		
				addCdRule("1..1", "1..1", "1..1");
				addCdRule("M_BOTH", "", "", "1..1", TypeChecker::CD_COERCE_11_B, "DYN");
/*T-NAME*/		
				addTnRule("M_BOTH", "NONE", "NONE", "");
				addTnRule("M_SAME", "", "", "");
				addTnRule("ELSE", "", "", "ERROR");
				break;
			}
			//all comparisons ( < , > , <= , >= ) have the same rules :
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
/*CARD*/		
				addCdRule("1..1", "1..1", "1..1");
				addCdRule("M_BOTH", "", "", "1..1", TypeChecker::CD_COERCE_11_B, "DYN"); //DYN
/*T-NAME*/		
				addTnRule("M_BOTH", "NONE", "NONE", "");
				addTnRule("M_SAME", "", "", "");
				addTnRule("ELSE", "", "", "ERROR");
				break;
			}
			//TODO: fill the rulesets...:
			case AlgOpNode::boolAnd:
			case AlgOpNode::boolOr: {
/*BASE*/		addTcRule("bool", "bool", "bool");
				addTcRule("ELSE", "", "", "ERROR");
/*CARD*/		
				addCdRule("1..1", "1..1", "1..1");
				addCdRule("M_BOTH", "", "", "1..1", TypeChecker::CD_COERCE_11_B, "DYN");//DYN_CTRL!
				addCdRule("ELSE", "", "", "ERROR");
				break;
			}
			case AlgOpNode::comma: {
/*BASE*/		
				addTcRule("M_BOTH", "X", "Y", DecisionTable::BS_STRUCT);
/*CARD*/		
				addCdRule("M_BOTH", "X", "Y", DecisionTable::CD_MULTBANDS);
/*T-NAME*/		
				addTnRule("M_BOTH", "X", "Y", "");	//struct always gets an empty distinct typename... 
/*C-KIND*/		
				addCkRule("M_BOTH",	"NONE",	"NONE", "bag");
				addCkRule("M_LEFT",	"NONE",	"bag",	"bag");
				addCkRule("M_RIGHT","bag",	"NONE",	"bag");
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

	void UnOpDecisionTable::initUnOpRules(int treeType, int op) {
		switch (treeType) {
		case TreeNode::TNUNOP :
			switch (op) {//TODO: fill the rulesets
			case UnOpNode::count: { }
			case UnOpNode::sum: { }
			case UnOpNode::avg:	{ }
			case UnOpNode::min: { }
			case UnOpNode::max: { }
			case UnOpNode::deref:{
				cout << "filling in deref rules\n";
			//must fill'em in so that augmentTreeDeref works 
				addTcRule("", "ref", UnOpDecisionTable::BS_REF_DEREF);
				addTcRule("", "string", "string");
				addTcRule("", "integer", "integer");
				addTcRule("", "double", "double");
				addTcRule("", "boolean", "boolean");
				// struct(s1,s2..): return struct(deref(s1), deref(s2), ...); 
				// binder (n, S): return binder (n, deref(S));
				addTcRule("ELSE", "", "ERROR"); // for now - finally should also consider structs/binders/.
				addCdRule("M", "x", UnOpDecisionTable::ARG_COPY);
				addTnRule("M", "x", UnOpDecisionTable::ARG_COPY);//OR maybe derefed sig should have no typename..? 
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
			break;
		case TreeNode::TNAS: 
			addTcRule("M", "Base", UnOpDecisionTable::BS_NAMEAS);
			addCdRule("M", "CD", UnOpDecisionTable::CD_NAMEAS);
			addTnRule("M", "TN", "");
			addCkRule("M", "CK", UnOpDecisionTable::CK_NAMEAS);	
			break;
		case TreeNode::TNCREATE: 
				//base : argument has to be a binder. else : error. 
				// but when its abinder... bind the name??, and check that args sig = name's sig... ?!
			// cd: well... 
			break;
		default: break;
		}
		provideReturnLinks();
	}
	
	
	/** *********************************************************** */

	/** ***********		Result generator dispatchers. 	*********** */ 
	
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
	
	Signature *UnOpDecisionTable::doSig(int method, Signature *sig, string param, int option) {
		switch(method) {
			case UnOpDecisionTable::BS_COPY : return copySig(sig);
			case UnOpDecisionTable::BS_REF_DEREF : return refDerefSig(sig);
			case UnOpDecisionTable::BS_NAMEAS : return nameAsSig(sig, param, option);
			default: return NULL;
		}
	}
	
	string UnOpDecisionTable::doAttr(int method, string arg, string param, int option) {
		switch(method) {
			case UnOpDecisionTable::CD_NAMEAS : return cdNameAs(arg, option);
			case UnOpDecisionTable::CK_NAMEAS : return ckNameAs(arg, option);
			case UnOpDecisionTable::ARG_COPY : return arg;
			default: return "";
		}
	}

	/** ***************		Result generators.. 	****************/ 
// DecisionTables
	Signature *DecisionTable::leftSig(Signature *lSig, Signature *rSig) {return lSig->clone();}
	
	Signature *DecisionTable::rightSig(Signature *lSig, Signature *rSig) { return rSig->clone();}
	
	Signature *DecisionTable::doStruct(Signature *lSig, Signature *rSig) {
		//RETURN: STRUCT( lSig[card=1..1], rSig[card=1..1] )...
		//Could flatten the structure here, if lSig or rSig were structs..
		Signature *sig = new SigColl(Signature::SSTRUCT);	
		lSig->setCard("1..1");
		rSig->setCard("1..1");
		((SigColl *)sig)->setElts(lSig);
		((SigColl *)sig)->addToMyList(rSig);
		return sig;
	}
	
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
	
	string DecisionTable::copyLeftArg(string lArg, string rArg) {return lArg;}
	
	string DecisionTable::copyRightArg(string lArg, string rArg) {return rArg;}
	
//UnOpDecisionTables
	
	Signature *UnOpDecisionTable::nameAsSig(Signature *sig, string name, int grouped) {
		if (grouped == 0) sig->setCard("1..1");
		Signature *resultSig = new StatBinder(name, sig);
		return resultSig;
	}
	
	Signature *UnOpDecisionTable::refDerefSig(Signature *sig) {
		return DataScheme::dScheme()->signatureOfRef(((SigRef *)sig)->getRefNo());
	}
	
	string UnOpDecisionTable::cdNameAs(string arg, int grouped) {
		return (grouped == 1 ? "1..1" : arg);
	}
	
	string UnOpDecisionTable::ckNameAs(string arg, int grouped) {
		return (grouped == 1 ? "" : arg);
	}
	
	Signature *UnOpDecisionTable::copySig(Signature *sig) {return sig->clone();}
	
		
	/** end of result generating methods..*/
	
	
	/** ***************		DecisionTable (bin) methods	   ******************** */	
	
	DecisionTable::DecisionTable(int opType, int oper) {
		this->op = oper; 
		this->algOrNonAlg = opType;
		switch (opType) {
			case TreeNode::TNALGOP : initAlgOpRules(op); break;
			case TreeNode::TNNONALGOP : initNonAlgOpRules(op); break;
			default: initOtherBinaryRules(op); break;
		}
	}
	
	int DecisionTable::getResult(TypeCheckResult &finalResult, Signature *lSig, Signature *rSig) {
		int result = 0;
		result = applyRuleset(rules, SIG_BASE, lSig, rSig, lSig->textType(), rSig->textType(), finalResult);
		if (!finalResult.isError()) { //don't apply rules for attributes when the base is wrong.
			//return 'ETCNotApplicable' if no rules were given.
			if (finalResult.getSig() == NULL) return (ErrTypeChecker | ETCNotApplicable);
			result += applyRuleset(cardRules, SIG_CARD, lSig, rSig, lSig->getCard(), rSig->getCard(), finalResult);
			result += applyRuleset(typeNameRules, SIG_TN, lSig, rSig, lSig->getTypeName(), rSig->getTypeName(), finalResult);
			result += applyRuleset(collKindRules, SIG_CK, lSig, rSig, lSig->getCollKind(), rSig->getCollKind(), finalResult);
		}
		return result;
	}	
	
	
	int DecisionTable::applyRuleset (vector<TCRule> ruleSet, string atr, Signature *lSig, Signature *rSig, string leftArg, string rightArg, TypeCheckResult &finalResult) {
		cout << "[TC]: " << "APPLYING RULESET WITH 	" << ruleSet.size() << " rules..." << endl; 
		for (unsigned int i = 0; i < ruleSet.size(); i++) {
			if (ruleSet.at(i).appliesTo(leftArg, rightArg)) {
				cout << "[TC]: " << " applied rule: nr "<< i << endl; 
				ruleSet.at(i).coutMe();
				cout << "[TC]: gonna getResult of this rule" << endl;
				return ruleSet.at(i).getResult(atr, lSig, rSig, finalResult);
			} else {
				cout << "[TC]: " << " ....... REJECTED RULE :"; ruleSet.at(i).coutMe();
			}
		}
		if (ruleSet.size() == 0) return 0;
		return -1;
	}

	/** ***************		UnOpDecisionTable methods	   ******************** */	
	
	int UnOpDecisionTable::applyRuleset (vector<UnOpRule> ruleSet, string atr, Signature *sig, string textArg, TypeCheckResult &finalResult, string param, int option) {
		cout << "[TC]: " << "APPLYING RULESET WITH 	" << ruleSet.size() << " rules..." << endl; 
		for (unsigned int i = 0; i < ruleSet.size(); i++) {
			if (ruleSet.at(i).appliesTo(textArg)) {
				cout << "[TC]: " << " applied UNOP rule: nr "<< i << endl; 
				ruleSet.at(i).coutMe();
				return ruleSet.at(i).getResult(atr, sig, finalResult, param, option);
			} else {
				cout << "[TC]: " << " ....... REJECTED RULE :"; ruleSet.at(i).coutMe();
			}
		}
		if (ruleSet.size() == 0) return 0;	//Empty ruleset - leaving default values for signature attributes.
		return -1;
	}
	
	int UnOpDecisionTable::getResult(TypeCheckResult &finalResult, Signature *sig) {
		return getResult(finalResult, sig, "", 0);
	}
	
	int UnOpDecisionTable::getResult(TypeCheckResult &finalResult, Signature *sig, string param, int option) {
		cout << "unOpT::getResult(finResult, sig,...);" << endl;
		int result = 0;
		result += applyRuleset(rules, SIG_BASE, sig, sig->textType(), finalResult, param, option);
		if (!finalResult.isError()) {
			if (finalResult.getSig() == NULL) return (ErrTypeChecker | ETCNotApplicable);
			result += applyRuleset(cardRules, SIG_CARD, sig, sig->getCard(), finalResult, param, option);
			result += applyRuleset(typeNameRules, SIG_TN, sig, sig->getTypeName(), finalResult, param, option);
			result += applyRuleset(collKindRules, SIG_CK, sig, sig->getCollKind(),finalResult, param, option);
		}
		cout << "applied all rulesets" << endl;
		cout << result;
		cout << "unOpT::getResult: ret int: <-- \n";
		cout << ", tcRes:\n" << finalResult.printAllInfo() << endl;
		return result;
	}
				

	/** ***************		DTable methods	   ******************** */	
	/* :/ :\ :/ ugly design ... with class A : B- can't put A into vector< B >..? */
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

	/** ***************	  DecisionTableHandler methods	******************** */	
	
	DecisionTableHandler *DecisionTableHandler::dTableHandler = new DecisionTableHandler();
	DecisionTableHandler *DecisionTableHandler::getHandle() {
		return dTableHandler;
	}
	
	DecisionTable *DecisionTableHandler::getDTable(int algOrNonAlg, int op) {
		map<int, DecisionTable*> *tableMap;
		int place = op;
		switch (algOrNonAlg) {
			case TreeNode::TNALGOP : tableMap = &algOpDTables; break;
			case TreeNode::TNNONALGOP : tableMap = &nonAlgOpDTables; break;
			//a new tree node with many operators may need new tableMap
			default : tableMap = &otherBinaryDTables; place = (_TN_MAX_OP * algOrNonAlg) + op; break;
		}
		if ((*tableMap).find(place) == (*tableMap).end()) {
			(*tableMap)[place] = new DecisionTable(algOrNonAlg, op);
		}
		return (*tableMap)[place];
	}	
	
// 	DecisionTable *DecisionTableHandler::getDTable(int algOrNonAlg, int op) {
// 		map<int, DecisionTable*> *tableMap;
// 		switch (algOrNonAlg) {
// 			case TreeNode::TNALGOP : {
// 				if (algOpDTables.find(op) == algOpDTables.end()) {
// 					algOpDTables[op] = new DecisionTable(algOrNonAlg, op);
// 				}
// 				return algOpDTables[op];
// 			}
// 			case TreeNode::TNNONALGOP : 
// 			default :{
// 				if (nonAlgOpDTables.find(op) == nonAlgOpDTables.end()) {
// 					nonAlgOpDTables[op] = new DecisionTable(algOrNonAlg, op);
// 				}
// 				return nonAlgOpDTables[op];	
// 			}
// 		}
// 	}
	
// 	UnOpDecisionTable *DecisionTableHandler::getUnOpDTable(int treeType, int op) {
// 		
// 		if (unOpDTables.find(op) == unOpDTables.end()) {
// 			unOpDTables[op] = new UnOpDecisionTable(op);
// 		}
// 		return unOpDTables[op];
// 	}
// 	
// 	UnOpDecisionTable *DecisionTableHandler::getUnOpDTable(int op, string param, int optParam) {
// 		if (unOpDTables.find(op) == unOpDTables.end()) {
// 			unOpDTables[op] = new UnOpDecisionTable(op, param, optParam);
// 		}
// 		return unOpDTables[op];
// 	}

	UnOpDecisionTable *DecisionTableHandler::getUnOpDTable(int treeType, int op) {
		map<int, UnOpDecisionTable*> *tableMap;
		int place = op;
		cout << "place: " << place << endl;
		if (treeType == TreeNode::TNUNOP) tableMap = &unOpDTables;
		else { 
			tableMap = &otherUnaryDTables;
			cout << "place changed: " << place << endl;
			place = (_TN_MAX_OP * treeType) + op;
		}
		
		if ((*tableMap).find(place) == (*tableMap).end()) {
			cout << "creating unopDTable(" << treeType << ", " << op << ") at " << place << "\n";
			(*tableMap)[place] = new UnOpDecisionTable(treeType, op);
		}
		return (*tableMap)[place];
	}
	/*
	UnOpDecisionTable *DecisionTableHandler::getUnOpDTable(int treeType, int op, string param, int optParam) {
		map<int, UnOpDecisionTable*> *tableMap;
		if (unOpDTables.find(op) == unOpDTables.end()) {
			unOpDTables[op] = new UnOpDecisionTable(op, param, optParam);
		}
		return unOpDTables[op];
	}
	*/
	DecisionTableHandler::~DecisionTableHandler(){
	//do I have to clear these maps by hand? 
		unOpDTables.clear();
		algOpDTables.clear();
		nonAlgOpDTables.clear();
	}
	
	/** *********** 		Methods adding rules to decision tables 	***************************** */	
	
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
	void DecisionTable::addCdRule (string specArg, string lArg, string rArg, string reslt, int action, string dynStat) {
		this->cardRules.push_back(TCRule(Rule::CARD, specArg, lArg, rArg, reslt, -1, action, dynStat));
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
	
	/** ************************************************************************ */
	
}

