#include "TreeNode.h"
#include "Stack.h"
#include "DataRead.h"
#include "Optymalizator.h"
#include "stdio.h"
#include "stdlib.h"
#include "Deb.h"

namespace QParser 
{

    TreeNode* NameNode::clone()     { return new NameNode(name); }
    TreeNode* ParamNode::clone()    { return new ParamNode(name); }
    TreeNode* IntNode::clone()      { return new IntNode(value); }
    TreeNode* StringNode::clone()   { return new StringNode(value); }
    TreeNode* DoubleNode::clone()   { return new DoubleNode(value); }
    TreeNode* NameAsNode::clone()   { return new NameAsNode((QueryNode*)arg->clone(), name, group); }
    TreeNode* UnOpNode::clone()     { return new UnOpNode((QueryNode*)arg->clone(), op); }
    TreeNode* AlgOpNode::clone()    { return new AlgOpNode((QueryNode*)larg->clone(), (QueryNode*)rarg->clone(), op); }
    TreeNode* NonAlgOpNode::clone() { return new NonAlgOpNode((QueryNode*)larg->clone(), (QueryNode*)rarg->clone(), op); }
    TreeNode* TransactNode::clone() { return new TransactNode(op); }
        
    TreeNode* CreateNode::clone()   { 
	if (arg != NULL) return new CreateNode(name, (QueryNode*)arg->clone()); 
	else return new CreateNode(name, NULL);}
    TreeNode* LinkNode::clone() {return new LinkNode(nodeName, ip, port); }
    TreeNode* CondNode::clone() {
	if (rarg != NULL) return new CondNode((QueryNode*)condition->clone(), (QueryNode*)larg->clone(), (QueryNode*)rarg->clone(), op);
	else return new CondNode((QueryNode*)condition->clone(), (QueryNode*)larg->clone(), op);
    }
    TreeNode* FixPointNode::clone() {
	vector<QueryNode*> q;
	vector<string> n;
	FixPointNode *fn = new FixPointNode();
	for (int i = 0; i < partsNumb; i++) {
	    q.push_back((QueryNode*) queries.at(i)->clone());
	    n.push_back(names.at(i));
	} 
	fn->setQueries(q);
	fn->setNames(n);
	fn->setPartsNumb(partsNumb);
	return fn;
    }

    TreeNode* ReturnNode::clone() { 
    	if( query != NULL )
    		return new ReturnNode((QueryNode*) query->clone()); 
    	else
    		return new ReturnNode();
    }
    TreeNode* ProcedureNode::clone() { return new ProcedureNode(name, code, params, paramsNumb); }
    TreeNode* RegisterProcNode::clone() { return new RegisterProcNode((QueryNode*) query->clone()); }
    TreeNode* RegisterViewNode::clone() { return new RegisterViewNode((QueryNode*) query->clone()); }
    TreeNode* CallProcNode::clone() {
	vector<QueryNode*> q;
	CallProcNode *cpn = new CallProcNode(name);
	for (unsigned int i = 0; i < partsNumb; i++) {
	    q.push_back((QueryNode*) queries.at(i)->clone());   
	} 
	cpn->setQueries(q);
	cpn->setPartsNumb(partsNumb);
	return cpn;    
    }
    TreeNode* VectorNode::clone() {
	vector<QueryNode*> q;
	for (unsigned int i = 0; i < queries.size(); i++) {
	    q.push_back((QueryNode*) queries.at(i)->clone());
	}
	VectorNode *vn = new VectorNode();
	vn->setQueries(q);
	return vn;
    }
    TreeNode* ViewNode::clone() {
    	ViewNode *vn = new ViewNode(name);
    	for (unsigned int i=0; i < procedures.size(); i++) {
		vn->procedures.push_back(procedures[i]);
	}
	for (unsigned int i=0; i < subviews.size(); i++) {
		vn->subviews.push_back(subviews[i]);
	}
	return vn;
    }
    
    
    
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
    
    
    
    /* INTERFACE NODES */
    
    CreateInterfaceNode::CreateInterfaceNode(string name, InterfaceStruct* iStruct) {
	this->interfaceName = name;
	this->iStruct = iStruct;
    };
    
