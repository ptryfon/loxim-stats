#include <TypeCheck/TypeChecker.h>
#include <TypeCheck/DecisionTable.h>
#include <TypeCheck/Rule.h>
#include <TypeCheck/TypeCheckResult.h>

#include <Errors/ErrorConsole.h>
#include <Errors/Errors.h>
#include <QueryExecutor/QueryResult.h>
#include <QueryParser/Stack.h>
#include <QueryParser/TreeNode.h>
#include <QueryParser/DataRead.h>
#include <QueryParser/Deb.h>

#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>



using namespace Errors;
using namespace QParser;
using namespace std;
using namespace TypeCheckConstants;


namespace TypeCheck
{
	/* ------------ coerce methods --------------- */

	//	coerce types: to_string, to_double, to_bool, element, to_bag, to_seq

	int TypeChecker::modifyTreeCoerce(int coerceType, TreeNode *tn, bool augLeft, bool augRight) {
		return tn->augmentTreeCoerce(coerceType, augLeft, augRight);
	}
	int TypeChecker::modifyTreeCoerce(int coerceType, TreeNode *tn) {
		return tn->augmentTreeCoerce(coerceType);
	}
	/* ------------ end of coerce methods --------------- */

	int TypeChecker::performAction(ActionStruct action, TreeNode *tn, Signature *coerceSig) {

		if (action.arg == DTable::MARK_NODE) return performMarkAction(action.id, tn, coerceSig);
		if (action.arg == DTable::SINGLE) return performSingleArgAction(action.id, tn, coerceSig);

		bool augLeft = (action.arg == DTable::LEFT || action.arg == DTable::BOTH);
		bool augRight = (action.arg == DTable::RIGHT || action.arg == DTable::BOTH);
		int coerceAction = 0;
		switch (action.id) {//Now this simply maps one enum to the other, but has be so in case new ops come
			case DTable::CARD_TO_11 : coerceAction = CoerceNode::element; break;
			case DTable::BS_TO_STR : coerceAction = CoerceNode::to_string; break;
			case DTable::BS_TO_BOOL : coerceAction = CoerceNode::to_bool; break;
			case DTable::BS_TO_DBL : coerceAction = CoerceNode::to_double; break;
			case DTable::BS_TO_INT : coerceAction = CoerceNode::to_int; break;
			case DTable::CK_TO_BAG : coerceAction = CoerceNode::to_bag; break;
			case DTable::CK_TO_SEQ : coerceAction = CoerceNode::to_seq; break;
			default: return -1;
		}
		return modifyTreeCoerce(coerceAction, tn, augLeft, augRight);
	}

	int TypeChecker::performSingleArgAction(int actionId, TreeNode *tn, Signature *coerceSig) {

		switch (actionId) {
			case DTable::CARD_TO_11 : return modifyTreeCoerce(CoerceNode::element, tn);
			case DTable::BS_TO_STR : return modifyTreeCoerce(CoerceNode::to_string, tn);
			case DTable::BS_TO_BOOL : return modifyTreeCoerce(CoerceNode::to_bool, tn);
			case DTable::BS_TO_DBL : return modifyTreeCoerce(CoerceNode::to_double, tn);
			case DTable::BS_TO_INT : return modifyTreeCoerce(CoerceNode::to_int, tn);
			case DTable::CK_TO_BAG : return modifyTreeCoerce(CoerceNode::to_bag, tn);
			case DTable::CK_TO_SEQ : return modifyTreeCoerce(CoerceNode::to_seq, tn);
			case DTable::CD_CAN_DEL: return modifyTreeCoerce(CoerceNode::can_del, tn);
			//case DTable::CD_CAN_CRT: return modifyTreeCoerce(CoerceNode::can_crt, tn); //now done by markAction.
			case DTable::CD_EXT_CRT: return modifyTreeCoerce(CoerceNode::ext_crt, tn);
			default: return -1;
		}
	}

	int TypeChecker::performMarkAction(int actionId, TreeNode *tn, Signature *coerceSig) {
		return tn->markTreeCoerce(actionId, coerceSig);
	}

