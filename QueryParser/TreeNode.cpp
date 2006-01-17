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


	int NameNode::staticEval (StatQResStack *&qres, StatEnvStack *&envs) {
		BinderWrap *bw = envs->bindName(this->name);
		this->setBindSect(bw->getSectNumb());
		this->setStackSize(envs->getSize());
		qres->pushSig (bw->getBinder()->getValue());
		return 0;
	}
	int ValueNode::staticEval (StatQResStack *&qres, StatEnvStack *&envs) {
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
		if (this->arg->staticEval(qres, envs) == -1 ) return -1;
		Signature *topSig = qres->topSig();
		qres->pop();
		if (((Signature *)topSig)->type() == Signature::SBINDER) {/* We simply change the name in the binder..?*/
			((StatBinder *) topSig)->setName(this->name);
			qres->pushSig (topSig);
		}
		else qres->pushSig (new StatBinder (this->name, topSig));
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
				qres->pushSig (topSig);
				return 0;
				}//case unMinus
			case UnOpNode::boolNot:	{
				cout << "static_eval::UNOP::boolNot -- starting" << endl;
				if (topSig->type() != Signature::SBOOL) {return 0;}
				/*if topSig is a value signature, the type doesn't change.. */				
				qres->pushSig (topSig);									
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
			cout << "static_eval::AlgOp -- can't handle comma yet ... " << endl;
			return -1; //change to return 0 when this case is implemented. 
		}
		if (op == AlgOpNode::insert) {
			cout << "static_eval::AlgOp -- can't handle insert ... " << endl;
			return -1; //change to return 0 when this case is implemented. 
		}
		return 0;	
	}
	
	int NonAlgOpNode::staticEval (StatQResStack *&qres, StatEnvStack *&envs) {
/*TODO: !!! first do dot, join and where, the rest will follow ... */
		if (this->larg->staticEval(qres, envs) == -1) return -1;
		Signature *lSig = qres->topSig();
		cout << "static_eval::NON ALG OP -- finished with LEFT arg" << endl;

		envs->pushBinders(lSig->statNested());
	
		this->setOpenSect(envs->getSize());
		if (this->rarg->staticEval(qres, envs) == -1) return -1;
		Signature *rSig = qres->topSig();
		cout << "static_eval::NON ALG OP -- finished with RIGHT arg"  << endl;
		
		envs->pop();	
		qres->pop();	/*pop rSig*/
		qres->pop();	/*pop lSig*/
		switch (op) {
			case NonAlgOpNode::dot :{/*we just take the part to the right of the dot*/
				qres->pushSig(rSig);
				}
			case NonAlgOpNode::where: {/*TODO: do we assume the <where> condition is fulfilled ?? */
				if (rSig->type() != Signature::SBOOL) {cout <<"no bool after 'where' !" << endl; return -1;}
				qres->pushSig(lSig);				
				} 
			case NonAlgOpNode::join: {/* eg. EMP.SAL --> push (struct(EMP, SAL)) ... */
				SigColl *s = new SigColl(Signature::SSTRUCT);
				s->setElts(rSig); s->addToMyList(lSig);
				qres->pushSig(s);
				}
			default: {cout << "statEval::nonalg operator not handled yet.." << endl; return -1;}
		
		}
		return 0;
	}

	
	
/*TODO: what about ... TransactNode, CreateNode .. ? */

}


