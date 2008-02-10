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

	/** augment...() methods, to be pointed to in TCResults.. */
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
	
	/** ------------ end of pointered methods --------------- */
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
				return coerceToString(true, false);	//true: left argument
			case TypeChecker::BS_TOSTR_R : 
				return coerceToString(false, true);	//
			default: return -1;
		}
	}
	
	/** #################### Constructors & Destr. ####################... */
/*	TypeChecker::TypeChecker(TreeNode *tn) {
		setTNode(tn);
		setEnvs(new StatEnvStack());
		setQres(new StatQResStack());
		sEnvs->pushBinders(DataScheme::dScheme()->bindBaseObjects());
	}
*/	
	TypeChecker::TypeChecker() {
		tNode = NULL;
		/* init the both static stacks. The data model is available as DataRead::dScheme() */
		setEnvs(new StatEnvStack());
		setQres(new StatQResStack());
		/* set the first section of the ENVS stack with binders to *
			* definitions of base (i.e. root) objects ...             */
		sEnvs->pushBinders(DataScheme::dScheme()->bindBaseObjects());
	}
	TypeChecker::~TypeChecker() {
		if (sQres != NULL) delete sQres;
		if (sEnvs != NULL) delete sEnvs;
		if (tNode != NULL) delete tNode;
	}

	/** ------------ end of constructors & desctructors --------------- */
	
	void TCGlobalResult::printOutput() {
		ErrorConsole ec("TypeChecker");
		ec << "TypeChecking result: " + overallResult  + "\n ";
		if (overallResult == "ERROR") {
			ec << "Query type check returned errors: \n";
			for (unsigned int i = 0; i < errors.size(); i++) {
				ec << errors[i].getOutput() + ", \n";
			}
		}
	}
	
	int TypeChecker::doTypeChecking(string &tcResultString) {
		
		cout << "I'll do all typeChecking here...:" << endl;
		int result = typeCheck(this->tNode);
		cout << "Finished all typecheck" << endl;
		Deb::ug("Finished all typecheck, result: %d", result);
		globalResult.printOutput();
		if (result != 0 && result != (ErrTypeChecker | ETCNotApplicable)) {
			cout << "setting tcResult to non ''" << endl;
			//Thats where globalResult should be sensibly serialized, with end-line chars, to let the user
			// see all errors caught while type checking. 
			tcResultString = "This is the result of your \n type check procedure... ";
			tcResultString += "\n	let it be stupid lines for now, ok?\n";
			return (ErrTypeChecker | EGeneralTCError);
		} else {
			cout << "leaving tcResult THE WAY IT WAS !!! " << endl;
		}
		return result;
	
	}
	/**
	 * Checks if param. vector contains at least one valid binder, i.e. if static bind succeeded
	 */
	bool TypeChecker::bindError(vector<BinderWrap*> *vec) {
		return (vec == NULL || vec->size() == 0 || vec->at(0) == NULL || vec->at(0)->getBinder() == NULL);
	}
	
	/**
	 * Recursive type checking method. Goes through query tree in a similar fashion to static eval.
	 */
	int TypeChecker::typeCheck(TreeNode *tn) {
		if (tn == NULL) {
			if (Deb::ugOn()) cout << " tn is null, nth to typecheck" << endl;
			return -1;
		}
		int nodeType = tn->type();
		int errcode = 0;
		switch (nodeType) {
			case TreeNode::TNNAME: {
				string name = ((NameNode *)tn)->getName();
				string expandName = "";
										if (Deb::ugOn()) cout << "       TTNNAAMMEE in typecheck:	" << name << endl;
				vector<BinderWrap*> *vec = new vector<BinderWrap*>();
				sEnvs->bindName2(name, vec);
				if (bindError(vec)) {//BIND ERROR
					Deb::ug("typeCheck static bind - NAME NOT FOUND - will try ellipsis");
					sEnvs->bindNameEllipsis(name, vec, expandName);
					cout << "AFTER bindNameEllipsis: name:"<< name << ", expandName:"<<expandName<<endl;
					if (!bindError(vec) && expandName != "") {
						errcode = augmentTreeEllipsis(tn, name, vec, expandName);
						if (errcode != 0) return errcode;
					}
				}
				if (bindError(vec)) {//ELLIPSIS DOESN'T WORK
					Deb::ug("typeCheck will report name error - ellipsis didn't work out.");
					globalResult.reportTypeError(TCError("BAD_NAME", "Name: '"+name+"' was not found."));
					errcode = restoreAfterMisspelledName(tn, name, vec);
					if (errcode != 0) return errcode;
				}
				Signature *sig = extractSigFromBindVector(vec);
				if (sig == NULL) return (ErrTypeChecker | ENameNotBound);
				errcode = sQres->pushSig (sig);
				if (errcode != 0) return errcode;
				Deb::ug("pushed some bound signature on sQres..\n");
				return 0;
			} //case TNNAME
			case TreeNode::TNINT: {
				errcode = sQres->pushSig(new SigAtomType("int"));
				if (errcode != 0) return errcode;
				Deb::ug("pushed new INT sig on sQres");
				return 0;
			} //case TNINT
			case TreeNode::TNSTRING: {
				errcode = sQres->pushSig(new SigAtomType("string"));
				if (errcode != 0) return errcode;
				Deb::ug("pushed new String sig on sQres");
				return 0;
			} //case TNSTRING
	
			case TreeNode::TNDOUBLE: {
				errcode = sQres->pushSig(new SigAtomType("double"));
				if (errcode != 0) return errcode;
				Deb::ug("pushed new Double sig on sQres");
				return 0;
			} //case TNDOUBLE
			case TreeNode::TNALGOP: {
				AlgOpNode::algOp op = ((AlgOpNode *) tn)->getOp();
				Deb::ug(" Algebraic operator - type recognized");
				//QueryResult *lResult, *rResult;
				Signature *lSig, *rSig, *resultSig; 
				errcode = typeCheck(((AlgOpNode *) tn)->getLArg());
				if (errcode != 0) return errcode;
				errcode = typeCheck(((AlgOpNode *) tn)->getRArg());
				if (errcode != 0) return errcode;
				sQres->putToString();
				//pop result signatures in reverted (stack) order...
				cout << "qres size: " << sQres->getSize() << endl;
				rSig = sQres->popSig();
				lSig = sQres->popSig();
				if (lSig == NULL || rSig == NULL) {Deb::ug("CANT GET TOP SIG OF sQRES"); return -1;}
				lSig->putToString();
				rSig->putToString();
				cout << "qres size after 2 pops: " << sQres->getSize() << endl;				
				TypeCheckResult tcRes;
				
				errcode = this->getDTable("ALG", op)->getResult(tcRes, lSig, rSig);
				
				Deb::ug("got tcresult with errcode: %d .", errcode);
				cout << "got such tcresult: " << tcRes.printAllInfo() << endl;
				//Deb::ug("got such tcresult: %s .", tcRes.printAllInfo()); 
				if ((tcRes.isError()) && (op != AlgOpNode::refeq)) { //if (op != AlgOpNode::refeq)
					errcode = augmentTreeDeref(tn, "ALG", op, tcRes, lSig, rSig);
					if (errcode != 0) return errcode;
					cout << "AFTER augmentTreeDeref: tn: "<< endl;
					tn->putToString();
				}
				if (tcRes.isCoerce()) {
					Deb::ug("TC :: WOULD DO AUGMENT TREE COERCE HERE... i.e. do one of the actions suggested.");
				}
				if (!tcRes.isError()) {
					resultSig = tcRes.getSig();
	//				Here are things like:
	//				if (ERROR) augmentTreeDeref(...)
	//				if (COERCE) augmentTreeCoerce(...)
	//				if (Still error) {reportTpeError, resultSig = restoreProcessAfterBadArg(oper, lSig, rSig)
	//				then finally goes the 'pushsig' below...
	
					errcode = sQres->pushSig(resultSig->clone());
					if (errcode != 0) return errcode;
					Deb::ug("Algebraic operation TYPECHECKED!");
				} else {
					//tryAugment ? deref first ? 

					reportTypeError(TCError("BAD_ARG", "Args: ("+lSig->textType()+", "+rSig->textType()+") do not match for ALG operator: "+((AlgOpNode *) tn)->opStr()));
					//should try and restoreProcess here, and if that fails - return -1 to stop typechecking.
					return -1;
	//reportTypeError(TCError("BAD_ARG", lSig->textType(), rSig->textType(), ((AlgOpNode *) tn)->opStr()));
				}
				return 0;
			}
			case TreeNode::TNNONALGOP: {
				NonAlgOpNode::nonAlgOp op = ((NonAlgOpNode *) tn)->getOp();
				Deb::ug("Trying to handle NON alg op node !!! ");
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
				errcode = this->getDTable("NONALG", op)->getResult(tcRes, lSig, rSig);
				Deb::ug("NONALGOP: got tcresult with errcode: %d .", errcode);
				cout << "got such tcresult: " << tcRes.printAllInfo() << endl;
				if (!tcRes.isError()) {
					resultSig = tcRes.getSig();
					errcode = sQres->pushSig(resultSig->clone());
					if (errcode != 0) return errcode;
					Deb::ug("NONAlgebraic operation TYPECHECKED!");
				} else {
					reportTypeError(TCError("BAD_ARG", "Args: ("+lSig->textType()+", "+rSig->textType()+") do not match for operator: "+((NonAlgOpNode *) tn)->opStr()));
					//should try and restoreProcess here, and if that fails - return -1 to stop typechecking.
					return -1;
	//reportTypeError(TCError("BAD_ARG", lSig->textType(), rSig->textType(), ((AlgOpNode *) tn)->opStr()));
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
				cout << "got such tcresult: " << tcRes.printAllInfo() << endl;
				//Deb::ug("got such tcresult: %s .", tcRes.printAllInfo()); 
				if (!tcRes.isError()) {
					resultSig = tcRes.getSig();
					errcode = sQres->pushSig(resultSig->clone());
					if (errcode != 0) return errcode;
					Deb::ug("UN - OP operation TYPECHECKED!");
				} else {
					reportTypeError(TCError("BAD_ARG", "Arg: ("+argSig->textType()+") does not match for UNop: "+((UnOpNode *) tn)->opStr()));
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
				cout << "got such tcresult: " << tcRes.printAllInfo() << endl;
				if (!tcRes.isError()) {
					resultSig = tcRes.getSig();
					errcode = sQres->pushSig(resultSig->clone());
					if (errcode != 0) return errcode;
					Deb::ug("NAMEAS operation TYPECHECKED!");
				} else {
					reportTypeError(TCError("BAD_ARG", "Arg: ("+argSig->textType()+") does not match for AS/GROUP AS"));
					return -1;
				}
				return 0;
			} //case TNAS ( 'as' or 'group as')
			default: Deb::ug("Dont know such tree node, so will not apply type checking.."); 
				return (ErrTypeChecker | ETCNotApplicable);
		}
		return 0;
	}
	
	Signature* TypeChecker::extractSigFromBindVector(vector<BinderWrap*> *vec) {
		if (bindError(vec)) return NULL;
		Signature *sig = vec->at(0)->getBinder()->getValue()->clone();
		Deb::ug("binded vector size: "); cout << vec->size() << endl;
		for (unsigned int i = 1; i < vec->size(); i++){ // dodaje do sygnatury pozostale
			BinderWrap *pombw = vec->at(i);
			if (sig->type() == Signature::SSTRUCT) {
				((SigColl *) sig)->addToMyList (pombw->getBinder()->getValue()->clone());
			} else {
				SigColl *s = new SigColl(Signature::SSTRUCT);
				s->setElts(sig); s->addToMyList(pombw->getBinder()->getValue()->clone());
				sig = s;		
			}
		}
		cout << "Signature extracted from binding vector : ";
		sig->putToString();
		cout << endl << "Signature type: " << sig->textType() << endl;
		return sig;
	}
	
	int TypeChecker::augmentTreeEllipsis(TreeNode *tn, string name, vector<BinderWrap*> *vec, string expandName) {
		cout << "AUGMENTING(ellipsis), ADDING '." << expandName << "' TO THE QUERY ..." << endl;
		TreeNode *parent = tn->getParent();
		if (parent == NULL) {
			Deb::ug("augmentEllipsis: parent is NULL, so... will not perform the ellipsis.. ? ");
			return -1;
		}
		
		TreeNode *dotNode = new NonAlgOpNode(new NameNode(expandName), (QueryNode *) tn, NonAlgOpNode::dot);
		int errcode = parent->swapSon(tn, dotNode);
		//debugs ... 
		cout << "performed all augmentEllipsis. Now lets see the whole tree.." << endl;
		TreeNode *ptr = tn;
		while (ptr->getParent() != NULL) {
			ptr = ptr->getParent();
		}
		cout << endl << endl << endl;
		cout << "serialize(): " << endl;
		ptr->serialize();
		cout << endl;
		return errcode;
	}
	
	int TypeChecker::augmentTreeCoerce(TreeNode *tn, Signature *lSig, Signature *rSig, TypeCheckResult &tcRes) {
		return 0;
	}
	
	int TypeChecker::augmentTreeDeref(TreeNode *tn, string opType, int oper, TypeCheckResult &tcRes, Signature *lSig, Signature *rSig) {
//							  augmentTreeDeref(tn, "NONALG", op, tcRes, lSig, rSig);
		Deb::ug("Trying to do augment tree dereference...");
		int errcode = 0;
		Signature *resultSig = NULL, *lSigIn, *rSigIn;
		TypeCheckResult outTcRes, tmpTcResL, tmpTcResR;
		bool derefLeft = false, derefRight = false;
		if ((lSig != NULL) && (lSig->type() == Signature::SREF)) {
			errcode = this->getDTables()->getUnOpDTable(UnOpNode::deref)->getResult(tmpTcResL, lSig);
			if (errcode != 0) return errcode;
			lSigIn = tmpTcResL.getSig()->clone();
			derefLeft = true;
		} else {
			lSigIn = lSig;
		}
		Deb::ug("augmentDeref: done with left sig");
		if ((rSig != NULL) && (rSig->type() == Signature::SREF)) {
			errcode = this->getDTables()->getUnOpDTable(UnOpNode::deref)->getResult(tmpTcResR, rSig);
			if (errcode != 0) return errcode;
			rSigIn = tmpTcResR.getSig()->clone();
			derefRight = true;
		} else {
			rSigIn = rSig;
		}
		Deb::ug("GOING to second phase of augment Tree deref. with tree: ");
		tn->putToString();
		cout << endl;
		Deb::ug("my params...(tcresL, tcresR, lSig, rSig)");
		cout << tmpTcResL.printAllInfo();
		cout << endl;
		cout << tmpTcResR.printAllInfo();
		cout << endl;
		if (lSigIn != NULL) lSigIn->putToString(); else cout << "lSigIn -- NULL ";
		cout << endl;
		if (rSigIn != NULL) rSigIn->putToString(); else cout << "rSigIn -- NULL ";
		cout << endl;
		if (!tmpTcResL.isError() && !tmpTcResR.isError() && lSigIn != NULL && rSigIn != NULL) {
			Deb::ug("AUGMENT DEREF - got these input signatures: ");
			lSigIn->putToString();
			rSigIn->putToString();
			tcRes.clear();
			errcode = this->getDTable(opType, oper)->getResult(tcRes, lSigIn, rSigIn);
			if (errcode != 0) return errcode;
			cout << "after repeating getResult on dereffed signatures: " << endl;
			cout << tcRes.printAllInfo() << endl;
			if (!tcRes.isError() && tcRes.getSig() != NULL) {	//then modify the tree...
				Deb::ug("AUGMENT DEREF - will modify the tree !!! ");
				errcode = tn->augmentTreeDeref(derefLeft, derefRight);
				if (errcode != 0) return errcode;
			}
			Deb::ug("AUGMENT DEREF - ending... ");
		}
		return 0;
//		errcode = this->getDTable(opType, op)->getResult(tcRes, lSig, rSig);
//		errcode = this->getDTables().getUnOpDTable(UnOpNode::deref)->getResult(tcRes, sig);
		
	}
	
	
	int TypeChecker::restoreAfterMisspelledName(TreeNode *tn, string name, vector<BinderWrap*> *vec) {
		cout << "restoreAfterMisspelledName CALLED ! for name: " << name << endl;
		//go through the stack and decide which name is 'the closest' to name. If none is really close, 
		// return sth other than zero. Else - fill the vector in and return 0;
		return (ErrTypeChecker | ECannotRestoreBadName); //For now - lets presume we can never guess the right name. 
	}
	
}



