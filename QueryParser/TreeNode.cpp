#include "TreeNode.h"
#include "Stack.h"
#include "DataRead.h"

namespace QParser 
{

    TreeNode* NameNode::clone()     { return new NameNode(name); }
    TreeNode* IntNode::clone()      { return new IntNode(value); }
    TreeNode* StringNode::clone()   { return new StringNode(value); }
    TreeNode* DoubleNode::clone()   { return new DoubleNode(value); }
    TreeNode* NameAsNode::clone()   { return new NameAsNode((QueryNode*)arg->clone(), name, group); }
    TreeNode* UnOpNode::clone()     { return new UnOpNode((QueryNode*)arg->clone(), op); }
    TreeNode* AlgOpNode::clone()    { return new AlgOpNode((QueryNode*)larg->clone(), (QueryNode*)rarg->clone(), op); }
    TreeNode* NonAlgOpNode::clone() { return new NonAlgOpNode((QueryNode*)larg->clone(), (QueryNode*)rarg->clone(), op); }
    TreeNode* TransactNode::clone() { return new TransactNode(op); }
    TreeNode* CreateNode::clone()   { return new CreateNode(name, (QueryNode*)arg->clone()); }
    
/*  to be used when subT is an subTree independant of (this) and (this) is a non-alg operator.
    called eg. for node  nd = nonalgop (where) ... 
    newTree = nd->factorSubQuery(subT, "randName")    
    (!!!) when used for optimization purposes, should not be called when 
	  subT is a NameNode, because this does not help optimise a query.. */
    TreeNode* TreeNode::factorSubQuery(TreeNode *subT, string newName) {
    
		subT->getParent()->swapSon((QueryNode *) subT, new NameNode(newName));
		TreeNode *nickNode = new NameAsNode ((QueryNode *) subT, newName, true);	
		TreeNode *topNode = new NonAlgOpNode ((QueryNode *) nickNode, (QueryNode *) this, NonAlgOpNode::dot);
		return topNode;
    }
/* wartosci zwracane ? : 0 -- nic nie znalezione, szukaj dalej 
						(cale drzewo daje 0 =>nie ma niez. podz.)
						-1 -- blad, przerwij. 
						-2 -- znalazlem i przenioslem niez. poddrzewo, trzeba przerwac, 
						      powtorzyc static eval i znow te procedure ... */

	int AlgOpNode::optimizeTree() {
			fprintf (stderr, "alg: %d.", op);
		int pRes = larg->optimizeTree();	/*partial result*/
		if (pRes < 0) return pRes;
		else return rarg->optimizeTree();		
	}
//	TreeNode* NonAlgOpNode::getHighestIndependant() {return NULL;};
	
	int NonAlgOpNode::optimizeTree() {
		int pRes = 0;
		TreeNode *hInd = NULL; //this->getHighestIndependant();
			fprintf (stderr, "NONalg: %d.", op);
		if (hInd == NULL) {
			if ((pRes = larg->optimizeTree()) < 0) return pRes;
			else return rarg->optimizeTree();
		}
		TreeNode *prt = this->parent;
		prt-> swapSon (this, this->factorSubQuery(hInd, "newName"));
		return -2;	/*found ind. subquery and factored it. restart the whole procedure.. */
	}

	int NameNode::staticEval (StatQResStack *&qres, StatEnvStack *&envs) {
		fprintf (stderr, "staticEval(Name) - start\n");
		
		BinderWrap *bw = envs->bindName(this->name);
		cout << name << endl;
		if (bw == NULL) { fprintf (stderr, "name could NOT be bound ! will exit..\n"); return -1;}
		fprintf (stderr, "name bound on envs, ");
		this->setBindSect(bw->getSectNumb());
		fprintf (stderr, "section nr set %d, ", bw->getSectNumb());
		this->setStackSize(envs->getSize());
		fprintf (stderr, "stack size set, ");
		if (bw->getBinder()->getName() == "EMP") fprintf(stderr, "ok");
		else fprintf (stderr, "not EMP, ");
		fprintf(stderr, "ok:%d, ", ((SigRef *) (bw->getBinder()->getValue()))->getRefNo());
		
		qres->pushSig (bw->getBinder()->getValue()->clone());
		fprintf (stderr, "result pushed on qres\n");
		return 0;
	}
	int ValueNode::staticEval (StatQResStack *&qres, StatEnvStack *&envs) {
		fprintf(stderr, "staticEval:value - starting... ");
		switch (this->type()) {
	    	case TreeNode::TNINT: {
		    	qres->pushSig (new SigAtomType ("int"));
				break;}
	    	case TNSTRING: {
				qres->pushSig (new SigAtomType ("string"));
				break;}
	    	case TNDOUBLE: { 
				qres->pushSig (new SigAtomType ("double"));
	    		break;}
	    	default: {return -1; break;}
	    }
	    return 0;
	}
	int NameAsNode::staticEval (StatQResStack *&qres, StatEnvStack *&envs) {
		/*zrob staticEval dla this->arg. potem sprawdz top na qres, zrob pop, zrob z tego bindera, wsadz spowrotem.*/
		fprintf(stderr, "staticEval(nameas) starting.. \n");
		if (this->arg->staticEval(qres, envs) == -1 ) return -1;
		fprintf(stderr, "staticEval(nas) - done with arg\n");
		Signature *topSig = qres->topSig();
		qres->pop();
		if (((Signature *)topSig)->type() == Signature::SBINDER) {/* We simply change the name in the binder..?*/
			((StatBinder *) topSig)->setName(this->name);
			fprintf(stderr, "about to push modified binder on qres\n");
			qres->pushSig (topSig->clone());
		}
		
		else {fprintf(stderr, "about to push sth on qres\n");
		qres->pushSig (new StatBinder (this->name, topSig->clone()));}
		return 0;
	}
	
