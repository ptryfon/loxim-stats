#include "TypeChecker.h"

#include "DecisionTable.h"
#include "Rule.h"
#include "TypeCheckResult.h"

#include "Errors/ErrorConsole.h"
#include "../Errors/Errors.h"
#include "QueryExecutor/QueryResult.h"
#include "QueryParser/Stack.h"
#include "QueryParser/TreeNode.h"
#include "QueryParser/DataRead.h"
#include "QueryParser/Deb.h"

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
	/* ------------ coerce methods --------------- */
	
	//	coerce types: to_string, to_double, to_bool, element, to_bag, to_seq	
/*	
	int TypeChecker::coerceCardsTo11(TreeNode *tn) {
		Deb::ug("Coercing cards to 1..1 ");
		return tn->augmentTreeCoerce(CoerceNode::element, true, true);
	}
	
	int TypeChecker::coerceOneCardTo11(TreeNode *tn, bool isLeft) {
		Deb::ug("coercing one card to 1..1 ");
		if (isLeft) return tn->augmentTreeCoerce(CoerceNode::element, true, false);
		else return tn->augmentTreeCoerce(CoerceNode::element, false,true);
	}
	
	int TypeChecker::coerceToString(TreeNode *tn, bool leftArg, bool rightArg) {
		Deb::ug("coercing base to string");
		return tn->augmentTreeCoerce(CoerceNode::to_string, leftArg, rightArg);
	}
	int TypeChecker::coerceToBool(TreeNode *tn, bool augLeft, bool augRight) {
		return tn->augmentTreeCoerce(CoerceNode::to_bool, augLeft, augRight);
	}
	int TypeChecker::coerceToDouble(TreeNode *tn, bool augLeft, bool augRight) {
		return tn->augmentTreeCoerce(CoerceNode::to_double, augLeft, augRight);
	}
	int TypeChecker::coerceToInt(TreeNode *tn, bool augLeft, bool augRight) {
		return tn->augmentTreeCoerce(CoerceNode::to_int, augLeft, augRight);
	}
	int TypeChecker::coerceToString(TreeNode *tn) {
		return tn->augmentTreeCoerce(CoerceNode::to_string);
	}
	int TypeChecker::coerceToBool(TreeNode *tn) {
		return tn->augmentTreeCoerce(CoerceNode::to_bool);
	}
	int TypeChecker::coerceCardTo11(TreeNode *tn) {
		return tn->augmentTreeCoerce(CoerceNode::element);
	}
*/
	int TypeChecker::modifyTreeCoerce(int coerceType, TreeNode *tn, bool augLeft, bool augRight) {
		return tn->augmentTreeCoerce(coerceType, augLeft, augRight);
	}
	int TypeChecker::modifyTreeCoerce(int coerceType, TreeNode *tn) {
		return tn->augmentTreeCoerce(coerceType);
	}
	/* ------------ end of coerce methods --------------- */
	
	int TypeChecker::performAction(int actionId, TreeNode *tn, Signature *lSig, Signature *rSig, TypeCheckResult &tcr) {
		switch (actionId) {
			case CD_COERCE_11_L : return modifyTreeCoerce(CoerceNode::element, tn, true, false); 
			case CD_COERCE_11_R : return modifyTreeCoerce(CoerceNode::element, tn, false, true); 
			case CD_COERCE_11_B : return modifyTreeCoerce(CoerceNode::element, tn, true, true);
			case CD_COERCE_11 : modifyTreeCoerce(CoerceNode::element, tn);
			case BS_TOSTR_L : return modifyTreeCoerce(CoerceNode::to_string, tn, true, false);
			case BS_TOSTR_R : return modifyTreeCoerce(CoerceNode::to_string, tn, false, true);
			case BS_TOSTR_B : return modifyTreeCoerce(CoerceNode::to_string, tn, true, true);
			case BS_TOSTR : return modifyTreeCoerce(CoerceNode::to_string, tn);
			case BS_TOBOOL_L : return modifyTreeCoerce(CoerceNode::to_bool, tn, true, false);
			case BS_TOBOOL_R : return modifyTreeCoerce(CoerceNode::to_bool, tn, false, true);
			case BS_TOBOOL_B : return modifyTreeCoerce(CoerceNode::to_bool, tn, true, true);
			case BS_TOBOOL : return modifyTreeCoerce(CoerceNode::to_bool, tn);
			case BS_TODBL_L : return modifyTreeCoerce(CoerceNode::to_double, tn, true, false);
			case BS_TOINT_L : return modifyTreeCoerce(CoerceNode::to_int, tn, true, false);
			default: return -1;
		}
		
// 		switch (actionId) {
// 			case TypeChecker::CD_COERCE_11_L : return coerceOneCardTo11(tn, true);
// 			case TypeChecker::CD_COERCE_11_R : return coerceOneCardTo11(tn, false);
// 			case TypeChecker::CD_COERCE_11_B : return coerceCardsTo11(tn);
// 			case TypeChecker::CD_COERCE_11 : return coerceCardTo11(tn);
// 			case TypeChecker::BS_TOSTR_L : return coerceToString(tn, true, false);	
// 			case TypeChecker::BS_TOSTR_R : return coerceToString(tn, false, true);
// 			case TypeChecker::BS_TOSTR_B : return coerceToString(tn, true, true);
// 			case BS_TOSTR : return coerceToString(tn);
// 			case BS_TOBOOL_L : return coerceToBool(tn, true, false);
// 			case BS_TOBOOL_R : return coerceToBool(tn, false, true);
// 			case BS_TOBOOL_B : return coerceToBool(tn, true, true);
// 			case BS_TOBOOL : return coerceToBool(tn);
// 			case BS_TODBL_L : return coerceToDouble(tn, true, false);
// 			case BS_TOINT_L : return coerceToInt(tn, true, false);
// 			default: return -1;
// 		}
	}
	
	/* ------------  Constructors & Destr. ------------  */
	TypeChecker::TypeChecker(TreeNode *tn) {
		setTNode(tn);
		setEnvs(new StatEnvStack());
		setQres(new StatQResStack());
		sEnvs->pushBinders(DataScheme::dScheme()->bindBaseObjects());
	}
	
	TypeChecker::TypeChecker() {
		tNode = NULL;
		/* init the both static stacks. The data model is available as DataRead::dScheme() */
		setEnvs(new StatEnvStack());
		setQres(new StatQResStack());
		/* set the first section of the ENVS stack with binders to definitions of base objects  */
		sEnvs->pushBinders(DataScheme::dScheme()->bindBaseObjects());
	}
	TypeChecker::~TypeChecker() {
		if (sQres != NULL) delete sQres;
		if (sEnvs != NULL) delete sEnvs;
		if (tNode != NULL) delete tNode;
	}

	/* ------------ end of constructors & desctructors --------------- */
	
	void TCGlobalResult::printOutput() {
		ErrorConsole ec("TypeChecker");
		ec << "TypeChecking result: " + overallResult  + "\n ";
		if (overallResult == "ERROR") {
			ec << "Query type check returned errors: \n";
			cout << "errors size: " << errors.size() << endl;
			for (unsigned int i = 0; i < errors.size(); i++) {
				ec << " -->" + errors[i].getOutput() + " \n";
			}
		}
	}
	
	string TCGlobalResult::getOutput() {
		string ret = "TypeCheck result: " + overallResult + "\n";
		if (overallResult == "ERROR") {
			for (unsigned int i = 0; i < errors.size(); i++) {
				ret += "* " + errors[i].getOutput() + "\n";
			}
		}
		return ret;
	}
	
	int TypeChecker::doTypeChecking(string &tcResultString) {
		
		cout << "I'll do all typeChecking here...:" << endl;
		int result = typeCheck(this->tNode);
		cout << "Finished all typecheck" << endl;
		Deb::ug("Finished all typecheck, result: %d", result);
		globalResult.printOutput();
		if (result != 0 && result != (ErrTypeChecker | ETCNotApplicable)) {
			cout << "setting tcResult to NON ''" << endl;
			//Thats where globalResult should be sensibly serialized, with end-line chars, to let the user
			// see all errors caught while type checking. 
			tcResultString = globalResult.getOutput();
			cout << globalResult.getOutput() << endl;
			return (ErrTypeChecker | EGeneralTCError);
		} else {
			cout << "leaving tcResult THE WAY IT WAS !!! " << endl;
		}
		return result;
	
	}
	/* Checks if param. vector contains at least one valid binder, i.e. if static bind succeeded */
	bool TypeChecker::bindError(vector<BinderWrap*> *vec) {
		return (vec == NULL || vec->size() == 0 || vec->at(0) == NULL || vec->at(0)->getBinder() == NULL);
	}
	
	/**
	 * Recursive type checking method. Goes through query tree in a similar fashion to static eval.
	 */
	int TypeChecker::typeCheck(TreeNode *tn) {
		if (tn == NULL) {
			if (Deb::ugOn()) cout << "NULL TreeNode cannot by typechecked" << endl;
			return -1;
		}
		int nodeType = tn->type();
		int errcode = 0;
		switch (nodeType) {
			case TreeNode::TNNAME: {
				string expandName = "", name = ((NameNode *)tn)->getName();
				vector<BinderWrap*> *vec = new vector<BinderWrap*>();
				sEnvs->bindName2(name, vec);
				if (bindError(vec)) {//BIND ERROR
					sEnvs->bindNameEllipsis(name, vec, expandName);
					if (!bindError(vec) && expandName != "") {
						errcode = augmentTreeEllipsis(tn, name, vec, expandName);
						if (errcode != 0) return errcode;
					}
				}
				if (bindError(vec)) {//ELLIPSIS DOESN'T WORK - report error, try to restore process.
					reportTypeError(TCError(TCError::BNAME, name));
					errcode = restoreAfterMisspelledName(tn, name, vec);
					if (errcode != 0) return errcode;
				}
				Signature *sig = extractSigFromBindVector(vec);
				if (sig == NULL) return (ErrTypeChecker | ENameNotBound);	
				errcode = sQres->pushSig (sig);
				if (errcode != 0) return errcode;
				return 0;
			} //case TNNAME
			case TreeNode::TNINT: {
				errcode = sQres->pushSig(new SigAtomType("int"));
				if (errcode != 0) return errcode;
				return 0;
			} //case TNINT
			case TreeNode::TNSTRING: {
				errcode = sQres->pushSig(new SigAtomType("string"));
				if (errcode != 0) return errcode;
				return 0;
			} //case TNSTRING	
			case TreeNode::TNDOUBLE: {
				errcode = sQres->pushSig(new SigAtomType("double"));
				if (errcode != 0) return errcode;
				return 0;
			} //case TNDOUBLE
			case TreeNode::TNALGOP: {
				AlgOpNode::algOp op = ((AlgOpNode *) tn)->getOp();
				string opStr = ((AlgOpNode *) tn)->opStr();
						if (Deb::ugOn()) cout<<"TypeChecker::typeCheck(ALG: "<< opStr <<")\n";
				Signature *lSig, *rSig; 
				errcode = typeCheck(((AlgOpNode *) tn)->getLArg());
				if (errcode != 0) return errcode;
				errcode = typeCheck(((AlgOpNode *) tn)->getRArg());
				if (errcode != 0) return errcode;
				//pop result signatures in reverted (stack) order...
				rSig = sQres->popSig();
				lSig = sQres->popSig();
				if (lSig == NULL || rSig == NULL) return (ErrTypeChecker | ETCInnerFailure);
				
				if (Deb::ugOn()) { cout << "args: " << lSig->toString() << " & " << rSig->toString() << endl;}
				TypeCheckResult tcRes;
				DecisionTable *dTab = this->getDTable(nodeType, op);
				errcode = dTab->getResult(tcRes, lSig, rSig);
				if (errcode != 0) return errcode;
						if (Deb::ugOn()) cout << "ALGOP:got such tcresult: \n" << tcRes.printAllInfo() << endl;
			
				if (tcRes.isError()) {	//Try to augment by DEREF
					if ((errcode = augmentTreeDeref(tn, dTab, tcRes, lSig, rSig)) != 0) return errcode;
				}
				if (tcRes.isCoerce()) {	//Perform the listed COERCE operations
					if ((errcode = augmentTreeCoerce(tn, lSig, rSig, tcRes)) != 0) return errcode;
				}
				if (tcRes.isError()) {	//Report error and try to restore process
					reportTypeError(TCError(TCError::ARG_ALG, tcRes.getErrorParts(), 
											lSig->toString(), rSig->toString(),opStr));
					errcode = restoreAfterBadArg(tn, dTab, tcRes, lSig, rSig);
					if (errcode != 0) return errcode;
				} 
				if (tcRes.isError() || tcRes.getSig() == NULL) {//Retore failed - return with final error.
					return (ErrTypeChecker | EGeneralTCError);	
				}
				//If all OK, or process restored - push result on sQres and continue typechecking
				errcode = sQres->pushSig(tcRes.getSig()->clone());
				if (errcode != 0) return errcode;
						if (Deb::ugOn()) cout<<"TypeChecker::typeCheck(ALG: "<< opStr <<") DONE.\n";
				return 0;
			}
			case TreeNode::TNNONALGOP: {
				NonAlgOpNode::nonAlgOp op = ((NonAlgOpNode *) tn)->getOp();
				string opStr = ((NonAlgOpNode *) tn)->opStr();
						if (Deb::ugOn()) cout << "TypeChecker::typeCheck(TN NONALG: " << opStr << ")\n";
				Signature *lSig, *rSig;
				//	TYPECHECK	left argument
				errcode = typeCheck(((NonAlgOpNode *) tn)->getLArg());
				if (errcode != 0) return errcode;
				if ((lSig = sQres->topSig()) == NULL) return (ErrTypeChecker | ETCInnerFailure);
				
				openScope(lSig);	Deb::ug("Static Env stack:\n"); if (Deb::ugOn()) sEnvs->putToString();
				//	TYPECHECK	right argument
				errcode = typeCheck(((NonAlgOpNode *) tn)->getRArg());
				if (errcode != 0) return errcode;
				closeScope(lSig);
				
				if ((rSig = sQres->topSig()) == NULL) return (ErrTypeChecker | ETCInnerFailure);
				sQres->pop();	/*pop rSig*/
				sQres->pop();	/*pop lSig*/			
				TypeCheckResult tcRes;
						if (Deb::ugOn()) cout << "-> args: " << lSig->toString() << " & " << rSig->toString() << endl;
				DecisionTable *dTab = this->getDTable(nodeType, op);
				errcode = dTab->getResult(tcRes, lSig, rSig);
				if (errcode != 0) return errcode;
						if (Deb::ugOn()) cout << "NONALG:got such tcresult: \n" << tcRes.printAllInfo() << endl;
				
				if (tcRes.isError()) {	//Try to augment by DEREF
					if ((errcode = augmentTreeDeref(tn, dTab, tcRes, lSig, rSig)) != 0) return errcode;
				}
				if (tcRes.isCoerce()) {	//Perform the listed COERCE operations
					if ((errcode = augmentTreeCoerce(tn, lSig, rSig, tcRes)) != 0) return errcode;
				}
				if (tcRes.isError()) {	//Report error and try to restore process
					reportTypeError(TCError(TCError::ARG_NALG, tcRes.getErrorParts(), 
											lSig->toString(), rSig->toString(),opStr));
					errcode = restoreAfterBadArg(tn, dTab, tcRes, lSig, rSig);
					if (errcode != 0) return errcode;
				} 
				if (tcRes.isError() || tcRes.getSig() == NULL) {//Restore failed - return with final error.
					return (ErrTypeChecker | EGeneralTCError);	
				}
				//If all OK, or process restored - push result on sQres and continue typechecking
				errcode = sQres->pushSig(tcRes.getSig()->clone());
				if (errcode != 0) return errcode;
				if (Deb::ugOn()) cout<<"TypeChecker::typeCheck(ALG: "<< opStr <<") DONE.\n";
				return 0;
			} //case TNNONALGOP
			case TreeNode::TNUNOP: {
				UnOpNode::unOp op = ((UnOpNode *) tn)->getOp();
				string opStr = ((UnOpNode *) tn)->opStr();
				if (Deb::ugOn()) cout << "TypeChecker::typeCheck( UN OP: " << opStr << ")\n";
				Signature *argSig; 
				
				//TYPECHECK argument
				errcode = typeCheck(((UnOpNode *) tn)->getArg());
				if (errcode != 0) return errcode;
				
				argSig = sQres->popSig();
				if (argSig == NULL) return (ErrTypeChecker | ETCInnerFailure);
				TypeCheckResult tcRes;
				UnOpDecisionTable *uDTab = getDTables()->getUnOpDTable(nodeType, op);
				errcode = uDTab->getResult(tcRes, argSig);
				if (errcode != 0) return errcode;
						if (Deb::ugOn()) cout << "UNOP, got such tcresult: \n" << tcRes.printAllInfo() << endl;
				if (tcRes.isError()) {	//Try to augment by DEREF
					if ((errcode = augmentTreeDeref(tn, uDTab, tcRes, argSig)) != 0) return errcode;
				}
				if (tcRes.isCoerce()) {	//perform COERCE operations
					if ((errcode = augmentTreeCoerce(tn, argSig, NULL, tcRes)) != 0) return errcode;
				}
				if (tcRes.isError()) {
					reportTypeError(TCError(TCError::ARG_UNOP, tcRes.getErrorParts(), argSig->toString(), opStr));
					errcode = restoreAfterBadArg(tn, uDTab, tcRes, argSig);
					if (errcode != 0) return errcode;
				}
				if (tcRes.isError() || tcRes.getSig() == NULL) {//Restore failed - return with final error.
					return (ErrTypeChecker | EGeneralTCError);	
				}
				//All went OK, or process restored - push result on sQres and continue typechecking
				errcode = sQres->pushSig(tcRes.getSig()->clone());
				if (errcode != 0) return errcode;
				if (Deb::ugOn()) cout<<"TypeChecker::typeCheck(UN OP: "<< opStr <<") DONE.\n";
				return 0;
			} //case TNUNOP
			case TreeNode::TNAS: {
				string name = ((NameAsNode *)tn)->getName();
				int grouped = (((NameAsNode *)tn)->isGrouped() ? 1 : 0);
				int op = TreeNode::TNAS;
				Deb::ug("TypeChecker::typeCheck(NAMEAS %d)..", grouped);
				Signature *argSig;
				
				//TYPECHECK argument
				errcode = typeCheck(((NameAsNode *)tn)->getArg());
				if (errcode != 0) return errcode;
				
				argSig = sQres->popSig();
				if (argSig == NULL) return (ErrTypeChecker | ETCInnerFailure);
				TypeCheckResult tcRes;
				UnOpDecisionTable *uDTab = getDTables()->getUnOpDTable(nodeType, op);
				errcode = uDTab->getResult(tcRes, argSig, name, grouped);
				Deb::ug("NAME AS OR GrOUP AS - got tcresult with errcode %d.", errcode);
				cout << "TN AS, got such tcresult: \n" << tcRes.printAllInfo() << endl;
				
				if (tcRes.isError()) {	//Try to augment by DEREF
					if ((errcode = augmentTreeDeref(tn, uDTab, tcRes, argSig)) != 0) return errcode;
				}
				if (tcRes.isCoerce()) {	//perform COERCE operations
					if ((errcode = augmentTreeCoerce(tn, argSig, NULL, tcRes)) != 0) return errcode;
				}
				if (tcRes.isError()) {
					reportTypeError(TCError(TCError::ARG_AS, tcRes.getErrorParts(), argSig->toString(), 
											(grouped == 1 ? "group as" : "as"), name));
					errcode = restoreAfterBadArg(tn, uDTab, tcRes, argSig);
					if (errcode != 0) return errcode;
				}
				if (tcRes.isError() || tcRes.getSig() == NULL) {//Restore failed - return with final error.
					return (ErrTypeChecker | EGeneralTCError);	
				}
				//All went OK, or process restored - push result on sQres and continue typechecking
				errcode = sQres->pushSig(tcRes.getSig()->clone());
				if (errcode != 0) return errcode;
				if (Deb::ugOn()) cout<<"TypeChecker::typeCheck(NAME AS: "<< name <<") DONE.\n";
				return 0;
			} //case TNAS ( 'as' or 'group as')
			default: Deb::ug("Tree node not recognised by TC, will not apply TypeChecking."); 
				return (ErrTypeChecker | ETCNotApplicable);
		}
		return 0;
	}
	
	Signature* TypeChecker::extractSigFromBindVector(vector<BinderWrap*> *vec) {
		if (bindError(vec)) return NULL;
		Signature *sig = vec->at(0)->getBinder()->getValue()->clone();
		/* Do not need this struct below - the signature we want is the first element of the vector.
		for (unsigned int i = 1; i < vec->size(); i++){ // dodaje do sygnatury pozostale
			BinderWrap *pombw = vec->at(i);
			if (sig->type() == Signature::SSTRUCT) {
				((SigColl *) sig)->addToMyList (pombw->getBinder()->getValue()->clone());
			} else {
				SigColl *s = new SigColl(Signature::SSTRUCT);
				s->setElts(sig); s->addToMyList(pombw->getBinder()->getValue()->clone());
				sig = s;		
			}
		}*/
		return sig;
	}
	
	int TypeChecker::openScope(Signature *sig) {
		return sEnvs->pushBinders(sig->statNested(NULL));
	}
	
	int TypeChecker::closeScope(Signature *sig) {
		sEnvs->pop();
		return 0;
	}

	
	int TypeChecker::trySingleDeref(Signature *sig, Signature *&sigIn, TypeCheckResult &tmpTcRes, bool &doDeref) {
		int errcode = 0;
		if ((sig != NULL) && (sig->type() == Signature::SBINDER)) sig = ((StatBinder *)sig)->getValue();
		if ((sig != NULL) && (sig->type() == Signature::SREF)) {
			errcode = this->getDTables()->getUnOpDTable(TreeNode::TNUNOP, UnOpNode::deref)->getResult(tmpTcRes, sig);
			if (errcode != 0) return errcode;
			sigIn = tmpTcRes.getSig()->clone();
			doDeref = true;
		} else {
			sigIn = sig;
		}
		return errcode;
	}
	
	int TypeChecker::augmentTreeEllipsis(TreeNode *tn, string name, vector<BinderWrap*> *vec, string expandName) {
		if (Deb::ugOn()) cout << "AUGMENTING(ellipsis), ADDING '." << expandName << "' TO THE QUERY ..." << endl;
		TreeNode *parent = tn->getParent();
		if (parent == NULL) {
			Deb::ug("augmentEllipsis: parent is NULL, so cannot do augment ellipsis.");
			return -1;
		}
		
		TreeNode *dotNode = new NonAlgOpNode(new NameNode(expandName), (QueryNode *) tn, NonAlgOpNode::dot);
		int errcode = parent->swapSon(tn, dotNode);
		//debugs ... 
		if (Deb::ugOn()) {
			TreeNode *ptr = tn;
			while (ptr->getParent() != NULL) ptr = ptr->getParent();
			cout << "Query tree afeter augmentEllipsis: \n"; ptr->serialize(); cout << endl;
		}
		return errcode;
	}
	
	int TypeChecker::augmentTreeCoerce(TreeNode *tn, Signature *lSig, Signature *rSig, TypeCheckResult &tcRes) {
		int actionsNumber = tcRes.actionsNumber();
		Deb::ug("typeChecker::augmentTreeCoerce(): performing %d actions.", actionsNumber);
		for (unsigned int i = actionsNumber; i > 0; i--) {
			performAction(tcRes.getActionAt(i-1), tn, lSig, rSig, tcRes);
		}
		if (Deb::ugOn()) {
			cout << "Tree after coerces:\n"; tn->serialize(); cout << endl;
			tn->putToString(); cout << endl;
		}
		return 0;
	}
	
	int TypeChecker::augmentTreeDeref(TreeNode *tn, DecisionTable *dt, TypeCheckResult &tcRes, Signature *lSig, Signature *rSig) {
		if (((AlgOpNode *) tn)->getOp() == AlgOpNode::refeq) return 0; // No deref for '=='
		Deb::ug("TypeChecker::augmentTreeDeref()...");
		int errcode = 0;
		Signature *lSigIn = NULL, *rSigIn = NULL;
		TypeCheckResult outTcRes, tmpTcResL, tmpTcResR;
		bool derefLeft = false, derefRight = false;
		
		errcode = trySingleDeref(lSig, lSigIn, tmpTcResL, derefLeft);
		if (errcode != 0) return errcode;
		
		errcode = trySingleDeref(rSig, rSigIn, tmpTcResR, derefRight);
		if (errcode != 0) return errcode;
		
		Deb::ug("GOING to second phase of augment Tree deref. with deref params:");
		cout << tmpTcResL.printAllInfo() << endl << tmpTcResR.printAllInfo() << endl;
		
		if (!tmpTcResL.isError() && !tmpTcResR.isError() && lSigIn != NULL && rSigIn != NULL
		   	&& !(lSig == lSigIn && rSig == rSigIn)) {
			Deb::ug("AUGMENT DEREF sec. phase - got these input signatures: ");
			cout << lSigIn->toString() << " & " << rSigIn->toString() << endl;
			errcode = dt->getResult(outTcRes, lSigIn, rSigIn);
			if (errcode != 0) return errcode;
			outTcRes.fill(tcRes);
			cout << "After getResult on dereffed signatures: \n" << outTcRes.printAllInfo() << endl;
			if (!outTcRes.isError() && outTcRes.getSig() != NULL) {	//then modify the tree...
				//outTcRes.fill(tcRes);
				Deb::ug("AUGMENT DEREF - will modify the tree !!! ");
				errcode = tn->augmentTreeDeref(derefLeft, derefRight);
				if (errcode != 0) return errcode;
				if (Deb::ugOn()) {tn->serialize(); cout << endl;}
			}
		}
		Deb::ug("AUGMENT DEREF - ending... ");
		return 0;
	}
	int TypeChecker::augmentTreeDeref(TreeNode *tn, UnOpDecisionTable *dt, TypeCheckResult &tcRes, Signature *sig) {
		Deb::ug("TypeChecker::augmentTreeDeref(UnOpDTable)...");
		return 0;
	}
	
	int TypeChecker::restoreAfterMisspelledName(TreeNode *tn, string name, vector<BinderWrap*> *vec) {
		cout << "restoreAfterMisspelledName CALLED ! for name: " << name << endl;
		//go through the stack and decide which name is 'the closest' to name. If none is really close, 
		// return ECannotRestoreBadName. Else - fill the vector in and return 0;
		return (ErrTypeChecker | ECannotRestoreBadName); //For now - lets presume we can never guess the right name. 
	}
	
	//TODO: should restore take the dt, or just op and nodeType? Will it be a new decisionTable or the same one..?	
	int TypeChecker::restoreAfterBadArg(TreeNode *tn, DecisionTable *dt, TypeCheckResult &tcRes, Signature *lSig, Signature *rSig) {
		cout << "restoreAfterBadArg() called" << endl;
		return (ErrTypeChecker | ECannotRestore);
	}
	
	int TypeChecker::restoreAfterBadArg(TreeNode *tn, UnOpDecisionTable *dt, TypeCheckResult &tcRes, Signature *sig) {
		cout << "restoreAfterBadArg( UN OP ) called" << endl;
		return (ErrTypeChecker | ECannotRestore);
	}
	
}



