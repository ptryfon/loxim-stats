#ifndef __TC_TYPE_CHECKER_H__
#define __TC_TYPE_CHECKER_H__

#include "Errors/ErrorConsole.h"
#include "QueryExecutor/QueryResult.h"
#include "Store/Store.h"
#include "TransactionManager/Transaction.h"
#include "TransactionManager/Mutex.h"
#include "QueryParser/Deb.h"
#include "QueryParser/Stack.h"
#include "QueryParser/TreeNode.h"
#include "QueryParser/DataRead.h"
#include "TCConstants.h"
#include "ClassNames.h"
#include "DecisionTable.h"
#include "Rule.h"
#include "TypeCheckResult.h"
#include "RestoreAlgorithm.h"

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
	
	class TCError {
	
	private:
		string type;		//"BAD_ARG", "BAD_NAME", ...
		string outputText;
		vector<string> errorParts;
		string typeToStr(int errType) {
			switch(errType) {
				case BNAME : return "Bad Name";
				case ARG_ALG : return "Bad Args";
				case ARG_NALG: return "Bad Args";
				case ARG_UNOP: return "Bad Arg";
				case ARG_AS: return "Bad Arg";	
				case ARG_CASTTO: return "Invalid Cast";
				case CREATE: return "Create failure";
				case UNKNOWN: return "Unknown";
			}
			return "Unknown";
		}
		
	public:
		enum TcErrorType {BNAME, ARG_ALG, ARG_NALG, ARG_UNOP, ARG_AS, ARG_CASTTO, CREATE, UNKNOWN };
		enum ErAttr {_op = 0, _lsig = 1, _rsig = 2, _lsigD = 3, _rsigD = 4};
		enum ErUnAttr {_sig = 1, _parm = 2, _sigD = 3};
		
		TCError(){};
		TCError(int errType, string output) {
			this->type = typeToStr(errType); 
			this->outputText = generateOutputText(errType, output);
		}
		TCError(int errType, vector<string> er, vector<string> attr) {
			this->type = typeToStr(errType);
			for (unsigned int i = 0; i < er.size(); i++) errorParts.push_back(er.at(i));
			this->outputText = generateOutputText(errType, attr);
		}
		string generateOutputText(int errType, string attr) {
			switch (errType) {
				case BNAME : return "Name \"" + attr + "\" not found.";
				case ARG_CASTTO: return "Cannot evaluate '" + attr + "' as proper signature.";
				default: break;
			}
			return "Unspecified error";
		}
		string generateOutputText(int errType, vector<string> &at) {
			string ret = "";
			switch (errType) {
				case BNAME : return "Name \"" + at[0] + "\" not found.";
				case ARG_ALG : 
				case ARG_NALG: 
					ret = " of args: ";
					ret += "" + at[_lsig] + ", " + at[_rsig];
					if (at[_lsig] != at[_lsigD] || at[_rsig] != at[_rsigD])
						ret += "\n	(derefed to: " + at[_lsigD] + ", " + at[_rsigD] + ")";
					ret += "\n	invalid for operator  ' " + at[_op] + " '.";
					return ret;
				case ARG_UNOP: 
					ret = " of arg: " + at[_sig] + " ";
					if (at[_sigD] != "" && at[_sig] != at[_sigD])
						ret += "  (derefed to: " + at[_sigD] + ")\n	";
					ret += "invalid for operator ' " + at[_op] + " '.";
				case ARG_AS: 
					return " of arg: " + at[_sig] + " invalid for operator '" + at[_op] + " " + at[_parm] + ".";
				case ARG_CASTTO: 
					ret = " does not match. Cannot cast " + at[_lsig] + " to " + at[_rsig] + "";
					if (at[_lsig] != at[_lsigD] || at[_rsig] != at[_rsigD])
						ret += "\n	(After deref: cannot cast " + at[_lsigD] + " to " + at[_rsigD] + ")";
					return ret;
				case CREATE:
					ret = " error - cannot create " + at[_parm] + " with signature: " + at[_sig] + ".";
					return ret;
			}
			return "Unspecified error";
		}
		virtual string getOutput() {
			string ret = "[error: " +type + "] : ";
			if (errorParts.size() > 0) {
				//ret += "	Failed attributes: ";
				for (unsigned int i = 0; i < errorParts.size(); i++) {
					if (i > 0) ret += ", ";
					ret += errorParts.at(i);	// ;-) nice to see it in plural. for 2-arg...
				}
			}
			ret += " " + outputText;
			return ret;
		}	
		virtual ~TCError(){};
	};
	
	//Does this class REALLY differ from TCError? 
	// Class not used any more.
	class TCAction {	
	private:
		string type;		//"DEREF", "COERCE", "ELLIPSIS", ...
		string outputText;
		
	public:
		TCAction(){};
		TCAction(string type, string output) {this->type = type; this->outputText = output;}
		virtual string getOutput() {
			return type + ": " + outputText;
		}
		virtual ~TCAction(){};
	};	
	
	class TCGlobalResult {
	
	private:
		vector<TCError> errors;
		vector<TCAction> implicitActions; //derefs, coerces, ellipsis... 
		string overallResult;	// "SUCCESS", "ERROR", "DYNAMIC_CTRL"
		//could also add field such as: onErrorActions, which would hold all actions
		//done on error, in order to restore the typechecking process. So, eg.: if
		//a name'd been miss-spelled, then what name was chosen to replace it, etc.
	public:
		TCGlobalResult(){overallResult = "SUCCESS";}; 	//let's presume default success...
		virtual vector<TCError> getErrors() {return errors;}
		virtual vector<TCAction> getActions() {return implicitActions;}
		virtual void addError(TCError err) {this->errors.push_back(err);}
		virtual void addAction(TCAction act) {this->implicitActions.push_back(act);}
		virtual void reportTypeError(TCError err);
		virtual void setOverallResult(string res) {this->overallResult = res;}
		virtual bool isError();
		virtual void printOutput();
		virtual string getOutput();
		virtual ~TCGlobalResult(){};
	};	
	
	class TypeChecker {	
	
	private: 
		TCGlobalResult globalResult;//	global type-check result: SUCCESS/ERROR, list of errors..
		StatQResStack *sQres;		//	static query result stack
		StatEnvStack *sEnvs;		//	static environment stack
		TreeNode *tNode;			//	the query tree to be type-checked.
		RestoreAlgorithm *restoreAlgorithm;
		
		int openScope(Signature *sig);
		int closeScope(Signature *sig);
		int trySingleDeref(bool canDeref, Signature *sig, Signature *&sigIn, TypeCheckResult &tmpTcRes, bool &doDeref);
	public:

		TypeChecker();
		TypeChecker(TreeNode *tn);
			
		virtual void setQres(StatQResStack *nq) {this->sQres = nq;};
		virtual void setEnvs(StatEnvStack *ne) {this->sEnvs = ne;};
		virtual void setTNode(TreeNode *tn) {this->tNode = tn;};
		virtual DecisionTableHandler *getDTables() {return DecisionTableHandler::getHandle();}
		virtual DecisionTable *getDTable(int algOrNonAlg, int op) {return DecisionTableHandler::getHandle()->getDTable(algOrNonAlg, op);}
		
		/** augmentCoerce methods, params taken from action ids in tcResult */
		int modifyTreeCoerce(int coerceType, TreeNode *tn, bool augLeft, bool augRight);
		int modifyTreeCoerce(int coerceType, TreeNode *tn);
	/*
		virtual int coerceCardsTo11(TreeNode *tn);
		virtual int coerceOneCardTo11(TreeNode *tn, bool isLeft);
		virtual int coerceCardTo11(TreeNode *tn);
		virtual int coerceToString(TreeNode *tn, bool leftArg, bool rightArg);
		virtual int coerceToString(TreeNode *tn);
		virtual int coerceToBool(TreeNode *tn, bool augLeft, bool augRight);
		virtual int coerceToBool(TreeNode *tn);
		virtual int coerceToDouble(TreeNode *tn, bool augLeft, bool augRight);
		virtual int coerceToInt(TreeNode *tn, bool augLeft, bool augRight);
	*/
		/** end of pointered methods... */
		virtual int performAction(ActionStruct actionId, TreeNode *tn);
		virtual int performSingleArgAction(int actionId, TreeNode *tn);
		virtual int performMarkAction(int actionId, TreeNode *tn);
		virtual void reportTypeError(TCError err) { return this->globalResult.reportTypeError(err);};
		virtual int doTypeChecking(string &s);
		
		virtual int typeCheck(TreeNode *tn);
		virtual bool bindError(vector<BinderWrap*> *vec);
		virtual Signature* extractSigFromBindVector(vector<BinderWrap*> *vec);
		
		virtual int processAugmentDerefCoerceRestore(int nodeType, int op, string opStr, Signature *lSig, Signature *rSig, TreeNode *tn);
		
		virtual int processAugmentDerefCoerceRestoreUnOp(int nodeType, int errType, int op, string opStr, Signature *argSig, TreeNode *tn);
		virtual int processAugmentDerefCoerceRestoreUnOp(int nodeType, int errType, int op, string opStr, Signature *argSig, TreeNode *tn, string name, int option, bool doDeref);
		
		virtual int augmentTreeEllipsis(TreeNode *tn, string name, vector<BinderWrap*> *vec, string expander);

		virtual int augmentTreeCoerce(TreeNode *tn, Signature *lSig, Signature *rSig, TypeCheckResult &tcRes);
		
		virtual int augmentTreeDeref(TreeNode *tn, DecisionTable *dt, TypeCheckResult &tcRes, Signature *lSig, Signature *rSig, Signature *&lSigIn, Signature *&rSigIn);
		virtual int augmentTreeDeref(TreeNode *tn, UnOpDecisionTable *udt, TypeCheckResult &tcRes, Signature *sig, Signature *&sigIn);
		
		virtual int restoreAfterMisspelledName(TreeNode *tn, string name, vector<BinderWrap*> *vec);
		virtual int restoreAfterBadArg(TreeNode *tn, int tType, int op, TypeCheckResult &tcr, Signature *lSig, Signature *rSig);
		virtual int restoreAfterBadArg(TreeNode *tn, int tType, int op, TypeCheckResult &tcr, Signature *sig, string param, int option);	
		virtual ~TypeChecker();
		
	};
	

}

	
#endif /* __TC_TYPE_CHECKER_H__ */
