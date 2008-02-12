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

	/* augment...() methods, to be pointed to in TCResults.. */
	//TODO: fill these methods below with real content
	int TypeChecker::coerceCardsTo11(TreeNode *tn, Signature *lSig, Signature*rSig) {
		// - modify tn, adding coerceNodes ?
		Deb::ug("coercing cards to 1..1. ");
		return 0;
	}
	
	int TypeChecker::coerceOneCardTo11(TreeNode *tn, Signature *sig) {
		Deb::ug("coercing ONLY ONE card to 1..1 ...");
		return 0;
	}
	
	int TypeChecker::coerceToString(bool leftArg, bool rightArg) {
		if (leftArg) Deb::ug("coercing LEFT sig. to string... ");
		if (rightArg) Deb::ug("coercing RIGHT sig. to string... ");
		return 0;
	}
	
	/* ------------ end of pointered methods --------------- */
	int TypeChecker::performAction(int actionId, TreeNode *tn, Signature *lSig, Signature *rSig, TypeCheckResult *tcr) {
	//TODO: there will be more params to perform(). will it be one set of param types?
	// lets say : some &tcresult, currTreeNode, lSig, rSig? ..? 
		switch (actionId) {
			case TypeChecker::CD_COERCE_11 : 
				return coerceCardsTo11(tn, lSig, rSig);
			case TypeChecker::CD_COERCE_11_L : 
				return coerceOneCardTo11(tn, lSig);
			case TypeChecker::CD_COERCE_11_R : 
				return coerceOneCardTo11(tn, rSig);
			case TypeChecker::BS_TOSTR_L : 
				return coerceToString(true, false);	
			case TypeChecker::BS_TOSTR_R : 
				return coerceToString(false, true);	
			default: return -1;
		}
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
					globalResult.reportTypeError(TCError(TCError::BNAME, name));
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
				if (Deb::ugOn()) cout<<"TypeChecker::typeCheck(ALG: "<<((AlgOpNode *) tn)->opStr()<<")\n";
				AlgOpNode::algOp op = ((AlgOpNode *) tn)->getOp();
				Signature *lSig, *rSig, *resultSig; 
				errcode = typeCheck(((AlgOpNode *) tn)->getLArg());
				if (errcode != 0) return errcode;
				errcode = typeCheck(((AlgOpNode *) tn)->getRArg());
				if (errcode != 0) return errcode;
				//pop result signatures in reverted (stack) order...
				rSig = sQres->popSig();
				lSig = sQres->popSig();
				if (lSig == NULL || rSig == NULL) {	Deb::ug("CANT GET TOP SIG OF sQRES"); 
					return (ErrTypeChecker | ETCInnerFailure);
				}
				if (Deb::ugOn()) { cout << "args: " << lSig->toString() << " & " << rSig->toString() << endl;}
				TypeCheckResult tcRes;
				DecisionTable *dTab = this->getDTable(DTable::ALG, op);
				errcode = dTab->getResult(tcRes, lSig, rSig);
				if (errcode != 0) return errcode;
				cout << "ALGOP:got such tcresult: \n" << tcRes.printAllInfo() << endl;
				if ((tcRes.isError()) && (op != AlgOpNode::refeq)) { //if (op != AlgOpNode::refeq)
					errcode = augmentTreeDeref(tn, dTab, tcRes, lSig, rSig);
					if (errcode != 0) return errcode;
				}
				if (tcRes.isCoerce()) {
					augmentTreeCoerce(tn, lSig, rSig, tcRes);
				}
				if (!tcRes.isError()) {
					resultSig = tcRes.getSig();
	//				if (Still error) {reportTpeError, resultSig = restoreProcessAfterBadArg(oper, lSig, rSig)
	//				then finally goes the 'pushsig' below...
	
					errcode = sQres->pushSig(resultSig->clone());
					if (errcode != 0) return errcode;
					Deb::ug("Algebraic operation TYPECHECKED!");
				} else {
					//tryAugment ? deref first ? 
					reportTypeError(TCError(TCError::ARG_ALG, lSig->toString(), rSig->textType(),((AlgOpNode *) tn)->opStr()));
					//should try and restoreProcess here, and if that fails - return -1 to stop typechecking.
					return -1;
				}
				return 0;
			}
			case TreeNode::TNNONALGOP: {
				if (Deb::ugOn()) cout << "TypeChecker::typeCheck(TN NONALG: " << ((NonAlgOpNode *) tn)->getOp() << ")\n";
				NonAlgOpNode::nonAlgOp op = ((NonAlgOpNode *) tn)->getOp();
				cout << "operator: |" << ((NonAlgOpNode *) tn)->opStr() << "| " << endl;
				Signature *lSig, *rSig, *resultSig;
				errcode = typeCheck(((NonAlgOpNode *) tn)->getLArg());
				if (errcode != 0) return errcode;
				lSig = sQres->topSig();
				if (lSig == NULL) {Deb::ug("CANT GET TOP SIG OF sQRES"); return -1;}
				Deb::ug("TC: finished LEFT ARG OF NONALGOPNODE, will push nested results on stack");
				((NonAlgOpNode *) tn)->setFirstOpenSect(1 + sEnvs->getSize());
				int sToPop = 0;	//nr of sections put on sEnvs, at the end must pop them all off the stack
				if (lSig->isColl()) {
					cout << "pushing from SigColl\n";
					BinderWrap *bindersColl = lSig->statNested(lSig->getDependsOn());
					sToPop++;
					if (sEnvs->pushBinders(bindersColl) != 0) {
						cout << "ERROR ! typeCheck for NonAlgOpNode, sigcoll \n" <<endl;
					}
				} else 	{			
					if (sEnvs->pushBinders(lSig->statNested(lSig->getDependsOn())) == 0) {
						sToPop = 1; 
					} else  {
						sToPop = 0;
					}
				}
				Deb::ug("pushed static nested result on envs, now envs looks like this: \n");
				sEnvs->putToString();
				((NonAlgOpNode *) tn)->setLastOpenSect(sEnvs->getSize());
				errcode = typeCheck(((NonAlgOpNode *) tn)->getRArg());
				if (errcode != 0) return errcode;
				rSig = sQres->topSig();
				if (rSig == NULL) {Deb::ug("CANT GET TOP SIG OF sQRES"); return -1;}
				Deb::ug("TC: finished _CHILDREN_ of RIGHT ARG OF NONALGOPNODE, will pop %d sections off sEnvs", sToPop);
				for (;sToPop > 0; sToPop--){sEnvs->pop();}
				sQres->pop();	/*pop rSig*/
				sQres->pop();	/*pop lSig*/			
				TypeCheckResult tcRes;
				cout << "LSIG: " << endl; lSig->putToString();
				cout << "RSIG: " << endl; rSig->putToString();
				errcode = this->getDTable(DTable::NONALG, op)->getResult(tcRes, lSig, rSig);
				Deb::ug("NONALGOP: got tcresult with errcode: %d .", errcode);
				cout << "NONALG: got such tcresult: \n" << tcRes.printAllInfo() << endl;
				if (!tcRes.isError()) {
					resultSig = tcRes.getSig();
					errcode = sQres->pushSig(resultSig->clone());
					if (errcode != 0) return errcode;
					Deb::ug("NONAlgebraic operation TYPECHECKED!");
				} else {
					reportTypeError(TCError(TCError::ARG_NALG,lSig->toString(),rSig->toString(),((NonAlgOpNode *) tn)->opStr()));
					//should try and restoreProcess here, and if that fails - return -1 to stop typechecking.
					return -1;
				}
				return 0;
			} //case TNNONALGOP
			case TreeNode::TNUNOP: {
				UnOpNode::unOp op = ((UnOpNode *) tn)->getOp();
				Deb::ug(" UNARY operator - type recognized");
				Signature *argSig, *resultSig; 
				errcode = typeCheck(((UnOpNode *) tn)->getArg());
				if (errcode != 0) return errcode;
				argSig = sQres->popSig();
				if (argSig == NULL) {Deb::ug("TC, UNOP: CANT GET TOP SIG OF sQRES"); return -1;}
				TypeCheckResult tcRes;
				errcode = this->getDTables()->getUnOpDTable(op)->getResult(tcRes, argSig);
				Deb::ug("UNARY OP: got tcresult with errcode: %d .", errcode);
				cout << "UNOP, got such tcresult: \n" << tcRes.printAllInfo() << endl;
				if (!tcRes.isError()) {
					resultSig = tcRes.getSig();
					errcode = sQres->pushSig(resultSig->clone());
					if (errcode != 0) return errcode;
					Deb::ug("UN - OP operation TYPECHECKED!");
				} else {
					reportTypeError(TCError(TCError::ARG_UNOP, argSig->toString(), ((UnOpNode *) tn)->opStr()));
					//should try and restoreProcess here, and if that fails - return -1 to stop typechecking.
					return -1;
				}
				return 0;
			} //case TNUNOP
			case TreeNode::TNAS: {
				string name = ((NameAsNode *)tn)->getName();
				int grouped = (((NameAsNode *)tn)->isGrouped() ? 1 : 0);
				int op = TreeNode::TNAS;
				op = (-1) * op;
				Deb::ug("NAMEAS: doing typecheck..");
				Signature *argSig, *resultSig;
				errcode = typeCheck(((NameAsNode *)tn)->getArg());
				if (errcode != 0) return errcode;
				argSig = sQres->popSig();
				if (argSig == NULL) {Deb::ug("TC, NAMEAS: CANT GET TOP SIG OF sQRES"); return -1;}
				TypeCheckResult tcRes;
				errcode = this->getDTables()->getUnOpDTable(op, name, grouped)->getResult(tcRes, argSig);
				Deb::ug("NAME AS OR GrOUP AS - got tcresult with errcode %d.", errcode);
				cout << "TN AS, got such tcresult: \n" << tcRes.printAllInfo() << endl;
				if (!tcRes.isError()) {
					resultSig = tcRes.getSig();
					errcode = sQres->pushSig(resultSig->clone());
					if (errcode != 0) return errcode;
					Deb::ug("NAMEAS operation TYPECHECKED!");
				} else {
					reportTypeError(TCError(TCError::ARG_AS, argSig->toString(), (grouped == 1 ? "group as" : "as")));
					return -1;
				}
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
	
	int TypeChecker::trySingleDeref(Signature *sig, Signature *sigIn, TypeCheckResult &tmpTcRes, bool &doDeref) {
		int errcode = 0;
		if ((sig != NULL) && (sig->type() == Signature::SREF)) {
			errcode = this->getDTables()->getUnOpDTable(UnOpNode::deref)->getResult(tmpTcRes, sig);
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
		Deb::ug("TC :: WOULD DO AUGMENT TREE COERCE HERE... i.e. do  the actions suggested.");
		//for( actions in tcRes) {
		//	performAction(), reportCoerceAction(what action, dynCtrl,...) 
		//}
		return 0;
	}
	
	int TypeChecker::augmentTreeDeref(TreeNode *tn, DecisionTable *dt, TypeCheckResult &tcRes, Signature *lSig, Signature *rSig) {
		if (((AlgOpNode *) tn)->getOp() == AlgOpNode::refeq) return 0; // No deref for '=='
		Deb::ug("TypeChecker::augmentTreeDeref()...");
		int errcode = 0;
		Signature *lSigIn, *rSigIn;
		TypeCheckResult outTcRes, tmpTcResL, tmpTcResR;
		bool derefLeft = false, derefRight = false;
		
		errcode = trySingleDeref(lSig, lSigIn, tmpTcResL, derefLeft);
		if (errcode != 0) return errcode;
		
		errcode = trySingleDeref(rSig, rSigIn, tmpTcResR, derefRight);
		if (errcode != 0) return errcode;
		
		Deb::ug("GOING to second phase of augment Tree deref. with deref params:");
		cout << tmpTcResL.printAllInfo() << endl << tmpTcResR.printAllInfo() << endl;
		
		if (!tmpTcResL.isError() && !tmpTcResR.isError() && lSigIn != NULL && rSigIn != NULL) {
			Deb::ug("AUGMENT DEREF sec. phase - got these input signatures: ");
			cout << lSigIn->toString() << " & " << rSigIn->toString() << endl;
			tcRes.clear();
			errcode = dt->getResult(tcRes, lSigIn, rSigIn);
			if (errcode != 0) return errcode;
			cout << "After getResult on dereffed signatures: \n" << tcRes.printAllInfo() << endl;
			if (!tcRes.isError() && tcRes.getSig() != NULL) {	//then modify the tree...
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
	
}



