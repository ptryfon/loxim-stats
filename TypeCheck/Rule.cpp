#include "Rule.h"

#include "Errors/ErrorConsole.h"
#include "QueryExecutor/QueryResult.h"
#include "QueryParser/Stack.h"
#include "TransactionManager/Transaction.h"
#include "TransactionManager/Mutex.h"

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
	
	int TCRule::getResult(Signature *lSig, Signature *rSig, TypeCheckResult &retResult) {
		if (this->result == "ERROR") {
			retResult.setEffect("ERROR"); //should give more info on the error, but.. HERE?
			//or outside this method- from typechecker?
			return 0;
		}
		cout << "RULE::getResult: not and error rule" << endl;
		if (needsAction()) {
			retResult.setEffect("COERCE");
			retResult.addActionId(this->action);
		}
		retResult.setDynCtrl(retResult.isDynCtrl() || this->dynCtrl);
		if (!needsResultGenerator()) {
			cout << "DOES NOT NEED RES. GEN" << endl;
			return getSimpleResult(lSig, rSig, retResult);
		} else {
			cout << "NEEDs RES. GEN" << endl;
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
				//if (retResult.getSig()->isColl()) {((SigColl *)retResult.getSig())->setCollTypeByString(this->result);}
				break;}
			default: cout << "unknown rule type" << endl; return -1; //unknown Rule type... 
		}
		return 0;
	}
	int TCRule::getGeneratedResult(Signature *lSig, Signature *rSig, TypeCheckResult &retResult) {
		cout << "doing result generation" << endl;
		if (dTable != NULL) {
			cout << "GEN RES:: dTable is not null " << endl;
		} else {
			cout << "GEN RES:: !!! dTable is NULL !!! " << endl;
		}
		switch (this->getRuleType()) {
		case Rule::BASE: {
				cout << "my sigGen: ";
				cout << sigGen << ", my attrGen: ";
				cout << attrGen << endl;
				cout << "now will setSig..." << endl;
				//lSig->putToString(); rSig->putToString();
			//retResult.setSignature((*(this->resultGenerator))(lSig, rSig));
			if (dTable != NULL) {
				cout << "dTable is not null " << endl;
				if (lSig != NULL) cout << "lSig AINT NULL" << endl;
				if (rSig != NULL) cout << "rSig AINT NULL" << endl;
				if (lSig == NULL) cout << "lSig IS !!! NULL" << endl;
				if (rSig == NULL) cout << "rSig IS !!! NULL" << endl;
				//Signature *resSig = dTable->doSig(sigGen, lSig, rSig);
				retResult.setSig(dTable->doSig(this->sigGen, lSig, rSig));
			} else {
				cout << "!!! dTable is NULL !!! " << endl;
			}
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
		default: return -1; //unknown Rule type... 
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
	
	int UnOpRule::getResult(Signature *sig, TypeCheckResult &retResult) {
		cout << "in unoprule, getResult" << endl;
		if (this->result == "ERROR") {
			retResult.setEffect("ERROR"); 
			return 0;
		}
		if (needsAction()) {
			retResult.setEffect("COERCE");
			retResult.addActionId(this->action);
		}
		retResult.setDynCtrl(retResult.isDynCtrl() || this->dynCtrl);
		if (!needsResultGenerator()) {
			cout << "in unoprule, get SIMPLE Result" << endl;
			return getSimpleResult(sig, retResult);
		} else {
			cout << "in unoprule, get GENERATEEDD Result" << endl;
			return getGeneratedResult(sig, retResult);
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
				if (retResult.getSig()->isColl()) {
					((SigColl *)retResult.getSig())->setCollTypeByString(this->result);
				}
				break;}
			default: return -1; //unknown Rule type... 
		}
		return 0;
	}
	
	int UnOpRule::getGeneratedResult(Signature *argSig, TypeCheckResult &retResult) {
		cout << "in unoprule, getGENResult" << endl;
		if (dTable == NULL) {
			cout << "in unoprule, dTable IS NULL" << endl;
		} else {
			cout << "in unoprule, dTable is probably ok." << endl;
		}
		switch (this->getRuleType()) {
			case Rule::BASE: {
				retResult.setSig(dTable->doSig(this->sigGen, argSig));
				retResult.getSig()->putToString();
				break;}
			case Rule::CARD: {
				retResult.getSig()->setCard(dTable->doAttr(attrGen, argSig->getCard()));
				break;}
			case Rule::TYPENAME: { 
				retResult.getSig()->setTypeName(dTable->doAttr(attrGen, argSig->getTypeName()));
				break;}
			case Rule::COLLKIND: { 
				if (retResult.getSig()->isColl()) {
					((SigColl *)retResult.getSig())->setCollTypeByString(dTable->doAttr(attrGen, argSig->textType()));
				}
				break;}
			default: return -1; //unknown Rule type... 
		}
		return 0;
	}
	
/** --------------- end of UnOpRule methods --------------- */

}