    CreateInterfaceNode::~CreateInterfaceNode() {
	if (iStruct)
	    delete iStruct;
    };
    
    
    TreeNode *CreateInterfaceNode::clone() {
	return new CreateInterfaceNode(interfaceName, (InterfaceStruct *) iStruct->clone());
    };
    
    
    InterfaceStruct::InterfaceStruct(InterfaceAttributeListNode *attList, InterfaceMethodListNode *methods) {
	this->attributeList = attList;
	this->methodList = methods;
    };
    
    InterfaceStruct::~InterfaceStruct() {
	if (attributeList)
	    delete attributeList;
	if (methodList)
	    delete methodList;
    };
    
    
    TreeNode* InterfaceStruct::clone() {
	return new InterfaceStruct((InterfaceAttributeListNode *)attributeList->clone(), (InterfaceMethodListNode *) methodList->clone());
    };
    
    InterfaceAttribute::InterfaceAttribute(string valueName, string typeName) {
	this->valueName = valueName;
	this->typeName = typeName;
    };
    
    InterfaceAttribute::~InterfaceAttribute() {};
    
    TreeNode* InterfaceAttribute::clone() {
	return new InterfaceAttribute(valueName, typeName);    
    };
    
    
    InterfaceAttributeListNode::InterfaceAttributeListNode(InterfaceAttribute *attribute, InterfaceAttributeListNode *rest) {
	this->attribute = attribute;
	this->attributeList = rest;
    };
    
    InterfaceAttributeListNode::~InterfaceAttributeListNode() {
	if (attribute) 
	    delete attribute;
	if (attributeList)
	    delete attributeList;
    };
    
    TreeNode* InterfaceAttributeListNode::clone() {
	return new InterfaceAttributeListNode((InterfaceAttribute *)attribute->clone(), (InterfaceAttributeListNode *)attributeList->clone());
    };
    
    InterfaceMethodListNode::InterfaceMethodListNode(InterfaceMethod *method, InterfaceMethodListNode *rest) {
	this->method = method;
	this->methodList = rest;    
    };
    
    InterfaceMethodListNode::~InterfaceMethodListNode() {
	if (method)
	    delete method;
	if (methodList)
	    delete methodList;
    };
    
    TreeNode * InterfaceMethodListNode::clone() {
	return new InterfaceMethodListNode((InterfaceMethod *)method->clone(), (InterfaceMethodListNode *)methodList->clone());
    }

    InterfaceMethod::InterfaceMethod(string methodName, InterfaceMethodParamListNode *methodParams) {
	this->methodName = methodName;
	this->methodParams = methodParams;    
    };
    
    InterfaceMethod::~InterfaceMethod() {
	if (methodParams)
	    delete methodParams;    
    };
    
    TreeNode *InterfaceMethod::clone() {
	return new InterfaceMethod(methodName, (InterfaceMethodParamListNode *)methodParams->clone());
    };
    
    InterfaceMethodParamListNode::InterfaceMethodParamListNode(InterfaceMethodParam *methodParam, InterfaceMethodParamListNode *rest) {
	this->methodParam = methodParam;
	this->methodParamList = rest;
    };
    
    InterfaceMethodParamListNode::~InterfaceMethodParamListNode() {
	if (methodParam)
	    delete methodParam;
	if (methodParamList)
	    delete methodParamList;    
    };
    
    TreeNode* InterfaceMethodParamListNode::clone() {
	return new InterfaceMethodParamListNode((InterfaceMethodParam *)methodParam->clone(), (InterfaceMethodParamListNode *)methodParamList->clone());
    };
    
    InterfaceMethodParam::InterfaceMethodParam(string valueName, string typeName) {
	this->valueName = valueName;
	this->typeName = typeName;
    };
    
    InterfaceMethodParam::~InterfaceMethodParam() {};
    
