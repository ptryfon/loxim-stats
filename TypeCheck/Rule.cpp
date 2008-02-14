#include "Rule.h"

#include "Errors/ErrorConsole.h"
#include "Errors/Errors.h"
#include "QueryExecutor/QueryResult.h"
#include "QueryParser/Stack.h"
#include "QueryParser/Deb.h"
#include "TransactionManager/Transaction.h"
#include "TransactionManager/Mutex.h"
#include "TypeChecker.h"
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

using namespace Errors;
using namespace QParser;
using namespace std;



namespace TypeCheck
{

/** #################### Vital methods for TCRule ####################... */
	bool TCRule::appliesTo(string lArg, string rArg) {
		if (this->specialArg == "ELSE") return true;
		if (this->specialArg == "M_BOTH") 
			return !((this->leftArg == "NONE" && !isEmpty(lArg)) ||
					(this->rightArg == "NONE" && !isEmpty(rArg)));
		if (this->specialArg == "M_SAME") return (lArg == rArg);
		if (this->specialArg == "M_LEFT")
			return !(rArg != this->rightArg || (this->leftArg == "NONE" && !isEmpty(lArg)));
		if (this->specialArg == "M_RIGHT")
			return !(lArg != this->leftArg || (this->rightArg == "NONE" && !isEmpty(rArg)));
		if ((this->specialArg == "ACCURATE" || isEmpty(this->specialArg)) &&
			(lArg == this->leftArg && rArg == this->rightArg)) return true;
		return false; /* rule doesn't match given input. */
	}
	
	int TCRule::getResult(string atr, Signature *lSig, Signature *rSig, TypeCheckResult &retResult) {
		if (this->result == TC_RS_ERROR) {
			retResult.setEffect(TC_RS_ERROR); //should give more info on the error, but.. HERE?
			cout << "tc rule: setting an error part: " << atr << endl;
			retResult.addErrorPart(atr);
			return 0;
		}
		cout << "RULE::getResult: not and error rule" << endl;
		if (needsAction()) {
			retResult.setEffect(TC_RS_COERCE);
			retResult.addActionId(this->action);
		}
		retResult.setDynCtrl(retResult.isDynCtrl() || this->dynCtrl);
		if (!needsResultGenerator()) {
			Deb::ug("Simple generator is enough");
			return getSimpleResult(lSig, rSig, retResult);
		} else {
			Deb::ug("Needs result generator");
			return getGeneratedResult(lSig, rSig, retResult);
		}
		return 0;
	}
	
	int TCRule::getSimpleResult(Signature *lSig, Signature *rSig, TypeCheckResult &retResult) {
		switch (this->getRuleType()) {
			case Rule::BASE: {
				retResult.setResultSig(this->result);
				break;}
			case Rule::CARD: {
				retResult.getSig()->setCard(this->result);
				break;}
			case Rule::TYPENAME: { 
				retResult.getSig()->setTypeName(this->result);
				break;}
			case Rule::COLLKIND: { 
				retResult.getSig()->setCollKind(this->result);
				break;}
			default: return (ErrTypeChecker  | ETCInnerRuleUnknown); 
		}
		return 0;
	}
	int TCRule::getGeneratedResult(Signature *lSig, Signature *rSig, TypeCheckResult &retResult) {
		switch (this->getRuleType()) {
		case Rule::BASE: {
			if (Deb::ugOn()) {cout << "TCRule::getGenRes: my sigGen: "<< sigGen << ", my attrGen: " << attrGen << endl;}
			if (dTable != NULL) {
				if (Deb::ugOn() && (lSig == NULL || rSig == NULL)) cout << "one of the signatures is NULL !\n";
				retResult.setSig(dTable->doSig(this->sigGen, lSig, rSig));
			} else return (ErrTypeChecker  | ETCInnerNULLFailure);
			break;}
		case Rule::CARD: {
			retResult.getSig()->setCard(dTable->doAttr(attrGen, lSig->getCard(), rSig->getCard()));
			break;}
		case Rule::TYPENAME: { 
			retResult.getSig()->setTypeName(dTable->doAttr(attrGen, lSig->getTypeName(), rSig->getTypeName()));
			break;}
		case Rule::COLLKIND: { 
			retResult.getSig()->setCollKind(dTable->doAttr(attrGen, lSig->getCollKind(), rSig->getCollKind()));
			break;}
		default: return (ErrTypeChecker  | ETCInnerRuleUnknown); 
		}
		return 0;
	}
/** ----------------- end of vital methods for TCRule ---------------- */

/** #################### Constructors & Destr. ####################... */
	void TCRule::coutMe() {
		cout << ruleType << ":" << specialArg << ",(" << leftArg << "," << rightArg<<")-> " << result <<". " << endl;
	}