	/* ------------  Constructors & Destr. ------------  */
	TypeChecker::TypeChecker(TreeNode *tn) {
		setTNode(tn);
		setEnvs(new StatEnvStack());
		setQres(new StatQResStack());
		sEnvs->pushBinders(DataScheme::dScheme(-1)->bindBaseObjects());

		//change here or in restore() methods for a different set of restoreProcess algorithms.
		restoreAlgorithm = new DTCRestoreAlgorithm(sQres, sEnvs);
	}

	TypeChecker::TypeChecker() {
		tNode = NULL;
		/* init the both static stacks. The data model is available as DataRead::dScheme() */
		setEnvs(new StatEnvStack());
		setQres(new StatQResStack());
		/* set the first section of the ENVS stack with binders to definitions of base objects  */
		sEnvs->pushBinders(DataScheme::dScheme(-1)->bindBaseObjects());
		//change here or in restore() methods for a different set of restoreProcess algorithms.
		restoreAlgorithm = new DTCRestoreAlgorithm(sQres, sEnvs);
	}
	TypeChecker::~TypeChecker() {
		if (sQres != NULL) delete sQres;
		if (sEnvs != NULL) delete sEnvs;
		if (tNode != NULL) delete tNode;
	}

	/* ------------ end of constructors & desctructors --------------- */