    TreeNode* InterfaceMethodParam::clone() {
	return new InterfaceMethodParam(valueName, typeName);
    };
    
    
    /**
      *		VALIDATION NODE	BEGIN
      */
    ValidationNode::ValidationNode(string login, string passwd) {
	this->login 	= login;
	this->passwd	= passwd;
    };
    string ValidationNode::get_login() {
	return login;
    };
    string ValidationNode::get_passwd() {
	return passwd;
    };    
    TreeNode* ValidationNode::clone() {
	return new ValidationNode(login, passwd);
    };
    int ValidationNode::type() {
	return TNVALIDATION;
    };
    int ValidationNode::putToString() {
	cout << "Validation node. Login: " << login << ", Password: " << passwd << endl;
	return 0;
    };      

    PriviligeListNode::PriviligeListNode(Privilige *priv, PriviligeListNode *priv_list) {
	this->priv = priv;
	this->priv_list = priv_list;
    };
    PriviligeListNode::~PriviligeListNode() {
	delete priv;
	if (priv_list)
	    delete priv_list;
    };
    Privilige* PriviligeListNode::get_priv() {
	return priv;
    };
    PriviligeListNode* PriviligeListNode::try_get_priv_list() {
	return priv_list;
    };
    TreeNode* PriviligeListNode::clone() {
	if (priv_list)
	    return new PriviligeListNode( priv->clone(), (PriviligeListNode *) priv_list->clone() );
	else
	    return new PriviligeListNode( priv->clone());
    };
    int PriviligeListNode::type() {
	return TNPRIVLIST;
    };
    int PriviligeListNode::putToString() {
	cout << "Privilige list. Privilige " << priv->to_string() << " ";
	if (priv_list)
	    priv_list->putToString();
	cout << endl;
	return 0;
    };

    NameListNode::NameListNode(string name, NameListNode *list) {
	this->name = name;
	this->name_list = list;
    };
    NameListNode::~NameListNode() {
	if (name_list)
	    delete name_list;
    };
	    
    /* getters */
    string NameListNode::get_name() {
	return name;
    };
    NameListNode* NameListNode::try_get_name_list() {
	return name_list;
    };

    /* inherited functions */
    TreeNode* NameListNode::clone() {
	if (name_list)
	    return new NameListNode(name, (NameListNode *) name_list->clone());
	else
	    return new NameListNode(name);
    };
    int NameListNode::type() {
	return TNNAMELIST;
    };
    int NameListNode::putToString() {
	cout << "Name list node. Name " << name << " ";
	if (name_list)
	    name_list->putToString();
	return 0;
    };
    
    GrantPrivNode::GrantPrivNode(PriviligeListNode *priv_list, NameListNode *name_list, string user, bool with_grant_option) {
    	this->priv_list = priv_list;
    	this->name_list = name_list;
    	this->user = user;
    	this->with_grant_option = with_grant_option;
    };
    GrantPrivNode::~GrantPrivNode() {
	delete name_list;
	delete priv_list;
    };
	    
    PriviligeListNode*  GrantPrivNode::get_priv_list() {
	return priv_list;
    };
    NameListNode* GrantPrivNode::get_name_list() {
	return name_list;
    };
    string GrantPrivNode::get_user() {
	return user;
    };
    bool GrantPrivNode::get_with_grant_option() {
	return with_grant_option;
    };

    TreeNode* 	GrantPrivNode::clone() {
	return new GrantPrivNode(   (PriviligeListNode *) priv_list->clone(), 
				    (NameListNode *) name_list->clone(), 
				    user,
				    with_grant_option);
    };
    int GrantPrivNode::type() {
	return TNGRANTPRIV;
    };
    int GrantPrivNode::putToString() {
	cout << "Grant node. grant option = " << with_grant_option << " " ;
	priv_list->putToString(); cout << " ";
	name_list->putToString(); cout << " ";
	cout << user << " ";
	return 0;
    };
    
    RevokePrivNode::RevokePrivNode(PriviligeListNode *priv_list, NameListNode *name_list, string user ) {
	this->priv_list = priv_list;
	this->name_list = name_list;
	this->user = user;
    };
    RevokePrivNode::~RevokePrivNode() {
	delete priv_list;
	delete name_list;
    };
	    
