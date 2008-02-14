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
	
#include "ClassNames.h"
#include "DecisionTable.h"
#include "Rule.h"
#include "TypeCheckResult.h"

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

// some TC constants, to use across typechecking procs..
#define TC_FINAL_ERROR -10

#define TC_MDN_NAME			"__MDN__"
#define TC_MDNT_NAME		"__MDNT__"
#define	TC_SUB_OBJ_NAME		"subobject"
#define TC_MDN_ATOMIC		"atomic"
#define TC_MDN_LINK			"link"
#define TC_MDN_COMPLEX		"complex"
#define TC_MDN_DEFTYPE		"defined_type"
#define TC_REF_TYPE_SUFF	"__REFTYPE_"
#define TC_MDS_KIND			"kind"
#define TC_MDS_NAME			"name"
#define TC_MDS_CARD			"card"
#define TC_MDS_ISDIST		"isDistinct"
#define TC_MDS_REFNAME		"refName"
#define TC_MDS_TYPENAME		"typeName"
#define TC_MDS_TYPE			"type"
#define TC_MDS_OWNER		"owner"
#define TC_MDS_TARGET		"target"
#define TC_CK_BAG			"bag"
#define TC_CK_SEQ			"sequence"
#define TC_RS_ERROR			"ERROR"
#define TC_RS_COERCE		"COERCE"
#define TC_RS_SUCCESS		"SUCCESS"

#define TC_INTEGER			"integer"
#define TC_STRING			"string"
#define TC_BOOLEAN			"boolean"
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
				case ARG_ALG : 
				case ARG_NALG: return "Bad Args";
				case ARG_UNOP: return "Bad Arg";
				case ARG_AS: return "Bad Arg";	
			}
			return "Unknown";
		}
		
	public:
		enum TcErrorType {BNAME, ARG_ALG, ARG_NALG, ARG_UNOP, ARG_AS };
		TCError(){};
		TCError(int errType, string output) {
			this->type = typeToStr(errType); 
			this->outputText = generateOutputText(errType, output);
		}
		TCError(int errType, vector<string> er, string attr1, string attr2 = "", string attr3 = "", string attr4 = "") {
			this->type = typeToStr(errType);
			for (unsigned int i = 0; i < er.size(); i++) errorParts.push_back(er.at(i));
			this->outputText = generateOutputText(errType, attr1, attr2, attr3, attr4);
		}
		string generateOutputText(int errType, string attr1, string attr2 = "", string attr3 = "", string attr4 = "") {
			switch (errType) {
				case BNAME : return "Name " + attr1 + " not found.";
				case ARG_ALG : 
				case ARG_NALG: return "Bad arguments for operator " + attr3 + " :\n	" + attr1 + ", " + attr2 + ".";
				case ARG_UNOP: return "Bad argument for operator " + attr2 + " :\n	" + attr1 + ".";	
				case ARG_AS: return "Bad argument for operator '" + attr2 + "' " + attr3 + " : " + attr1 + ".";
			}
			return "Unspecified error";
		}
		virtual string getOutput() {
			string ret = "[error: " +type + "] : " + outputText + "\n";
			if (errorParts.size() > 0) {
				ret += "	Failed attributes: ";
				for (unsigned int i = 0; i < errorParts.size(); i++) {
					if (i > 0) ret += ", ";
					ret += errorParts.at(i);
				}
			}
			return ret;
		}	
		virtual ~TCError(){};
	};
	
	//TODO: does this class REALLY differ from TCError? if not - maybe lets just add an attribute
	//		saying if its an error, or an action, huh?
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
		TCGlobalResult(){overallResult = "SUCCESS";}; //let's presume best-case scenario...
		virtual vector<TCError> getErrors() {return errors;}
		virtual vector<TCAction> getActions() {return implicitActions;}
		virtual void addError(TCError err) {this->errors.push_back(err);}
		virtual void addAction(TCAction act) {this->implicitActions.push_back(act);}
		virtual void reportTypeError(TCError err) {
			overallResult = TC_RS_ERROR;
			cout << "will add error to globalresult" << endl;
			addError(err);
			cout << "Added error to globalresult" << endl;
		}
		virtual void setOverallResult(string res) {this->overallResult = res;}
		virtual bool isError() {return overallResult == TC_RS_ERROR;}
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
		//DecisionTableHandler dTables;
		
		int openScope(Signature *sig);
		int closeScope(Signature *sig);
		int trySingleDeref(Signature *sig, Signature *&sigIn, TypeCheckResult &tmpTcRes, bool &doDeref);
	public:
		enum TcAction {	CD_COERCE_11, CD_COERCE_11_L, CD_COERCE_11_R, CD_COERCE_11_B, 
						BS_TOSTR, BS_TOSTR_L, BS_TOSTR_R, BS_TOSTR_B,
						BS_TOBOOL, BS_TOBOOL_L, BS_TOBOOL_R, BS_TOBOOL_B,
						BS_TODBL, BS_TODBL_L, BS_TODBL_R, BS_TODBL_B,
						BS_TOINT, BS_TOINT_L, BS_TOINT_R, BS_TOINT_B};

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
		virtual int performAction(int actionId, TreeNode *tn, Signature *lSig, Signature *rSig, TypeCheckResult &tcr);
		virtual void reportTypeError(TCError err) { return this->globalResult.reportTypeError(err);};
		virtual int doTypeChecking(string &s);
		
		virtual int typeCheck(TreeNode *tn);
		virtual bool bindError(vector<BinderWrap*> *vec);
		virtual Signature* extractSigFromBindVector(vector<BinderWrap*> *vec);
		virtual int augmentTreeEllipsis(TreeNode *tn, string name, vector<BinderWrap*> *vec, string expander);

		virtual int augmentTreeCoerce(TreeNode *tn, Signature *lSig, Signature *rSig, TypeCheckResult &tcRes);
		
		virtual int augmentTreeDeref(TreeNode *tn, DecisionTable *dt, TypeCheckResult &tcRes, Signature *lSig, Signature *rSig);
		virtual int augmentTreeDeref(TreeNode *tn, UnOpDecisionTable *udt, TypeCheckResult &tcRes, Signature *sig);
		
		virtual int restoreAfterMisspelledName(TreeNode *tn, string name, vector<BinderWrap*> *vec);
		virtual int restoreAfterBadArg(TreeNode *tn, DecisionTable *dt, TypeCheckResult &tcr, Signature *lSig, Signature *rSig);
		virtual int restoreAfterBadArg(TreeNode *tn, UnOpDecisionTable *dt, TypeCheckResult &tcr, Signature *sig);	
		virtual ~TypeChecker();
		
	};
	

}

	
#endif /* __TC_TYPE_CHECKER_H__ */