	int UnOpNode::staticEval (StatQResStack *&qres, StatEnvStack *&envs) {
//	enum unOp { unMinus, count, sum, avg, min, max, distinct, boolNot, deleteOp, deref };		 
		if (this->arg->staticEval(qres, envs) == -1) return -1;
		Signature *topSig = qres->topSig();
		cout << "eval::UNOP -- finished with arg" << endl;
		qres->pop();

		switch (op) {
/*TODO: deleteOp, distinct *//* DONE: unMinus, boolNot, deref, count */
			case UnOpNode::unMinus:	{
				cout << "static_eval::UNOP::unMinus -- starting" << endl;
				if (!topSig->isAtom() || topSig->type() == Signature::SSTRING) {return -1;} /* TODO: or should I insert a deref here?*/
				/*if topSig is a value signature, the type doesn't change.. */
				qres->pushSig (topSig->clone());
				return 0;
				}//case unMinus
			case UnOpNode::boolNot:	{
				cout << "static_eval::UNOP::boolNot -- starting" << endl;
				if (topSig->type() != Signature::SBOOL) {return 0;}
				/*if topSig is a value signature, the type doesn't change.. */				
				qres->pushSig (topSig->clone());									
				return 0;
				}//case boolNOT				
			case UnOpNode::deref: {
				cout << "static_eval::UNOP::deref -- starting" << endl;				
				int argType = topSig->type();
				if (argType != Signature::SREF) {return -1;}
				/*TODO: should also allow topSig to be a bag or sequence, huh? what about struct? */
				/*we make use of the global DataScheme *dScheme */
				DataObjectDef *obj = DataScheme::dScheme()->getObjById (((SigRef *)topSig)->getRefNo());
				if (obj == NULL) return -1;
				/*error: invalid object def id. */
				string objKind = obj->getKind();
				if (objKind == "complex") {
					cout << "eval::UNOP::deref -- can't handle deref(complex object) yet..." <<endl;
					return -1;}
				if (objKind == "atomic") qres->pushSig (new SigAtomType (obj->getType()));
				else if (objKind == "link") qres->pushSig (new SigRef (obj->getTarget()->getMyId()));
				return 0;	
				} /*case deref */					
			default: break; // not implemented yet. or in {count, sum, avg, min, max}
		}/*switch*/
		if (op == UnOpNode::count || op == UnOpNode::sum || op == UnOpNode::avg || op == UnOpNode::min || op == UnOpNode::max) {
			cout << "static_eval::UNOP::simpleIntUnOp -- starting" << endl;				
			/*TODO: check, if this is really independant of topSig ? ...*/
			qres->pushSig (new SigAtomType ("int"));
			return 0;
		}
		return -1;
	}
	
