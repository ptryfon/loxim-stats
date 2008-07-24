#include "TreeNode.h"
#include "Stack.h"
#include "DataRead.h"
#include "Optimizer.h"
#include "stdio.h"
#include "stdlib.h"
#include "Deb.h"
#include "Errors/Errors.h"
#include "Errors/ErrorConsole.h"
#include "Indexes/Constraints.h"

namespace QParser
{
	long TreeNode::uv = 0;
	long TreeNode::getUv(){return uv++;};


    TreeNode* NameNode::clone()     { NameNode * nowy =  new NameNode(name); nowy->setUsedBy(this->getUsedBy()); nowy->setBoundIn(this->getBoundIn()); nowy->copyAttrsOf(this); return nowy; }
	TreeNode* ParamNode::clone()    { TreeNode * nowy =  new ParamNode(name); nowy->copyAttrsOf(this); return nowy;}
	TreeNode* IntNode::clone()      { IntNode * nowy = new IntNode(value); nowy->copyAttrsOf(this); return nowy;}
	TreeNode* StringNode::clone()   { StringNode * nowy = new StringNode(value);  nowy->copyAttrsOf(this); return nowy;}
	TreeNode* DoubleNode::clone()   { DoubleNode * nowy = new DoubleNode(value); nowy->copyAttrsOf(this); return nowy; }
	TreeNode* NameAsNode::clone()   { NameAsNode * nowy =  new NameAsNode((QueryNode*)arg->clone(), name, group); nowy->setUsedBy(this->getUsedBy()); nowy->copyAttrsOf(this); return nowy;}
	TreeNode* UnOpNode::clone()     { TreeNode * nowy = new UnOpNode((QueryNode*)arg->clone(), op);  nowy->copyAttrsOf(this); return nowy;}
	TreeNode* CoerceNode::clone()     { TreeNode * nowy = new CoerceNode((QueryNode*)arg->clone(), cType);  nowy->copyAttrsOf(this); return nowy;}
	TreeNode* AlgOpNode::clone()    { TreeNode * nowy = new AlgOpNode((QueryNode*)larg->clone(), (QueryNode*)rarg->clone(), op); nowy->copyAttrsOf(this); return nowy;}
	TreeNode* NonAlgOpNode::clone() { TreeNode * nowy = new NonAlgOpNode((QueryNode*)larg->clone(), (QueryNode*)rarg->clone(), op); nowy->copyAttrsOf(this); return nowy;}
	TreeNode* TransactNode::clone() { TreeNode * nowy = new TransactNode(op); nowy->copyAttrsOf(this); return nowy;}
    TreeNode* DMLNode::clone() { TreeNode * nowy = new DMLNode(inst); nowy->setOid(this->getOid()); return nowy;}

    TreeNode* CreateNode::clone()   {
		TreeNode *cloned = NULL;
		if (arg != NULL)
			cloned = new CreateNode(name, (QueryNode*)arg->clone());
		else
			cloned = new CreateNode(name, NULL);
		cloned->copyAttrsOf(this);
		return cloned;
	}
    TreeNode* LinkNode::clone() {return new LinkNode(nodeName, ip, port); }
    TreeNode* CondNode::clone() {
		TreeNode *cloned = NULL;
		if (rarg != NULL)
			cloned = new CondNode((QueryNode*)condition->clone(), (QueryNode*)larg->clone(), (QueryNode*)rarg->clone(), op);
		else
			cloned = new CondNode((QueryNode*)condition->clone(), (QueryNode*)larg->clone(), op);
		cloned->copyAttrsOf(this);
		return cloned;
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
		fn->copyAttrsOf(this);
		return fn;
    }

    TreeNode* ReturnNode::clone() {
    	if( query != NULL )
    		return new ReturnNode((QueryNode*) query->clone());
    	else
    		return new ReturnNode();
    }

    TreeNode* VirtualizeAsNode::clone() {
    	if (query != NULL) {
    		if (sub_query != NULL) {
    			return new VirtualizeAsNode((QueryNode*) query->clone(), (QueryNode*) sub_query->clone(), name);
    		}
    		else return new VirtualizeAsNode((QueryNode*) query->clone(), name);
    	}
    	else return new VirtualizeAsNode();
    }