	TCRule::TCRule() {
		specialArg = "ACCURATE";
		ruleType = Rule::BASE; 
		sigGen = -1; attrGen = -1; action = -1;
		leftArg = ""; rightArg = "";
		dTable = NULL;
		dynCtrl = false;
	};
	// constructors used for simple rules 
	TCRule::TCRule(int rType, string lArg, string rArg, string res) {
		specialArg = "ACCURATE";
		ruleType = rType;
		result = res;
		sigGen = -1; attrGen = -1; action = -1; 
		leftArg = lArg;
		rightArg = rArg;
		dTable = NULL;
		dynCtrl = false;
	}
	
	// constructors for more complicated rules		
	TCRule::TCRule (int rType, string specArg, string lArg, string rArg, string resStr, int resGen, int actId, string dStr) {
		specialArg = specArg;
		ruleType = rType;
		result = resStr;
		sigGen = -1; attrGen = -1;
		if (rType == Rule::BASE) sigGen = resGen; else attrGen = resGen;
		leftArg = lArg;
		rightArg = rArg;
		dTable = NULL;
		action = actId;
		dynCtrl = (dStr == "DYN");
	}

/** --------------- end of constructors & desctructors --------------- */
/** #################### UnOpRule methods... ####################... */

	UnOpRule::UnOpRule(int rType, string _arg, string res) {
		specialArg = "ACCURATE";
		ruleType = rType;
		result = res;
		sigGen = -1, attrGen = -1; action = -1;
		arg = _arg;
		dTable = NULL;
		dynCtrl = false;
	}
	
	UnOpRule::UnOpRule (int rType, string specArg, string _arg, string resStr, int resGen, int actId, string dStr) {
		specialArg = specArg;
		ruleType = rType;
		result = resStr;
		sigGen = -1; attrGen = -1;
		if (rType == Rule::BASE) sigGen = resGen; else attrGen = resGen;
		arg = _arg;
		dTable = NULL;
		action = actId;
		dynCtrl = (dStr == "DYN");
	}
	
	int UnOpRule::getResult(string atr, Signature *sig, TypeCheckResult &retResult, string param, int option) {
		cout << "in unoprule, getResult" << endl;
		if (this->result == TC_RS_ERROR) {
			retResult.setEffect(TC_RS_ERROR); 
			cout << "gonna set error part: " << atr << endl;
			retResult.addErrorPart(atr);
			return 0;
		}
		if (needsAction()) {
			retResult.setEffect(TC_RS_COERCE);
			retResult.addActionId(this->action);
		}
		retResult.setDynCtrl(retResult.isDynCtrl() || this->dynCtrl);
		if (!needsResultGenerator()) {
			cout << "in unoprule, get SIMPLE Result" << endl;
			return getSimpleResult(sig, retResult);
		} else {
			cout << "in unoprule, get GENERATEEDD Result" << endl;
			return getGeneratedResult(sig, retResult, param, option);
		}
		return 0;
	}
	
	int UnOpRule::getSimpleResult(Signature *argSig, TypeCheckResult &retResult) {
		switch (this->getRuleType()) {
			case Rule::BASE: {
				retResult.setResultSig(this->result);
				break;}
			case Rule::CARD: {
				retResult.getSig()->setCard(this->result);
				break;}
			case Rule::TYPENAME: { 
				retResult.getSig()->setTypeName(this->result);
				break;}
			case Rule::COLLKIND: { 
				retResult.getSig()->setCollKind(this->result);
				break;}
			default: return -1; //unknown Rule type... 
		}
		return 0;
	}
		
	int UnOpRule::getGeneratedResult(Signature *argSig, TypeCheckResult &retResult, string param, int option) {
		cout << "in unoprule, getGENResult extended" << endl;
		if (dTable == NULL) {
			cout << "in unoprule, dTable IS NULL" << endl;
		} else {
			cout << "in unoprule, dTable is probably ok." << endl;
		}
		switch (this->getRuleType()) {
			case Rule::BASE: {
				retResult.setSig(dTable->doSig(this->sigGen, argSig, param, option));
				break;}
			case Rule::CARD: {
				retResult.getSig()->setCard(dTable->doAttr(attrGen, argSig->getCard(), param, option));
				break;}
			case Rule::TYPENAME: { 
				retResult.getSig()->setTypeName(dTable->doAttr(attrGen, argSig->getTypeName(), param, option));
				break;}
			case Rule::COLLKIND: {
				retResult.getSig()->setCollKind(dTable->doAttr(attrGen, argSig->getCollKind(), param, option));
				break;}
			default: return -1; //unknown Rule type... 
		}
		return 0;
	}
	
	
	
/** --------------- end of UnOpRule methods --------------- */

}