    PriviligeListNode* 	RevokePrivNode::get_priv_list() {
	return priv_list;
    };
    NameListNode* RevokePrivNode::get_name_list() {
	return name_list;
    };
    string RevokePrivNode::get_user() {
	return user;
    };
    TreeNode* RevokePrivNode::clone() {
	return new RevokePrivNode( (PriviligeListNode *) priv_list->clone(), 
				    (NameListNode *) name_list->clone(), 
				    user);
    };
    int RevokePrivNode::type() {
	return TNREVOKEPRIV;
    };
    int RevokePrivNode::putToString() {
	cout << "Revoke node. User " << user << " ";
	priv_list->putToString(); cout << " ";
	name_list->putToString();
	return 0;
    };

    CreateUserNode::CreateUserNode(string user, string passwd) {
	this->user = user;
	this->passwd = passwd;
    };	
    string CreateUserNode::get_user() {
	return user;
    };
    string CreateUserNode::get_passwd() {
	return passwd;
    };	    
    TreeNode* CreateUserNode::clone() {
	return new CreateUserNode(user, passwd);
    };
    int CreateUserNode::type() {
	return TNCREATEUSER;
    };
    int CreateUserNode::putToString() {
	cout << "Create user node. User " << user << ", passwd " << passwd;
	return 0;
    };
    
    RemoveUserNode::RemoveUserNode(string user) {
	this->user = user;
    };
    string RemoveUserNode::get_user() {
	return user;
    };
    TreeNode* RemoveUserNode::clone() {
	return new RemoveUserNode(user);
    };
    int RemoveUserNode::type() {
	return TNREMOVEUSER;
    };
    int RemoveUserNode::putToString() {
	cout << "Remove user node. User " << user;
	return 0;
    };

      
/* wartosci zwracane ? : 0 -- nic nie znalezione, szukaj dalej 
						(cale drzewo daje 0 =>nie ma niez. podz.)
						-1 -- blad, przerwij. 
						-2 -- znalazlem i przenioslem niez. poddrzewo, trzeba przerwac, 
						      powtorzyc static eval i znow te procedure ... */

	int AlgOpNode::optimizeTree() {
		Deb::ug("alg: %d.", op);
		int pRes = larg->optimizeTree();	/*partial result*/
		if (pRes < 0) return pRes;
		else return rarg->optimizeTree();		
	}
//	TreeNode* NonAlgOpNode::getHighestIndependant() {return NULL;};
	
	int NonAlgOpNode::optimizeTree() {
		int pRes = 0;
		Optymalizator *optimiser = new Optymalizator();
		
		TreeNode *hInd = optimiser->getIndependant(this);
		//LL; //this->getHighestIndependant();
			Deb::ug("NONalg: %d.\n", op);
		if (hInd == NULL) {
			Deb::ug("hInd is NULL\n");
			if ((pRes = larg->optimizeTree()) < 0) return pRes;
			else return rarg->optimizeTree();
		} else {
		    if (Deb::ugOn()) { cout << "found an indep. subtree: " << endl;
			hInd->putToString(); cout << endl;
		    }
		}
		TreeNode *prt = this->parent;
		if (Deb::ugOn()) {
    		    if (prt == NULL) fprintf(stderr, "parent is NULL ! \n");
		    else {
		        fprintf (stderr, "parent is ... NOT NULL ! \n");
			prt->putToString(); cout << endl;
			fprintf (stderr, "this was the parent, huh? \n");
		    }
		}
		//TreeNode *fctrd = this->factorSubQuery(hInd, randomString());
		if (prt != NULL) {
			prt-> swapSon (this, this->factorSubQuery(hInd, randomString()));
		        Deb::ug("swapped old son with factoredSubQuery\n");
		} else this->factorSubQuery(hInd, randomString());
		return -2;	
		/*found ind. subq and factored it. restart the whole stEval & optimisation procedure. */
	}