	int TypeChecker::doTypeChecking(string &tcResultString) {

		cout << "I'll do all typeChecking here...:" << endl;
		int result = typeCheck(this->tNode);
		cout << "Finished all typecheck" << endl;
		Deb::ug("Finished all typecheck, result: %d", result);
		globalResult.printOutput();
		if ((result != 0 && result != (ErrTypeChecker | ETCNotApplicable)) ||
			(result == 0 && globalResult.isError())) {
			cout << "setting tcResult to NON ''" << endl;
			//Thats where globalResult should be sensibly serialized, with end-line chars, to let the user
			// see all errors caught while type checking.
			if (not globalResult.isError()) tcResultString = SBQLstrerror(result);
			else tcResultString = globalResult.getOutput();
			cout << tcResultString << endl;
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
					if (bindError(vec)) return (ErrTypeChecker | ENameNotBound);
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

				errcode = typeCheck(((AlgOpNode *) tn)->getLArg());
				if (errcode != 0) return errcode;

				errcode = typeCheck(((AlgOpNode *) tn)->getRArg());
				if (errcode != 0) return errcode;
				//pop result signatures in reverted (stack) order...
				Signature *rSig = sQres->popSig();
				Signature *lSig = sQres->popSig();
				//imperative operators: make sure left sig isn't aut. derefed:
				if (op == AlgOpNode::insert || op == AlgOpNode::assign) lSig->setRefed();

				return processAugmentDerefCoerceRestore(nodeType, op, opStr, lSig, rSig, tn);
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

				return processAugmentDerefCoerceRestore(nodeType, op, opStr, lSig, rSig, tn);
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

				return processAugmentDerefCoerceRestoreUnOp(nodeType, TCError::ARG_UNOP, op, opStr, argSig, tn);

			} //case TNUNOP
			case TreeNode::TNAS: {
				string name = ((NameAsNode *)tn)->getName();
				int grouped = (((NameAsNode *)tn)->isGrouped() ? 1 : 0);
				int op = TreeNode::TNAS;
				string opStr = (grouped == 1 ? "group as" : "as");
				Deb::ug("TypeChecker::typeCheck(NAMEAS %d)..", grouped);
				Signature *argSig;

				//TYPECHECK argument
				errcode = typeCheck(((NameAsNode *)tn)->getArg());
				if (errcode != 0) return errcode;

				argSig = sQres->popSig();

				return processAugmentDerefCoerceRestoreUnOp(nodeType, TCError::ARG_AS, op, opStr, argSig, tn, name, grouped, true);
			} //case TNAS ( 'as' or 'group as')
			case TreeNode::TNCASTTO: {
				Deb::ug("TypeChecker::typeCheck( TN CAST TO !)");
				string opStr = "cast(X to Y)";
				errcode = typeCheck(((SimpleCastNode *) tn)->getArg());
				if (errcode != 0) return errcode;
				Signature *lSig = sQres->popSig();
				cout << "poped lSig\n";
				cout << "popped lSig in CASTTO: " << lSig->toString() << " and now to createSignature..>\n";
				Signature *rSig = ((SimpleCastNode *) tn)->createSignature();
				if (rSig == NULL) {
					reportTypeError(TCError(TCError::ARG_CASTTO, ((SimpleCastNode *)tn)->getSig()->deParse()));
					cout << "rSig null" << endl;
					return (ErrTypeChecker | ETCInnerFailure);
				}
				else {
					cout << "CASTTO SIGS: " << lSig->toString() <<", " << rSig->toString() <<endl;
				}
				return processAugmentDerefCoerceRestore(nodeType, 0, opStr, lSig, rSig, tn);
			}
			case TreeNode::TNCREATE: {
				if (((CreateNode *) tn)->isClassMember()) return (ErrTypeChecker | ETCNotApplicable);
				Deb::ug("TypeChecker::typeCheck( TN CREATE )");
				string opStr = "create";
				errcode = typeCheck(((CreateNode *) tn)->getArg());
				if (errcode != 0) return errcode;
				Signature *argSig = sQres->popSig();
				string name = "";
				if (argSig->type() == Signature::SBINDER) name = ((StatBinder *)argSig)->getName();
				return processAugmentDerefCoerceRestoreUnOp(nodeType, TCError::CREATE, nodeType, opStr, argSig, tn, name, 0, true); //false: do not deref
				break;
			}
			default: Deb::ug("Tree node not recognised by TC, will not apply TypeChecking.");
				return (ErrTypeChecker | ETCNotApplicable);
		}
		return 0;
	}

	int TypeChecker::processAugmentDerefCoerceRestore(int nodeType, int op, string opStr, Signature *lSig, Signature *rSig, TreeNode *tn) {
		int errcode = 0;
		if (lSig == NULL || rSig == NULL) return (ErrTypeChecker | ETCInnerFailure);
		if (Deb::ugOn()) { cout << "args: " << lSig->toString() << " & " << rSig->toString() << endl;}
		TypeCheckResult tcRes;
		DecisionTable *dTab = this->getDTable(nodeType, op);
		errcode = dTab->getResult(tcRes, lSig, rSig);
		if (errcode != 0) return errcode;
		if (Deb::ugOn()) cout << "OP: " << opStr << " got such tcresult: \n" << tcRes.printAllInfo() << endl;
		Signature *lSigDf = NULL;
		Signature *rSigDf = NULL;
		if (tcRes.isError()) {	//Try to augment by DEREF
			if ((errcode = augmentTreeDeref(tn, dTab, tcRes, lSig, rSig, lSigDf, rSigDf)) != 0) return errcode;
		}
		if (lSigDf == NULL) lSigDf = lSig;
		if (rSigDf == NULL) rSigDf = rSig;
		if (tcRes.isCoerce()) {	//Perform the listed COERCE operations
			if ((errcode = augmentTreeCoerce(tn, lSig, rSig, tcRes)) != 0) return errcode;
		}
		if (tcRes.isError()) {	//Report error and try to restore process
			int err = 0;
			switch (nodeType) {
				case TreeNode::TNALGOP : err = TCError::ARG_ALG; break;
				case TreeNode::TNNONALGOP : err = TCError::ARG_NALG; break;
				case TreeNode::TNCASTTO : err = TCError::ARG_CASTTO; break;
				default : err = TCError::UNKNOWN; break;
			}
			//int err = (nodeType == TreeNode::TNALGOP ? TCError::ARG_ALG : TCError::ARG_NALG);
			string attr[] = {opStr, lSig->toString(), rSig->toString(), lSigDf->toString(), rSigDf->toString()};
			reportTypeError(TCError(err, tcRes.getErrorParts(), vector<string>(attr, attr + 5)));

			errcode = restoreAfterBadArg(tn, nodeType, op, tcRes, lSig, rSig);
			if (errcode != 0) return (errcode != (ErrTypeChecker | ETCNotApplicable) ? errcode : (ErrTypeChecker | EGeneralTCError));
		}
		if (tcRes.isError() || tcRes.getSig() == NULL) {//Retore failed - return with final error.
			return (ErrTypeChecker | EGeneralTCError);
		}
				//If all OK, or process restored - push result on sQres and continue typechecking
		errcode = sQres->pushSig(tcRes.getSig()->clone());
		if (errcode != 0) return errcode;
		if (Deb::ugOn()) cout<<"TypeChecker::typeCheck( "<< opStr <<" ) DONE.\n";
		return 0;

	}

	int TypeChecker::processAugmentDerefCoerceRestoreUnOp(int nodeType, int errType, int op, string opStr, Signature *argSig, TreeNode *tn) {
		return processAugmentDerefCoerceRestoreUnOp(nodeType, errType, op, opStr, argSig, tn, "", 0, true);
	}

	int TypeChecker::processAugmentDerefCoerceRestoreUnOp(int nodeType, int errType, int op, string opStr, Signature *argSig, TreeNode *tn, string name, int option, bool doDeref) {
		int errcode = 0;
		if (argSig == NULL) return (ErrTypeChecker | ETCInnerFailure);
		TypeCheckResult tcRes;
		Signature *argSigD = NULL;
		UnOpDecisionTable *uDTab = getDTables()->getUnOpDTable(nodeType, op);
		errcode = uDTab->getResult(tcRes, argSig, name, option);
		if (errcode != 0) return errcode;
		if (Deb::ugOn()) cout << "UNOP(" << opStr << "), got such tcresult: \n" << tcRes.printAllInfo() << endl;

		if (doDeref && tcRes.isError()) {	//Try to augment by DEREF
			if ((errcode = augmentTreeDeref(tn, uDTab, tcRes, argSig, argSigD)) != 0) return errcode;
		}
		if (argSigD == NULL) argSigD = argSig;
		if (tcRes.isCoerce()) {	//perform COERCE operations
			if ((errcode = augmentTreeCoerce(tn, argSig, NULL, tcRes)) != 0) return errcode;
		}
		if (tcRes.isError()) {
			string attr[] = {opStr, argSig->toString(), name, argSigD->toString()};
			reportTypeError(TCError(errType, tcRes.getErrorParts(), vector<string>(attr, attr + 4)));
			errcode = restoreAfterBadArg(tn, nodeType, op, tcRes, argSig, name, option);
			if (errcode != 0) return (errcode != (ErrTypeChecker | ETCNotApplicable) ? errcode : (ErrTypeChecker | EGeneralTCError));
		}
		if (tcRes.isError() || tcRes.getSig() == NULL) {//Restore failed - return with final error.
			return (ErrTypeChecker | EGeneralTCError);
		}
		//All went OK, or process restored - push result on sQres and continue typechecking
		errcode = sQres->pushSig(tcRes.getSig()->clone());
		if (errcode != 0) return errcode;
		if (Deb::ugOn()) cout<<"TypeChecker::typeCheck(UNOP: "<< opStr <<") DONE.\n";
		return 0;
	}

	Signature* TypeChecker::extractSigFromBindVector(vector<BinderWrap*> *vec) {
		if (bindError(vec)) return NULL;
		Signature *sig = vec->at(0)->getBinder()->getValue()->clone();

		return sig;
	}

	int TypeChecker::openScope(Signature *sig) {
		return sEnvs->pushBinders(sig->statNested(NULL));
	}

	int TypeChecker::closeScope(Signature *sig) {
		sEnvs->pop();
		return 0;
	}

	int TypeChecker::augmentTreeEllipsis(TreeNode *tn, string name, vector<BinderWrap*> *vec, string expandName) {
		if (Deb::ugOn()) cout << "AUGMENTING(ellipsis), ADDING '." << expandName << "' TO THE QUERY ..." << endl;
		TreeNode *parent = tn->getParent();
		if (parent == NULL) {
			Deb::ug("augmentEllipsis: parent is NULL, so cannot do augment ellipsis.");
			return (ErrTypeChecker | ETCInnerFailure);
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
			performAction(tcRes.getActionAt(i-1), tn, tcRes.getCoerceSig());
		}
		if (Deb::ugOn()) {
			cout << "Tree after coerces:\n"; tn->serialize(); cout << endl;
			tn->putToString(); cout << endl;
		}
		return 0;
	}

	int TypeChecker::trySingleDeref(bool canDeref, Signature *sig, Signature *&sigIn, TypeCheckResult &tmpTcRes, bool &doDeref) {
		int errcode = 0;
		if (not canDeref) {sigIn = sig; return 0;}

		//code below can be simplified when deref dec. table properly filled.
		//if ((sig != NULL) && (sig->type() == Signature::SBINDER)) sig = ((StatBinder *)sig)->getValue();
		//if ((sig != NULL) && (sig->type() == Signature::SREF)) {
		if (sig != NULL) {
			if (sig->isRefed()) {sigIn = sig; return 0;}
			errcode = this->getDTables()->getUnOpDTable(TreeNode::TNUNOP, UnOpNode::deref)->getResult(tmpTcRes, sig);
			if (errcode != 0) return errcode;
			if (tmpTcRes.getSig() == NULL) return (ErrTypeChecker | ETCInnerFailure);
			if (tmpTcRes.isError() || tmpTcRes.getSig()->isSolelyEqualTo(sig)) sigIn = sig;
			else {
				sigIn = tmpTcRes.getSig()->clone();
				//typenames hold when derefing implicitly. they don't have to when deref is explicit. ..?
				//sigIn->setTypeName(sig->getTypeName());
				doDeref = true;
			}
		} else {
			sigIn = sig;
		}
		return errcode;
	}

	int TypeChecker::augmentTreeDeref(TreeNode *tn, DecisionTable *dt, TypeCheckResult &tcRes, Signature *lSig, Signature *rSig, Signature *&lSigIn, Signature *&rSigIn) {
		if ((tn->type() == TreeNode::TNALGOP) &&
			(((AlgOpNode *) tn)->getOp() == AlgOpNode::refeq)) return 0; // No deref for '=='
		Deb::ug("TypeChecker::augmentTreeDeref()...");
		int errcode = 0;
		//Signature *lSigIn = NULL, *rSigIn = NULL;
		TypeCheckResult outTcRes, tmpTcResL, tmpTcResR;
		bool derefLeft = false, derefRight = false;
		bool canDerefL = true, canDerefR = true;
		tn->canDerefSons(canDerefL, canDerefR);

		errcode = trySingleDeref(canDerefL, lSig, lSigIn, tmpTcResL, derefLeft);
		if (errcode != 0) return errcode;

		errcode = trySingleDeref(canDerefR, rSig, rSigIn, tmpTcResR, derefRight);
		if (errcode != 0) return errcode;

		Deb::ug("GOING to second phase of augment Tree deref. with deref params:");
		cout << tmpTcResL.printAllInfo() << endl << tmpTcResR.printAllInfo() << endl;

		if (!tmpTcResL.isError() && !tmpTcResR.isError() && lSigIn != NULL && rSigIn != NULL
		   	&& !(lSig == lSigIn && rSig == rSigIn)) {
			Deb::ug("AUGMENT DEREF sec. phase - got these input signatures: ");
			cout << lSigIn->toString() << " & " << rSigIn->toString() << endl;
			errcode = dt->getResult(outTcRes, lSigIn, rSigIn);
			if (errcode != 0) return errcode;
			if (not (outTcRes.isBaseError())) outTcRes.fill(tcRes);
			cout << "After getResult on dereffed signatures: \n" << outTcRes.printAllInfo() << endl;
			if (!outTcRes.isError() && outTcRes.getSig() != NULL) {	//then modify the tree...
				//outTcRes.fill(tcRes);
				Deb::ug("AUGMENT DEREF - will modify the tree !!! ");
				tn->augmentTreeDeref(derefLeft, derefRight);
				if (Deb::ugOn()) {tn->serialize(); cout << endl;}
			}
		}
		Deb::ug("AUGMENT DEREF - ending... ");
		return 0;
	}


	int TypeChecker::augmentTreeDeref(TreeNode *tn, UnOpDecisionTable *udt, TypeCheckResult &tcRes, Signature *sig, Signature *&sigIn) {
		if (not tn->canDerefNode()) return 0;	// No deref for '=='
		int errcode = 0;
		Deb::ug("TypeChecker::augmentTreeDeref(UnOpDTable)...");
		TypeCheckResult outTcRes, tmpTcRes;
		bool canDeref = tn->canDerefSon();
		bool doDeref = false;

		errcode = trySingleDeref(canDeref, sig, sigIn, tmpTcRes, doDeref);
		if (errcode != 0) return errcode;
		Deb::ug(" UN OP going to second phase of augment Tree deref, with deref tcRes: ");
		cout << tmpTcRes.printAllInfo() << endl;
		if (!tmpTcRes.isError() && (sigIn != NULL) && (sig != sigIn)) {
			cout << "UNOP Augment deref sec. ph with sig: " << sigIn->toString() << endl;
			errcode = udt->getResult(outTcRes, sigIn);
			if (errcode != 0) return errcode;
			if (not (outTcRes.isBaseError())) outTcRes.fill(tcRes);
			if (!outTcRes.isError() && outTcRes.getSig() != NULL) {
				tn->augmentTreeDeref();
				if (Deb::ugOn()) {tn->serialize(); cout << endl;}
			}
		}
		Deb::ug("UN OP AUGMENT DEREF - ending... ");
		return 0;
	}

	int TypeChecker::restoreAfterMisspelledName(TreeNode *tn, string name, vector<BinderWrap*> *vec) {
		if (Deb::ugOn()) cout << "will call estoreAfterMisspelledName ! for name: " << name << endl;
		return this->restoreAlgorithm->restoreOnMisspelledName(tn, name, vec);
	}

	//TODO: should restore take the dt, or just op and nodeType? Will it be a new decisionTable or the same one..?
	int TypeChecker::restoreAfterBadArg(TreeNode *tn, int tType, int op, TypeCheckResult &tcRes, Signature *lSig, Signature *rSig) {
		cout << "restoreAfterBadArg() called" << endl;
		return this->restoreAlgorithm->restoreOnBadArg(tn, tType, op, tcRes, lSig, rSig, new DTCRestoreParam("",0));
	//return (ErrTypeChecker | ECannotRestore);
	}

	int TypeChecker::restoreAfterBadArg(TreeNode *tn, int tType, int op, TypeCheckResult &tcRes, Signature *sig, string name, int option) {
		cout << "restoreAfterBadArg( UN OP ) called" << endl;
		return this->restoreAlgorithm->restoreOnBadArg(tn, tType, op, tcRes, sig, new DTCRestoreParam(name, option));
		//return (ErrTypeChecker | ECannotRestore);
	}


	/* ----------------		TCGlobalResult methods		------------------- */
	void TCGlobalResult::printOutput() {
		ErrorConsole &ec(ErrorConsole::get_instance(EC_TYPE_CHECKER));
		debug_print(ec,  "TypeChecking result: " + overallResult  + "\n ");
		if (overallResult == "ERROR") {
			debug_print(ec,  "Query type check returned errors: \n");
			cout << "errors size: " << errors.size() << endl;
			for (unsigned int i = 0; i < errors.size(); i++) {
				debug_print(ec,  " -->" + errors[i].getOutput() + " \n");
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

	void TCGlobalResult::reportTypeError(TCError err) {
		overallResult = TC_RS_ERROR;
		cout << "will add error to globalresult" << endl;
		addError(err);
		cout << "Added error to globalresult" << endl;
	}

	bool TCGlobalResult::isError() {return overallResult == TC_RS_ERROR;}

}