	int AlgOpNode::staticEval (StatQResStack *&qres, StatEnvStack *&envs) {
//	TODO: { comma, insert}
//  DONE:  {eq, neq, lt, gt, le, ge, boolAnd, boolOr, plus, minus, 
//			times, divide, bagUnion, bagIntersect, bagMinus }; 	
		fprintf(stderr, "staticEval(algOp) - starting\n");
		if (this->larg->staticEval(qres, envs) == -1) return -1;
		Signature *lSig = qres->topSig();
		cout << "static_eval::AlgOP:: -- finished with LEFT arg" << endl;
		if (this->rarg->staticEval(qres, envs) == -1) return -1;
		Signature *rSig = qres->topSig();
		cout << "static_eval::AlgOP -- finished with RIGHT arg" << endl;
		qres->pop();
		qres->pop();
		if (op == AlgOpNode::eq || op == AlgOpNode::neq || op == AlgOpNode::lt || 
				op == AlgOpNode::gt || op == AlgOpNode::le || op == AlgOpNode::ge || 
				op == AlgOpNode::boolAnd || op == AlgOpNode::boolOr) {
			/* !!! not checking on types !!! */
			/*TODO: check, if this is really independant of lSig and rSig ? ...*/
			qres->pushSig (new SigAtomType ("bool"));
			return 0;
		}		
		if (op == AlgOpNode::plus || op == AlgOpNode::minus || 
			op == AlgOpNode::times || op == AlgOpNode::divide) {				
			qres->pushSig (new SigAtomType ("double"));				
			return 0;
		}
		if (op == AlgOpNode::bagUnion || op == AlgOpNode::bagIntersect || op == AlgOpNode::bagMinus) {
			cout << "can't handle Union,intersect and Minus on bags yet ... " << endl;
			return 0;	
		}
		if (op == AlgOpNode::comma) {
			/*TODO: must construct a struct here, or if lSig is a struct - then
					add rSig to it... */	
			lSig->putToString(); cout << "/^\\"; rSig->putToString(); cout << endl;
			if (lSig->type() == Signature::SSTRUCT) {
				((SigColl *) lSig)->addToMyList (rSig);
				qres->pushSig (lSig);
			} else {
				SigColl *s = new SigColl(Signature::SSTRUCT);
				s->setElts(lSig); s->addToMyList(rSig);
				qres->pushSig(s);
				
			}
			return 0; //change to return 0 when this case is implemented. 
		}
		if (op == AlgOpNode::insert) {
			cout << "static_eval::AlgOp -- can't handle insert ... " << endl;
			return -1; //change to return 0 when this case is implemented. 
		}
		return 0;	
	}
	
	int NonAlgOpNode::staticEval (StatQResStack *&qres, StatEnvStack *&envs) {
/*TODO: !!! first do dot, join and where, the rest will follow ... */
		fprintf (stderr, "staticEval(nonalg) - start\n");
		if (this->larg->staticEval(qres, envs) == -1) return -1;
		Signature *lSig = qres->topSig();
		fprintf (stderr, "static_eval::NON ALG OP -- finished with LEFT arg\n");
		//fprintf(stderr, "refno: %d.\n", ((SigRef *) lSig)->getRefNo());
		this->setFirstOpenSect(1 + envs->getSize());
		/*TODO: this below is temporary, as it does not handle eg. struct{struct{a,b}, c}...*/
		int sToPop = 0;
		Signature *toPush = lSig->clone();
		if (toPush->isColl()) {
			Signature *pt = ((SigColl *) toPush)->getMyList();
			while (pt != NULL) { if (envs->pushBinders(pt->statNested()) == 0) sToPop++; pt = pt->getNext();}
		} else 	{if (envs->pushBinders(toPush->statNested()) == 0) sToPop = 1; else sToPop = 0;}
		fprintf(stderr, "pushed static nested result on envs\n");
		this->setLastOpenSect(envs->getSize());
		if (this->rarg->staticEval(qres, envs) == -1) return -1;
		Signature *rSig = qres->topSig();
		
		fprintf (stderr, "static_eval::NON ALG OP -- finished with RIGHT arg\n");
		envs->putToString();
		fprintf (stderr, "static_eval(nonalg): will pop 2 off qres, %d off envs\n", sToPop);

		for (;sToPop > 0; sToPop--){envs->pop();}
		qres->pop();	/*pop rSig*/
		fprintf (stderr, "popped 1off qres. ");
		qres->pop();	/*pop lSig*/
		fprintf (stderr, "static_eval(nonalg): popped'em. \n");
		switch (op) {
			case NonAlgOpNode::dot :{/*we just take the part to the right of the dot*/
				qres->pushSig(rSig->clone());
				fprintf (stderr, "dot: pushed rSig on qres\n");
				break;}
			case NonAlgOpNode::where: {/*TODO: do we assume the <where> condition is fulfilled ?? */
				if (rSig->type() != Signature::SBOOL) {cout <<"no bool after 'where' !" << endl; return -1;}
				qres->pushSig(lSig->clone());				
				break;} 
			case NonAlgOpNode::join: {/* eg. EMP.SAL --> push (struct(EMP, SAL)) ... */
				SigColl *s = new SigColl(Signature::SSTRUCT);
				s->setElts(lSig); s->addToMyList(rSig);
				qres->pushSig(s->clone());
				break;}
			default: {cout << "statEval::nonalg operator not handled yet.." << endl; return -1;}
		
		}
		fprintf(stderr, "will return 0\n");
		return 0;
	}

		
/*TODO: what about ... TransactNode, CreateNode .. ? */

}


