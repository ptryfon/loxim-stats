#ifndef __TYPE_CHECK_RESULT_H__
#define __TYPE_CHECK_RESULT_H__

#include "ClassNames.h"
#include "Errors/ErrorConsole.h"
#include "QueryExecutor/QueryResult.h"
#include "QueryParser/Stack.h"
#include "Store/Store.h"
#include "TransactionManager/Transaction.h"
#include "TransactionManager/Mutex.h"

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

	class TypeCheckResult {
	private:
		Signature *resultSig;
		string effect;	//"ERROR" OR "SUCCESS" OR "COERCE" 
		bool dynCtrl; //states if any part of the control is put off to qExecution phase.
		vector<int> actionIds;
		vector<string> errorParts;//which parts of the signature caused the error - a sig. attr. or base.
		//int (*resultAction) (TypeChecker *, Singature *, Signature*);
		//TODO: do I need the Signatures? resultAction is an implicit augmentTreeOperation:
		//coertion, elipsis handling, dereference. 

	public:
		TypeCheckResult() {resultSig = NULL; effect = ""; dynCtrl = false;}
		TypeCheckResult(string res);
		virtual string getEffect() {return effect;}
		virtual string setEffect(string e);
		virtual Signature *getSig() {return resultSig;}
		virtual void setSig(Signature *sig) {resultSig = sig;};
		virtual void setResultSig(string res);
		virtual bool isSuccess() {return (effect == "SUCCESS" || effect == "");}
		virtual bool isError() {return (effect == "ERROR");}
		virtual bool isCoerce() {return (effect == "COERCE");}
		virtual bool isDynCtrl() {return dynCtrl;}
		virtual void setDynCtrl(bool dc) {dynCtrl = dc;}
		virtual void addActionId(int actId);
		virtual void addErrorPart(string atr);
		virtual vector<int> getActionIds() {return this->actionIds;}
		virtual int actionsNumber() {return actionIds.size();}
		virtual int getActionAt(int position) {return actionIds.at(position);}
		virtual vector<string> getErrorParts() {return this->errorParts;}
		virtual string printAllInfo();
		virtual void clear();
		virtual void fill(TypeCheckResult &tcr);
		virtual ~TypeCheckResult();
	};

}

#endif /* __TYPE_CHECK_RESULT_H__ */