	int NameNode::staticEval (StatQResStack *&qres, StatEnvStack *&envs) {
		Deb::ug("staticEval(Name) \n");
		if (Deb::ugOn()) cout << name << endl;
		BinderWrap *bw = envs->bindName(this->name);
		if (bw == NULL) { Deb::ug("name could NOT be bound ! will exit..\n"); return -1;}
		this->setBindSect(bw->getSectNumb());
		this->setStackSize(envs->getSize());
		this->setCard(bw->getBinder()->getCard());		// to jest zamiast wyw evalCard
		Deb::ug("name bound on envs, section nr set to %d, ", bw->getSectNumb());
		//Deb::ug("ok:%d, ", ((SigRef *) (bw->getBinder()->getValue()))->getRefNo());
		Signature *sig = bw->getBinder()->getValue()->clone();	//death
	
		if (sig->getDependsOn() == NULL){
			sig->setDependsOn(this);
		} else {
			Deb::ug("jsi death sygnatura juz miala ustawione dependsOn, nie ustawia go ponownie\n");
		}
		qres->pushSig (sig);
		 this->setDependsOn(bw->getBinder()->getDependsOn());
		Deb::ug("result pushed on qres - end nameNode stEval\n");
		return 0;
	}
	int ValueNode::staticEval (StatQResStack *&qres, StatEnvStack *&envs) {
		Deb::ug("staticEval:value node ");
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
	    this->evalCard();
	    return 0;
	}
	int NameAsNode::staticEval (StatQResStack *&qres, StatEnvStack *&envs) {
		/*zrob staticEval dla this->arg. potem sprawdz top na qres, zrob pop, zrob z tego bindera, wsadz spowrotem.*/
		Deb::ug( "staticEval(nameas) starting.. \n");
		if (this->arg->staticEval(qres, envs) == -1 ) return -1;
		Deb::ug( "staticEval(nas) - done with arg\n");
		Signature *topSig = qres->topSig();
		qres->pop();
		/* to jest wersja oryginalna, wydaje mi sie ze nie poprawna
		 * jakub sitek nizej napisalem tak jak mi sie wydaje ze jest ok
		 * tzn nie ma tego prostego zmieniania nazwy bindera, zawsze jest tworzony nowy,
		if (((Signature *)topSig)->type() == Signature::SBINDER) {// We simply change the name in the binder..?
			((StatBinder *) topSig)->setName(this->name);
			qres->pushSig (topSig->clone());
		}
		else qres->pushSig (new StatBinder (this->name, topSig->clone()));
    	*/
    	StatBinder *sb = new StatBinder (this->name, topSig->clone());
   // 	sb->setDependsOn(NULL);	// czy to jest ok?? - chyba jednak nie, i powinno byc tak:
   		sb->setDependsOn(this);
    	qres->pushSig (sb);
    	this->evalCard();
    	return 0;
	}
	
	int UnOpNode::staticEval (StatQResStack *&qres, StatEnvStack *&envs) {
//	enum unOp { unMinus, count, sum, avg, min, max, distinct, boolNot, deleteOp, deref };		 
		if (this->arg->staticEval(qres, envs) == -1) return -1;
		Signature *topSig = qres->topSig();
		Deb::ug("eval::UNOP -- finished with ARG.");
		qres->pop();

		switch (op) {
/*TODO: deleteOp, distinct *//* DONE: unMinus, boolNot, deref, count */
			case UnOpNode::unMinus:	{
				Deb::ug("static_eval::UNOP::unMinus -- starting");
				if (!topSig->isAtom() || topSig->type() == Signature::SSTRING) {return -1;} /* TODO: or should I insert a deref here?*/
				/*if topSig is a value signature, the type doesn't change.. */
				qres->pushSig (topSig->clone());
				return 0;
				}//case unMinus
			case UnOpNode::boolNot:	{
				Deb::ug("static_eval::UNOP::boolNot -- starting");
				if (topSig->type() != Signature::SBOOL) {return 0;}
				/*if topSig is a value signature, the type doesn't change.. */				
				qres->pushSig (topSig->clone());									
				return 0;
				}//case boolNOT				
			case UnOpNode::deref: {
				Deb::ug("static_eval::UNOP::deref -- starting");
				int argType = topSig->type();
				if (argType != Signature::SREF) {return -1;}
				/*TODO: should also allow topSig to be a bag or sequence, huh? what about struct? */
				/*we make use of the global DataScheme *dScheme */
				DataObjectDef *obj = DataScheme::dScheme()->getObjById (((SigRef *)topSig)->getRefNo());
				if (obj == NULL) return -1;
				/*error: invalid object def id. */
				string objKind = obj->getKind();
				if (objKind == "complex") {
					Deb::ug("eval::UNOP::deref -- can't handle deref(complex)");
					return -1;}
				if (objKind == "atomic") qres->pushSig (new SigAtomType (obj->getType()));
				else if (objKind == "link") qres->pushSig (new SigRef (obj->getTarget()->getMyId()));
				return 0;	
				} /*case deref */
            case UnOpNode::nameof: {
                
                Deb::ug("static_eval::UNOP::nameof -- starting");
                
                int argType = topSig->type();
                
                if (argType != Signature::SREF) return -1;
                /* TODO: should also allow topSig to be a bag or sequence, 
                 * huh? what about struct? 
                 * we make use of the global DataScheme *dScheme */
                                                
                DataObjectDef *obj = DataScheme::dScheme()->getObjById (((SigRef *)topSig)->getRefNo());
                
                if (obj == NULL) return -1;
                
                qres->pushSig(new SigAtomType("string"));                    	               
                                    
                return 0;
                }					
			default: break; // not implemented yet. or in {count, sum, avg, min, max}
		}/*switch*/
		if (op == UnOpNode::count || op == UnOpNode::sum || op == UnOpNode::avg || op == UnOpNode::min || op == UnOpNode::max) {
			Deb::ug("static_eval::UNOP::simpleIntUnOp -- starting");
			/*TODO: check, if this is really independent of topSig ? ...*/
			qres->pushSig (new SigAtomType ("int"));
			return 0;
		}
		this->evalCard();
		return -1;
	}
	