    TreeNode* ThrowExceptionNode::clone() { return new ThrowExceptionNode((QueryNode*) query->clone()); }
    TreeNode* ProcedureNode::clone() { return new ProcedureNode(name, code, params, paramsNumb); }
    TreeNode* RegisterProcNode::clone() { return new RegisterProcNode((QueryNode*) query->clone()); }
    TreeNode* RegisterViewNode::clone() { return new RegisterViewNode((QueryNode*) query->clone()); }
    TreeNode* RegisterClassNode::clone() { return new RegisterClassNode((QueryNode*) query->clone(), getCreateType()); }
    //TreeNode* AsInstanceOfNode::clone() { return new AsInstanceOfNode((QueryNode*) objectsQuery->clone(), (QueryNode*) classQuery->clone()); }
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
		//vn->copyAttrsOf(this);
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
	for (unsigned int i=0; i < objects.size(); i++) {
		vn->objects.push_back(objects[i]);
	}
	//vn->copyAttrsOf(this);
	return vn;
    }

    TreeNode* ClassNode::clone() {
        ClassNode *cn = new ClassNode(name);
        cn->createType = createType;
        cn->invariant = invariant;
        cn->extends = (extends == NULL)?NULL:dynamic_cast<NameListNode*>(extends->clone());
        cn->fields = (fields == NULL)?NULL:dynamic_cast<NameListNode*>(fields->clone());
        cn->staticFields = (staticFields == NULL)?NULL:dynamic_cast<NameListNode*>(staticFields->clone());
        for (unsigned int i=0; i < procedures.size(); i++) {
            cn->procedures.push_back(dynamic_cast<QueryNode*>(procedures[i]->clone()));
        }
		for (unsigned int i=0; i < staticProcedures.size(); i++) {
            cn->staticProcedures.push_back(dynamic_cast<QueryNode*>(staticProcedures[i]->clone()));
        }
        return cn;
    }

    /*TreeNode* ClassCastNode::clone() {
    	ClassCastNode* ccn = new ClassCastNode(name, (queryToCast == NULL)?NULL:dynamic_cast<QueryNode*>(queryToCast->clone()));
    	return ccn;
    }*/

    TreeNode* ClassCastNode::clone() {
    	return new ClassCastNode(
    		(getObjectsQuery() == NULL)?NULL:dynamic_cast<QueryNode*>(getObjectsQuery()->clone()),
    		(getClassesQuery() == NULL)?NULL:dynamic_cast<QueryNode*>(getClassesQuery()->clone()));
    }

    /*TreeNode* InstanceOfNode::clone() {
    	InstanceOfNode* ion = new InstanceOfNode(
    		(queryToCheck == NULL)?NULL:dynamic_cast<QueryNode*>(queryToCheck->clone()),
    		(classes == NULL)?NULL:dynamic_cast<QueryNode*>(classes->clone()));
    	return ion;
    }*/

    TreeNode* InstanceOfNode::clone() {
    	return new InstanceOfNode(
    		(getObjectsQuery() == NULL)?NULL:dynamic_cast<QueryNode*>(getObjectsQuery()->clone()),
    		(getClassesQuery() == NULL)?NULL:dynamic_cast<QueryNode*>(getClassesQuery()->clone()));
    }

    TreeNode* ExcludesNode::clone() {
    	return new ExcludesNode(
    		getClassesQuery()==NULL ? NULL : dynamic_cast<QueryNode*>(getClassesQuery()->clone()),
    		getObjectsQuery()==NULL ? NULL : dynamic_cast<QueryNode*>(getObjectsQuery()->clone()) );
    }

    TreeNode* IncludesNode::clone() {
    	return new ExcludesNode(
    		getClassesQuery()==NULL ? NULL : dynamic_cast<QueryNode*>(getClassesQuery()->clone()),
    		getObjectsQuery()==NULL ? NULL : dynamic_cast<QueryNode*>(getObjectsQuery()->clone()) );
    }
	/** Typecheck clones...*/
	TreeNode* SignatureNode::clone() {
		TreeNode * retNode = NULL;
		if (arg != NULL) retNode = new SignatureNode((StructureTypeNode *)arg->clone());
		else {
			if (this->sigKind == SignatureNode::atomic) retNode = new SignatureNode(sigKind, atomType);
			else retNode = new SignatureNode(sigKind, typeName);
		}
		retNode->copyAttrsOf(this);
		return retNode;
	}
	TreeNode* ObjectDeclareNode::clone() {
		TreeNode * retNode = NULL;
		if (signature != NULL) retNode = new ObjectDeclareNode(name, (SignatureNode *)signature->clone(), card);
		else retNode = new ObjectDeclareNode(name, NULL, card);
		retNode->copyAttrsOf(this);
		return retNode;
	}
	TreeNode* StructureTypeNode::clone() {
		StructureTypeNode *newStrct = new StructureTypeNode();
		newStrct->setSubDeclarations(subDeclarations);
		newStrct->copyAttrsOf(this);
		return newStrct;
	}
	TreeNode* TypeDefNode::clone() {
		TreeNode * retNode = NULL;
		if (signature != NULL) retNode = new TypeDefNode(name, (SignatureNode *)signature->clone(), isDistinct);
		else retNode = new TypeDefNode(name, NULL, isDistinct);
		retNode->copyAttrsOf(this);
		return retNode;
	}
	TreeNode* SimpleCastNode::clone() {
		TreeNode *retNode = NULL;
		if (arg != NULL && sig != NULL) retNode = new SimpleCastNode((QueryNode *)arg->clone(), (SignatureNode *)sig->clone());
		else retNode = new SimpleCastNode (NULL, NULL);
		retNode->copyAttrsOf(this);
		return retNode;
	}
	/** Typecheck clones end. */

	void TreeNode::copyAttrsOf(TreeNode *tn) {
		coerceFlag = tn->isCoerced();
		for (int i = 0; i < tn->nrOfCoerceActions(); i++) {
			coerceActions.push_back(tn->getCoerceAction(i));
		}
		setOid(tn->getOid());
		setCoerceSig(tn->getCoerceSig());
		//setCard(tn->getCard()); //wasn't done previously in clone, so not doing it here. MH
	}
	bool TreeNode::needsCoerce(int actionId) {
		if (not isCoerced()) return false;
		for (unsigned int i = 0; i < coerceActions.size(); i++) {
			if (coerceActions.at(i) == actionId) return true;
		}
		return false;
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
    
    InterfaceBind::InterfaceBind(string interfaceName, string implementationName, InterfaceInnerLinkageList *arbLinks)
    : m_interfaceName(interfaceName), m_implementationName(implementationName), m_arbLinks(arbLinks) {};

    InterfaceBind::~InterfaceBind() {
	if (m_arbLinks)
	    delete m_arbLinks;
    };

    TreeNode* InterfaceBind::clone() {
	InterfaceInnerLinkageList *cpy = new InterfaceInnerLinkageList(*m_arbLinks);
	return new InterfaceBind(m_interfaceName, m_implementationName, cpy);
    };
    
    string InterfaceBind::getInterfaceName() {
	return m_interfaceName;
    };
    
    string InterfaceBind::getImplementationName() {
	return m_implementationName;
    };

    InterfaceInnerLinkage::InterfaceInnerLinkage(string intName, string impName) {
	this->nameAtInterface = intName;
	this->nameAtImplementation = impName;
    };

    InterfaceInnerLinkage::~InterfaceInnerLinkage() {};

    TreeNode* InterfaceInnerLinkage::clone() {
	return new InterfaceInnerLinkage(nameAtInterface, nameAtImplementation);
    };

    InterfaceInnerLinkageList::InterfaceInnerLinkageList(InterfaceInnerLinkage *linkage, InterfaceInnerLinkageList *lst) {
	this->linkage = linkage;
	this->linkageList = lst;
    };

    InterfaceInnerLinkageList::~InterfaceInnerLinkageList() {
	if (linkage)
	    delete linkage;
	if (linkageList)
	    delete linkageList;
    };

    TreeNode* InterfaceInnerLinkageList::clone() {
    if (linkageList)
	return new InterfaceInnerLinkageList((InterfaceInnerLinkage *)linkage->clone(), (InterfaceInnerLinkageList *)linkageList->clone());
    else
	return new InterfaceInnerLinkageList((InterfaceInnerLinkage *)linkage->clone(), NULL);
    };





    RegisterInterfaceNode::RegisterInterfaceNode(QueryNode *q) {
	this->query = q;
    };

    RegisterInterfaceNode::~RegisterInterfaceNode() {
	if (query!=NULL)
	    delete query;
    };

    TreeNode* RegisterInterfaceNode::clone() {
	return new RegisterInterfaceNode((QueryNode*) query->clone());
    };
    
    InterfaceNode::InterfaceNode(string interfaceName, string objectName, InterfaceStruct* iStruct) :
    m_interfaceName(interfaceName), m_objectName(objectName), m_iStruct(iStruct) {};
    
    InterfaceNode::~InterfaceNode() {
	if (m_iStruct)
	    delete m_iStruct;
    };
    
    
    TreeNode *InterfaceNode::clone() {
	return new InterfaceNode(m_interfaceName, m_objectName, (InterfaceStruct *) m_iStruct->clone());
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
	if ((attributeList) && (methodList))
	    return new InterfaceStruct((InterfaceAttributeListNode *)attributeList->clone(), (InterfaceMethodListNode *) methodList->clone());
	if (attributeList)
	    return new InterfaceStruct((InterfaceAttributeListNode *)attributeList->clone(), NULL);
	if (methodList)
	    return new InterfaceStruct(NULL, (InterfaceMethodListNode *) methodList->clone());
	return new InterfaceStruct(NULL, NULL);
    };
    
    vector<InterfaceMethod *> InterfaceStruct::getMethods() const {
	vector<InterfaceMethod *> vec;
	InterfaceMethod *met;
	InterfaceMethodListNode *metList = this->get_methodList();
	while (metList != NULL) {
	    met = metList->get_method();
	    metList = metList->get_methodList();
	    vec.push_back(met);
	}
	return vec;
    };
    
    vector<InterfaceAttribute *> InterfaceStruct::getAttribs() const {
	vector<InterfaceAttribute *> vec;
	InterfaceAttribute *attrib;
	InterfaceAttributeListNode *attrList = this->get_attributeList();
	while (attrList != NULL) {
	    attrib = attrList->get_attribute();
	    attrList = attrList->get_attributeList();
	    vec.push_back(attrib);
	}
	return vec;
    };

    vector<InterfaceAttribute *> InterfaceMethod::getMethodParams() {
	vector<InterfaceAttribute *> vec;
	InterfaceAttribute *attrib;
	InterfaceMethodParamListNode *mpList = m_params;
	while (mpList != NULL) {
	    attrib = mpList->get_methodParam();
	    mpList = mpList->get_methodParamList();
	    vec.push_back(attrib);
	}
	return vec;
    };
    
    InterfaceAttribute::InterfaceAttribute(string name, SignatureNode* signature) : m_name(name), m_signature(signature) {};
    InterfaceAttribute::~InterfaceAttribute() {if (m_signature) delete m_signature;};
    TreeNode* InterfaceAttribute::clone() {
	SignatureNode *sigCpy = NULL;
	if (m_signature)
	    sigCpy = new SignatureNode(*m_signature);
	return new InterfaceAttribute(m_name, sigCpy);    
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
    if (attributeList)
	return new InterfaceAttributeListNode((InterfaceAttribute *)attribute->clone(), (InterfaceAttributeListNode *)attributeList->clone());
    else
	return new InterfaceAttributeListNode((InterfaceAttribute *)attribute->clone(), NULL);
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

    TreeNode* InterfaceMethodListNode::clone() {
    if (methodList)
	return new InterfaceMethodListNode((InterfaceMethod *)method->clone(), (InterfaceMethodListNode *)methodList->clone());
    else
	return new InterfaceMethodListNode((InterfaceMethod *)method->clone(), NULL);
    };

    InterfaceMethod::InterfaceMethod(string name, InterfaceMethodParamListNode *params, SignatureNode* signature)
    : m_name(name), m_params(params) {m_signature = signature;};
    
    InterfaceMethod::~InterfaceMethod() {
	if (m_params)
	    delete m_params;    
    };

    TreeNode *InterfaceMethod::clone() {
	InterfaceMethodParamListNode *clonedParams = m_params?(InterfaceMethodParamListNode *)m_params->clone():NULL;
	SignatureNode *clonedSignature = m_signature?(SignatureNode *)m_signature->clone():NULL;
	return new InterfaceMethod(m_name, clonedParams, clonedSignature);
    };

    InterfaceMethodParamListNode::InterfaceMethodParamListNode(InterfaceAttribute *methodParam, InterfaceMethodParamListNode *rest) {
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
    if (methodParamList)
	return new InterfaceMethodParamListNode((InterfaceAttribute *)methodParam->clone(), (InterfaceMethodParamListNode *)methodParamList->clone());
    else
	return new InterfaceMethodParamListNode((InterfaceAttribute *)methodParam->clone(), NULL);
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

    int NameListNode::namesFromUniqueList( set<string>*& names ) {
    	NameListNode *name_list_or_null = this->try_get_name_list();
    	if(names->end() != names->find(name)) {
    		return (ErrQParser | ENotUniqueNameList);
    	}
    	names->insert(name);
		while (name_list_or_null != NULL) {
		    string tmpName = name_list_or_null->get_name();
		    if(names->end() != names->find(tmpName)) {
    			return (ErrQParser | ENotUniqueNameList);
    		}
    		names->insert(tmpName);
		    name_list_or_null = name_list_or_null->try_get_name_list();
		}
    	return 0;
    }

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
		Optimizer *optimiser = new Optimizer();

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



		return staticEval2(qres, envs);



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

	int NameNode::staticEval2 (StatQResStack *&qres, StatEnvStack *&envs) {
		vector<BinderWrap*> *vec = new vector<BinderWrap*>();

		Deb::ug("staticEval2(Name) \n");
		if (Deb::ugOn()) cout << name << endl;
		BinderWrap *bw = NULL;
		envs->bindName2(this->name, vec);
		if (vec->size() > 0){
			//bw = vec->at(0);	// pierwszy ktory zostal znaleziony, wynik powinien byc taki jak w starym
			bw = vec->at(vec->size()-1);	// teraz wynik powinien byc inny!
		}
		if (bw == NULL) { Deb::ug("name could NOT be bound ! will exit..\n"); return -1;}
		this->setBindSect(bw->getSectNumb());
		this->setStackSize(envs->getSize());
		this->setCard(bw->getBinder()->getCard());		// to jest zamiast wyw evalCard

		printf("size: %d \n", ((BinderWrap*)  envs->getElt(this->getBindSect())->getContent())->size());

		this->setStatEnvSecSize(((BinderWrap*)envs->getElt(this->getBindSect())->getContent())->size());
		cout << "jsi after setting StatEnvSecSize\n";
		Deb::ug("name bound on envs, section nr set to %d, ", bw->getSectNumb());
		//Deb::ug("ok:%d, ", ((SigRef *) (bw->getBinder()->getValue()))->getRefNo());
		Signature *sig = bw->getBinder()->getValue()->clone();	//death

		if (sig->getDependsOn() == NULL){	// nie wiem czy to jest potrzebne i czy wogole ma sens - ???
			sig->setDependsOn(this);		// ta sygnatura zalezy od nazwy ?? ktora jest w niej wiazana ??
		} else {							// a moze inaczej - zalezy od nazwy kora ja wklada na stos ?? taka interpretacja bylaby lepsza
			Deb::ug("jsi death sygnatura juz miala ustawione dependsOn, nie ustawia go ponownie\n");
		}

		for (unsigned int i = 1; i < vec->size(); i++){ // dodaje do sygnatury pozostale
			BinderWrap *pombw = vec->at(i);

			if (sig->type() == Signature::SSTRUCT) {
				((SigColl *) sig)->addToMyList (pombw->getBinder()->getValue()->clone());
			} else
			{
				SigColl *s = new SigColl(Signature::SSTRUCT);
				s->setElts(sig); s->addToMyList(pombw->getBinder()->getValue()->clone());
				sig = s;
			}
		}

		qres->pushSig (sig);
		 this->setDependsOn(bw->getBinder()->getDependsOn());		// tak bylo kiedys, nie wiem czy to jest poprawne, co z pozostalymi z vec ?? w ktorych tez jest wiazana ??
		Deb::ug("result pushed on qres - end nameNode stEval\n");
		calculateBoundUsed(vec);
		return 0;
	}

	void NameNode::calculateBoundUsed(vector<BinderWrap*> *vec){
		cout << "calculateBoundUsed " << vec->size() << endl;
		if (QueryParser::getStatEvalRun() == 1){
			for (unsigned int i = 0; i < vec->size(); i++){
				cout << i << " " << vec->at(i)->getBinder()->getName() << " binder: " << endl;
				vec->at(i)->getBinder()->putToString() ;
				cout << endl << "zalezy od: " << endl;
				if (vec->at(i)->getBinder()->getDependsOn() != NULL){
					vec->at(i)->getBinder()->getDependsOn()->putToString();
				} else cout << "NULL";

				cout << endl;
				TreeNode * wiazanyW = vec->at(i)->getBinder()->getDependsOn();
				this->getBoundIn()->push_back(wiazanyW);
				if (NULL != wiazanyW){
					if (wiazanyW->type() == TreeNode::TNNAME){
						((NameNode*) wiazanyW)->getUsedBy()->push_back(this);
					} else if (wiazanyW->type() == TreeNode::TNAS){
						((NameAsNode*) wiazanyW)->getUsedBy()->push_back(this);
					} else {
						cout << "ERROR BBBBBBBBBLLLLLLLLLAAAAAAAAAADDDDDDDDDDDD w 	NameNode::calculateBoundUsed                 ERROR\n";
						cerr << "ERROR BBBBBBBBBLLLLLLLLLAAAAAAAAAADDDDDDDDDDDD w 	NameNode::calculateBoundUsed                 ERROR\n";
					}
				}
			}
		} else
			cout << "to jest " << QueryParser::getStatEvalRun() << " przebieg statycznej ewaluacji" << endl;
		cout << "---------------------------------------------" << endl;
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
				DataObjectDef *obj = DataScheme::dScheme(-1)->getObjById (((SigRef *)topSig)->getRefNo());
				if (obj == NULL) return -1;
				/*error: invalid object def id. */
				string objKind = obj->getKind();
				if (objKind == "complex") {
					//Deb::ug("eval::UNOP::deref -- can't handle deref(complex)");
					return 0;
					return -1;}	// deref moze byc zupelnie przezroczysty
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

                DataObjectDef *obj = DataScheme::dScheme(-1)->getObjById (((SigRef *)topSig)->getRefNo());

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
			// uwaga ten fragment kodu jest skopiowany do NameNode::staticEval jezeli cos bedzie zmieniane, moze trzeba tez tam
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
			if (envs->pushBinders(toPush->statNested(toPush->getDependsOn())) == 0)
				sToPop = 1;
			else
				sToPop = 0;
		}
		if (toPush->getDependsOn() == NULL)
			cout << "the signature depends on NULL !!!!!\n";
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

	int SimpleCastNode::staticEval (StatQResStack *&qres, StatEnvStack *&envs) {
		Deb::ug("SimpleCastNode::staticEval()");
		if (this->arg->staticEval(qres, envs) == -1) return -1;
		//This node may be either ignored - than do nothing more, leave the qres as it was.
		//(instead of popping a sig and pushing its clone back on the stack!)
		// OR : could pop qres, and push a createSignature(this->getSig())... given createSignature(SigNode*).
		//qres->pop();
		//Signature *sig = this->getSig()->createSignature();	//based on dScheme, for type reference...
		//if (sig == NULL) return -1;
		//qres->pushSig(sig);
		this->evalCard();
		return 0;
	}

	int CoerceNode::staticEval (StatQResStack *&qres, StatEnvStack *&envs) {
		Deb::ug("CoerceNode::staticEval()");
		if (this->arg->staticEval(qres, envs) == -1) return -1;
		Signature *topSig = qres->topSig();
		qres->pop();
		Signature *sig = NULL;
		switch (cType) {
			case CoerceNode::to_string : {
				if (!topSig->isAtom()) return -1;
				sig = new SigAtomType("string");
				sig->copyAttrsOf(topSig);
				qres->pushSig(sig);
				break;
			}
			case CoerceNode::to_double : {
				if (!topSig->isAtom()) return -1;
				sig = new SigAtomType("double");
				sig->copyAttrsOf(topSig);
				qres->pushSig(sig);
				break;
			}
			case CoerceNode::to_int : {
				if (!topSig->isAtom()) return -1;
				sig = new SigAtomType("int");
				sig->copyAttrsOf(topSig);
				qres->pushSig(sig);
				break;
			}
			case CoerceNode::to_bool : {
				if (!topSig->isAtom()) return -1;
				sig = new SigAtomType("boolean");
				sig->copyAttrsOf(topSig);
				qres->pushSig(sig);
				break;
			}
			case CoerceNode::element : {
				topSig->setCard("1..1");	// collKind autmatically set to "".
				qres->pushSig(topSig->clone());
				break;
			}
			case CoerceNode::to_bag : {
				topSig->setCardMultiple();
				topSig->setCollKind("bag");
				qres->pushSig(topSig->clone());
				break;
			}
			case CoerceNode::to_seq : {
				topSig->setCardMultiple();
				topSig->setCollKind("list");
				qres->pushSig(topSig->clone());
                break;
			}
			default: qres->pushSig(topSig->clone()); break;
		}/*switch*/
		this->evalCard();
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

	DMLNode::DMLNode (dmlInst _inst) : inst(_inst) {/*DataScheme::reloadDScheme(); - done through executor!*/}
	void TreeNode::qpLocalAction(QueryParser *qp) {/*By default does nothing*/}
	void DMLNode::qpLocalAction(QueryParser *qp) {
		switch (inst) {
			case DMLNode::reload : break;
			case DMLNode::tcoff : qp->setTcOffTmp(true); break;
			case DMLNode::tcon : qp->setTcOffTmp(false); break;
		}
	}

bool TreeNode::containsOid(long oid){
	vector<TreeNode*> *listVec = new vector<TreeNode*>();
	this->getInfixList(listVec);
	for(vector<TreeNode*>::iterator iter = listVec->begin(); iter != listVec->end(); iter++){
		if ((*iter)->getOid() == oid  ){
			return true;
		}
	}
	return false;
}

void TreeNode::getBoundIn(TreeNode *node, vector<TreeNode*> *treeVec, vector<TreeNode*> *boundVec){
	for(vector<TreeNode*>::iterator iter = treeVec->begin(); iter != treeVec->end(); iter++){
		if ((*iter)->type()==TreeNode::TNNAME  ){
			if ( ((NameNode*)(*iter))->getDependsOn() == node ){
				boundVec->push_back(*iter);
			}
		}
	}
}
// ustawia w tym drzewie used i needen na puste wektory
void TreeNode::resetUsedNeeded(TreeNode *qTree){

	vector<TreeNode*> * listVec = new vector<TreeNode*>();	// wszystkie wezly w drzewie
	qTree->getInfixList(listVec);
	for(vector<TreeNode*>::iterator iter = listVec->begin(); iter != listVec->end(); iter++){
		if ((*iter)->type() == TreeNode::TNNAME){
			// jsi_kom cout<< "resetUsedNeeded::usedBySize : " << ((NameNode*) (*iter))->getUsedBy()->size() << endl;
			((NameNode*) (*iter))->setUsedBy(new vector<TreeNode*>());
			((NameNode*) (*iter))->setBoundIn(new vector<TreeNode*>());
		} else if ((*iter)->type() == TreeNode::TNAS){
			((NameAsNode*) *iter)->setUsedBy(new vector<TreeNode*>());
		} else {

		}
	}
}

TreeNode* TreeNode::getNodeByOid(vector<TreeNode*>* listVec, long oid){
	for(vector<TreeNode*>::iterator iter = listVec->begin(); iter != listVec->end(); iter++){
		if ((*iter)->getOid() == oid) {

			return (*iter);
		}
	}
	return NULL;
}


/**
 *		TypeCheck specific begin...
 */

	int TwoArgsNode::augmentTreeDeref(bool derefLeft, bool derefRight) {
		if (derefLeft) {
			cout << "2argsNode::augmentTreeDeref left " << endl;
			UnOpNode *deref = new UnOpNode(this->getLArg(),UnOpNode::deref);
			this->setLArg(deref);
		}
		if (derefRight) {
			cout << "2argsNode::augmentTreeDeref RIGHT" << endl;
			UnOpNode *deref = new UnOpNode(this->getRArg(),UnOpNode::deref);
			this->setRArg(deref);
		}
		return 0;
	}

	int TwoArgsNode::augmentTreeCoerce(int coerceType, bool augLeft, bool augRight) {
		if (augLeft) {
			CoerceNode *coerce = new CoerceNode(this->getLArg(), coerceType);
			this->setLArg(coerce);
		}
		if (augRight) {
			CoerceNode *coerce = new CoerceNode(this->getRArg(), coerceType);
			this->setRArg(coerce);
		}
		return 0;
	}

	int SimpleCastNode::augmentTreeDeref(bool derefLeft, bool derefRight) {
		if (derefLeft) {
			cout << "SimpleCastNode::augmentTreeDeref left " << endl;
			UnOpNode *deref = new UnOpNode(this->arg,UnOpNode::deref);
			this->setArg(deref);
		}
		if (derefRight) {
			cout << "SimpleCastNode::augmentTreeDeref RIGHT - sig not derefed." << endl;
		}
		return 0;
	}

	int CreateNode::augmentTreeCoerce(int coerceType) {
		CoerceNode *coerce = new CoerceNode((QueryNode *)this->getArg(), coerceType);
		this->setArg(coerce);
		return 0;
	}

	int NameAsNode::augmentTreeCoerce(int coerceType) {
		CoerceNode *coerce = new CoerceNode((QueryNode *)this->getArg(), coerceType);
		this->setArg(coerce);
		return 0;
	}

	int UnOpNode::augmentTreeCoerce(int coerceType) {
		CoerceNode *coerce = new CoerceNode((QueryNode *)this->getArg(), coerceType);
		this->setArg(coerce);
		return 0;
	}

	int UnOpNode::augmentTreeDeref() {
		UnOpNode *deref = new UnOpNode(this->arg,UnOpNode::deref);
		this->setArg(deref);
		return 0;
	}

	int NameAsNode::augmentTreeDeref() {
		UnOpNode *deref = new UnOpNode(this->arg,UnOpNode::deref);
		this->setArg(deref);
		return 0;
	}

	void TwoArgsNode::canDerefSons(bool &canDerefL, bool &canDerefR) {
		canDerefL = this->getLArg()->canDerefNode();
		canDerefR = this->getLArg()->canDerefNode();
	}

	SignatureNode::~SignatureNode() {
		if (arg != NULL) delete arg;
	}
	StructureTypeNode::~StructureTypeNode() {
		for (unsigned int i=0; i < subDeclarations->size(); i++) {
			if (subDeclarations->at(i) != NULL) delete subDeclarations->at(i);
		}
		subDeclarations->clear();
		delete subDeclarations;
	}
	ObjectDeclareNode::~ObjectDeclareNode () {
		if (this->signature != NULL) delete this->signature;
	}

	int SignatureNode::putToString () {
		cout << "{sig" << this->sigKind << ";";
		switch (sigKind) {
			case SignatureNode::atomic : {cout << this->atomType; break;}
			case SignatureNode::ref : { cout << "ref " << this->typeName; break;}
			case SignatureNode::defType : { cout << this->typeName; break;}
			case SignatureNode::complex : { cout << "("; this->arg->putToString(); cout << ")"; break;}
			default: {cout << "_unkown_sig_"; break;}
		}
		cout << "}";
		return 0;
	}

	string SignatureNode::getHeadline() {
		switch (sigKind) {
			case SignatureNode::atomic :
				return "\"atomic\" as kind, \""+atomType+"\" as type";
			case SignatureNode::ref :
				return "\"link\" as kind, \""+typeName+"\" as refName";
			case SignatureNode::defType :
				return "\"defined_type\" as kind, \""+typeName+"\" as typeName";
			case SignatureNode::complex :
				return "\"complex\" as kind";
				default: return "";
		}
	}

	Signature *SignatureNode::createSignature() {

		BinderWrap *bwObj = DataScheme::dScheme(-1)->bindBaseObjects();
		cout << "bound base objects" << endl;
		BinderWrap *bwTypes = DataScheme::dScheme(-1)->bindBaseTypes();
		cout << "bound base types" << endl;
		if (sigKind != SignatureNode::defType) {
			cout << "not a deftype signaturenode\n";
			return this->recCreateSig(bwObj, bwTypes);
		}
		if (bwObj == NULL) {cout << "bwObj NULL" <<  endl; return NULL;}
		if (bwTypes == NULL) {cout << "bwTypes NULL" <<  endl; return NULL;}
		vector<BinderWrap*> *vec = new vector<BinderWrap*>();
		bwObj->bindName2(typeName, vec);
		if (DataScheme::bindError(vec)) //no object with this name...
			bwTypes->bindName2(typeName, vec);
		Signature *sig = DataScheme::extractSigFromBindVector(vec);
		if (sig == NULL) {cout << "returning null sig straight away, not bound\n"; return NULL;}
		if (sig->type() != Signature::SREF) return sig;
		return DataScheme::dScheme(-1)->signatureOfRef(((SigRef *)sig)->getRefNo());
	}

	Signature *SignatureNode::recCreateSig(BinderWrap *bwObj, BinderWrap *bwTypes) {
		string ret = "";
		Signature *sig;
		vector<BinderWrap*> *vec = new vector<BinderWrap*>();
		switch (sigKind) {
			case SignatureNode::atomic :
				sig = new SigAtomType(atomType);
				return sig;
			case SignatureNode::ref :
				bwObj->bindName2(typeName, vec);
				if (DataScheme::bindError(vec))
					bwTypes->bindName2(typeName, vec);
				sig = DataScheme::extractSigFromBindVector(vec);
				return sig;
			case SignatureNode::defType :
				bwTypes->bindName2(typeName, vec);
				sig = DataScheme::extractSigFromBindVector(vec);
				if (sig == NULL) return NULL;
				if (sig->type() != Signature::SREF) return sig;
				return DataScheme::dScheme(-1)->signatureOfRef(((SigRef *)sig)->getRefNo());
			case SignatureNode::complex :
				sig = new SigColl(Signature::SSTRUCT);
				vector<ObjectDeclareNode*> *subs = NULL;
				if (getStructArg() == NULL) return sig;
				subs = this->getStructArg()->getSubDeclarations();

				for (unsigned int i = subs->size(); i > 0; i--) {
					Signature *son = subs->at(i-1)->getSigNode()->createSignature();
					if (son == NULL) return NULL;
					((SigColl *)sig)->addToMyList(son);
				}
				return sig;
		}
		return NULL;
	}

	int StructureTypeNode::putToString () {
		for (unsigned int i = 0; i < subDeclarations->size(); i++) {
			if (i != 0) cout << ", ";
			subDeclarations->at(i)->putToString();
		}
		return 0;
	}

	void StructureTypeNode::setSubDeclarations(vector<ObjectDeclareNode*> *q) {
		for (unsigned int i = 0; i < q->size(); i++) {
			subDeclarations->push_back((ObjectDeclareNode *) q->at(i)->clone());
		}
	}

	string SignatureNode::deParse() {
		switch (sigKind) {
			case SignatureNode::atomic :
				return atomType;
			case SignatureNode::ref :
				return "ref "+typeName;
			case SignatureNode::defType :
				return typeName;
			case SignatureNode::complex :
				return getStructArg()->deParse();
		}
		return "_";
	}
	string StructureTypeNode::deParse() {
		string ret = "(";
		for (unsigned int i = 0; i < subDeclarations->size(); i++) {
			if (i != 0) ret += ", ";
			ret += subDeclarations->at(i)->deParse();
		}
		ret += ")";
		return ret;
	}
	string TypeDefNode::deParse() {
		string ret = "typedef ";
		ret += (isDistinct ? "distinct " : "") + name + " = " + getSigNode()->deParse();
		return ret;
	}
	string ObjectDeclareNode::deParse() {
		string ret = name + "[" + card + "]:" + getSigNode()->deParse();
		return ret;
	}
	int TreeNode::markTreeCoerce(int actionId, Signature *cSig) {
		if (actionId != -1) {
			coerceFlag = true;
			bool actionPresent = false;
			for (unsigned int i = 0; i < coerceActions.size(); i++) {
				if (coerceActions.at(i) == actionId) actionPresent = true;
			}
			if (not actionPresent)
				this->coerceActions.push_back(actionId);
		}
		if (cSig != NULL) {
			this->setCoerceSig(cSig->clone());
		}
		return 0;
	}


/**
 *		TypeCheck specific end.
 */

IndexSelectNode::IndexSelectNode(QueryNode *exact) {
	c = new ExactConstraints(exact);
}

IndexSelectNode::IndexSelectNode(IndexBoundaryNode *left, IndexBoundaryNode *right) {
	c = new TwoSideConstraints(left->inclusive, left->value, right->value, right->inclusive);
}

IndexSelectNode::IndexSelectNode(IndexBoundaryNode *left, bool right) {
	c = new LeftBoundedConstraints(left->inclusive, left->value);
}

IndexSelectNode::IndexSelectNode(bool left, IndexBoundaryNode *right) {
	c = new RightBoundedConstraints(right->value, right->inclusive);
}

string IndexSelectNode::deParse() {return " index " + getIndexName()+ " " + c->toString() + " ";}

}