	int AlgOpNode::staticEval (StatQResStack *&qres, StatEnvStack *&envs) {
//  DONE:  {eq, neq, lt, gt, le, ge, boolAnd, boolOr, plus, minus, 
//			times, divide, bagUnion, bagIntersect, bagMinus }; 	
		Deb::ug("staticEval(algOp) - starting\n");
		if (this->larg->staticEval(qres, envs) == -1) return -1;
		Signature *lSig = qres->topSig();
		Deb::ug("static_eval::AlgOP:: -- finished with LEFT arg");
		if (this->rarg->staticEval(qres, envs) == -1) return -1;
		Signature *rSig = qres->topSig();
		Deb::ug("static_eval::AlgOP -- finished with RIGHT arg");
		qres->pop();
		qres->pop();
		this->evalCard();
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
			Deb::ug("can't handle Union,intersect and Minus now.. ");
			return -1;	
		}
		if (op == AlgOpNode::comma) {
			if (Deb::ugOn()) {
			    lSig->putToString(); cout << "/^\\"; rSig->putToString(); cout << endl;
			}
			if (lSig->type() == Signature::SSTRUCT) {
				((SigColl *) lSig)->addToMyList (rSig);
				qres->pushSig (lSig);
			} else 
			{
				SigColl *s = new SigColl(Signature::SSTRUCT);
				s->setElts(lSig); s->addToMyList(rSig);
				qres->pushSig(s);				
			}
			return 0; 
		}
		if (op == AlgOpNode::insert) {
			return -1; //change to return 0 when this case is implemented. 
		}
		this->evalCard();
		return 0;	
	}
	
	int NonAlgOpNode::staticEval (StatQResStack *&qres, StatEnvStack *&envs) {
/*TODO: !!! first do dot, join and where, the rest will follow ... */
		Deb::ug( "staticEval(nonalg) - start\n");
		if (this->larg->staticEval(qres, envs) == -1) return -1;
		Signature *lSig = qres->topSig();
		Deb::ug("static_eval::NON ALG OP -- finished with LEFT arg\n");
		this->setFirstOpenSect(1 + envs->getSize());
		/*TODO: this below is temporary, as it does not handle eg. struct{struct{a,b}, c}...*/
		int sToPop = 0;
		Signature *toPush = lSig->clone();
		Deb::ug("just before push/statNested loop\n");
		if (toPush->isColl()) {
			cout << "pushing from SigColl\n";
			//ten fragment jest zamiast tego nastepnego zakomentowanego, wydaje mi sie ze
			// tamto zle dzialalo, co jezeli elem, sigcoll byla siggcol
			BinderWrap *bindersColl = toPush->statNested(toPush->getDependsOn());
			//sToPop+= bindersColl->size();
			sToPop++;
			
			/*
			BinderWrap *pom = bindersColl;
			
			cout << "wklada na envs: \n";
			while(pom!=NULL){
			    cout << " name: " << pom->getBinder()->getName();
			    pom = pom->getNext();
			}
			cout << " koniec\n";
			*/			
			if (envs->pushBinders(bindersColl) != 0) 
					cout << "EROOR jsi zobacz nonalgopnode steval dla sigcoll \n" <<endl;

			
			/*
			Signature *pt = ((SigColl *) toPush)->getMyList();
			while (pt != NULL) { 
				if (envs->pushBinders(pt->statNested(pt->getDependsOn())) == 0) 
					sToPop++; 
				pt = pt->getNext();
			}
			*/
		} else 	{
			cout << "pushing not coll\n";
			
			if (envs->pushBinders(toPush->statNested(toPush->getDependsOn())) == 0) 
				sToPop = 1; 
			else 
				sToPop = 0;
		}
		if (toPush->getDependsOn() == NULL)
			cout << "the signature depends on NULL !!!!!\n";
		else 
			cout << "the signature depends on " + toPush->getDependsOn()->getName() << endl;
		
		Deb::ug("pushed static nested result on envs\n");
		this->setLastOpenSect(envs->getSize());
		if (this->rarg->staticEval(qres, envs) == -1) return -1;
		Signature *rSig = qres->topSig();
		
		Deb::ug("static_eval::NON ALG OP -- finished with RIGHT arg\n");
		Deb::ug( "static_eval(nonalg): will pop 2 off qres, %d off envs\n", sToPop);

		for (;sToPop > 0; sToPop--){envs->pop();}
		qres->pop();	/*pop rSig*/
		qres->pop();	/*pop lSig*/
		Deb::ug("static_eval(nonalg): popped'em. \n");
		switch (op) {
			case NonAlgOpNode::dot :{/*we just take the part to the right of the dot*/
				qres->pushSig(rSig->clone());
				Deb::ug("dot: pushed rSig on qres\n");
				break;}
			case NonAlgOpNode::where: {/*TODO: do we assume the <where> condition is fulfilled ?? */
				if (rSig->type() != Signature::SBOOL) {
				Deb::ug("no bool after 'where' !"); return -1;}
				qres->pushSig(lSig->clone());				
				break;} 
			case NonAlgOpNode::join: {/* eg. EMP.SAL --> push (struct(EMP, SAL)) ... */
				SigColl *s = new SigColl(Signature::SSTRUCT);
				s->setElts(lSig); s->addToMyList(rSig);
				qres->pushSig(s->clone());
				break;}
			default: {Deb::ug("statEval::nonalg operator not handled yet.."); return -1;}
		
		}
		this->evalCard();
		Deb::ug("stEv will return 0\n");
		return 0;
	}

	string TreeNode::randomString(){
	    char buf[21];
	    buf[20] = 0;
	    for (int i = 0; i < 20; i++){
		int z = rand() % 22;
		buf[i] = (char) ('a' + z);
	    } 
	    string s(buf);
	    if (Deb::ugOn()) {
		cout << "funkcja randomString zwraca 20 literowy napis: " << s << endl;
	    }
	    return s;
	}
	
	int TreeNode::minCard(string card){
		if (card[0] == '0') return 0;
		if (card[0] == '1') return 1;
		return 10;	
	}
	int TreeNode::maxCard(string card){
		if (card[3] == '0') return 0;
		if (card[3] == '1') return 1;
		return 10;		
	}
	string TreeNode::int2card(int card){
		if (card == 0) return "0";
		if (card == 1) return "1";
		return "*";	
	}
	
	string TreeNode::mulCard(string leftCard, string rightCard){
		int min = ((this->minCard(leftCard) < this->minCard(rightCard)) ? this->minCard(leftCard): this->minCard(rightCard));	
		int max = this->maxCard(leftCard) * this->maxCard(rightCard);
		return this->int2card(min) + ".." + this->int2card(max);
	}
}


