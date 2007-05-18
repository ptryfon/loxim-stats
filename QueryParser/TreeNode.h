#ifndef _TREE_NODE_H_
#define _TREE_NODE_H_

#include <iostream>

#include <vector>
#include <string>
#include <sstream>
#include "Stack.h"
#include "ClassNames.h"
#include "../Store/Store.h"
#include "Privilige.h"
#include "QueryParser.h"
//using namespace std;

namespace QParser {

// statement (the starting symbol)
    class TreeNode 
    {
#ifdef QUERY_CACHE
    private:
	bool isCacheable;
#endif
    protected:
    static long uv;
    long oid;	// zachowywane przy clone, jezeli beda konstruktory kopiujace tez powinny to kopiowac
    long id;	// unikatowy identyfikator obiektu, nie zachowywane przy clone i  konstr kopiujacych
   
	TreeNode* parent;
	bool needed;		// is not death
	string card;		// cardinality

      string getPrefixForLevel( int level, string name ) {
        string result = "";

        if( level > 0 ) {
          for( int i = 0; i < level; i++ ) {
            result += " |";
          }
          result += name + "\n";
          for( int i = 0; i < level-1; i++ ) {
            result += " |";
          }
          result += " +-";
        }

        return result;
      }


    public:
	enum TreeNodeType { TNINT, TNSTRING, TNDOUBLE, TNVECTOR, TNNAME, 
	    TNAS, TNUNOP, TNALGOP, TNNONALGOP, TNTRANS, TNCREATE, TNCOND, TNLINK, TNPARAM, TNFIX, 
	    TNPROC, TNCALLPROC, TNRETURN, TNREGPROC, TNVIEW, TNREGVIEW, TNVALIDATION, TNCREATEUSER, 	
	    TNREMOVEUSER, TNPRIVLIST, TNNAMELIST, TNGRANTPRIV, TNREVOKEPRIV, TNREMOTE, TNINDEXDDL, TNINDEXDML,
	    TNCREATEINTERFACENODE, TNINTERFACESTRUCT, TNINTERFACEATTRIBUTELISTNODE, TNINTERFACEATTRIBUTE, 
	    TNINTERFACEMETHODLISTNODE, TNINTERFACEMETHOD, TNINTERFACEMETHODPARAMLISTNODE, TNINTERFACEMETHODPARAM,
        TNREGCLASS, TNCLASS, TNDML};
	 
	 
	 
	    
	TreeNode() : parent(NULL) { 
		this->needed = false;
		this->setId(TreeNode::getUv());
		this->setOid(TreeNode::getUv());
		#ifdef QUERY_CACHE
			this->isCacheable = false;
		#endif
	}
	
	#ifdef QUERY_CACHE
		markCacheable() { this->isCacheable = true;}
	#endif
	
	long getOid(){return this->oid;};
	long getId(){return this->id;};
	void setOid(long _oid){this->oid = _oid;};
	void setId(long _id){this->id = _id;};
	static long getUv();
	
	TreeNode* getParent() { return parent; }
	void setParent(TreeNode* _parent) { parent = _parent; }
	virtual ~TreeNode() {};

	virtual TreeNode* clone()=0;
	virtual int type()=0;
	virtual int putToString()=0;
	virtual void serialize(){this->putToString();};
	

    // na wniosek Executora:
	virtual string getName() {return (string) NULL;}
	virtual TreeNode* getArg() {return (TreeNode *) NULL;}
	virtual TreeNode* getLArg() {return (TreeNode *) NULL;}
	virtual TreeNode* getRArg() {return (TreeNode *) NULL;}
	virtual int getSectionNum(){ return -1;};  /* Nodes other than nonalgops don't hold this info. */
	virtual TreeNode* factorSubQuery (TreeNode *subT, string newName);
//	virtual TreeNode* optimizeByFactoring ();
	virtual int swapSon (TreeNode *oldSon, TreeNode *newSon) {
	/*should call an error. Subclasses that have at least 1 arg override this method.*/
	    return -1;}
	virtual string randomString();
	virtual int staticEval (StatQResStack *&qres, StatEnvStack *&envs) {return -1;}	
	virtual int optimizeTree () {Deb::ug( "type: %d.", type());	return 0;}
	/* should overridden in subclasses...*/
	/* AND should have an argument for the data scheme ... ??? !!! */
	
	virtual string getCard(){return this->card;}
	virtual void setCard(string _card){this->card = _card;}
	virtual void evalCard(){this->setCard("1..1");}
	virtual string mulCard(string leftCard, string rightCard);
	virtual int minCard(string card);
	virtual int maxCard(string card);
	virtual string int2card(int card);
	virtual bool getNeeded(){return this->needed;}
	virtual void setNeeded(bool _needed) {this->needed = _needed;}
	virtual void markNeeded(){
		if (Deb::ugOn()) cerr << "jsi unimplemented markNeeded" << this->getName() << endl;
	};
	virtual void markNeededUp(){
		this->setNeeded(true);
		if (this->getParent() != NULL){
			this->getParent()->markNeededUp();	
		}	
	}
	virtual TreeNode * getDeath(){	
		return NULL;
	}
	
	virtual void getInfixList(vector<TreeNode*> *auxVec){
		
	}
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) { return ""; }
    };

// statement := query
    class QueryNode : public TreeNode 
    {
    public:
	virtual TreeNode* clone()=0;
	virtual int type()=0;
	virtual int putToString()=0;
	virtual string deParse()=0;
	virtual void getInfixList(vector<TreeNode*> *auxVec){
		auxVec->push_back(this);
	}
    };
    
    class NameNode;
    
    class ValidationNode: public TreeNode {
	protected:
	    string login;
	    string passwd;
	public:

	    ValidationNode(string login, string passwd);

	    /* getters */
	    virtual string get_login();
	    virtual string get_passwd();
	    
	    /* inherited functions */
	    virtual TreeNode* clone();
	    virtual int type();
	    virtual int putToString();
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        return getPrefixForLevel( level, name ) + "[Validation] login='" + login + "', passwd='" + passwd + "'\n";
      }
    };
    
    
    
    
    /* Interface Nodes */
    
    
    class InterfaceMethodParam: public TreeNode {
	private:
	    string valueName;
	    string typeName;
	public:
	    InterfaceMethodParam(string valueName, string typeName);
	    virtual ~InterfaceMethodParam();
	    string getValueName() {return valueName;};
	    string getTypeName() {return typeName;};
	    
	    virtual TreeNode* clone();
	    virtual int type() {return TNINTERFACEMETHODPARAM;};
	    virtual int putToString() {cout << "InterfaceMethodParam value =" + valueName + " type = " + typeName + "\n"; return 0;};
	    virtual string toString(int level = 0, bool recursive = false, string name = "") {
		string result = getPrefixForLevel( level, name) + "[MethodParam] + " + valueName + " : " + typeName + "\n";
		return result;
	    }
    };
    
    
    
    class InterfaceMethodParamListNode: public TreeNode {
	private:
	    InterfaceMethodParam *methodParam;
	    InterfaceMethodParamListNode *methodParamList;
	public:
	    InterfaceMethodParamListNode(InterfaceMethodParam *methodParam, InterfaceMethodParamListNode *methodParamList = NULL);
	    virtual ~InterfaceMethodParamListNode();
	    virtual InterfaceMethodParam* get_methodParam() {return methodParam;};
	    virtual InterfaceMethodParamListNode* get_methodParamList() {return methodParamList;};
	    virtual TreeNode* clone();
	    virtual int type() {return TNINTERFACEMETHODPARAMLISTNODE;};
	    virtual int putToString() {cout << "InterfaceMethodParamListNode \n"; return 0;};
	    virtual string toString(int level = 0, bool recursive = false, string name = "") {
		string methodParamVal = methodParam ? methodParam->toString() : "<null>";
		string result = getPrefixForLevel( level, name) + "[MethodParamList] methodParam=" + methodParamVal + "\n";
		if (recursive) {
		    if (methodParamList) 
			result += methodParamList->toString(level+1, true, "methodParamList");
		}
		return result;
	    }
    };

    class InterfaceMethod: public TreeNode {
	private:
	    string methodName;
	    InterfaceMethodParamListNode *methodParams;
	public:
	    InterfaceMethod(string methodName, InterfaceMethodParamListNode *methodParams = NULL);
	    virtual ~InterfaceMethod();
	    virtual string getMethodName() {return methodName;};
	    virtual InterfaceMethodParamListNode* get_methodParams() {return methodParams;};
	    
	    virtual TreeNode* clone();
	    virtual int type() {return TNINTERFACEMETHOD;};
	    virtual int putToString() {cout << "InterfaceMethod name = " + methodName + "\n"; return 0;};
	    virtual string toString(int level = 0, bool recursive = false, string name = "") {
		string methodParamsString = methodParams ? methodParams->toString() : "<null>";
		string result = getPrefixForLevel( level, name) + "[Method] + " + methodName + " ( " + methodParamsString + " ) " + "\n";
		
		return result;	    
	    }
    };
    

    class InterfaceMethodListNode: public TreeNode {
	private:
	    InterfaceMethod *method;
	    InterfaceMethodListNode *methodList;
	public:
	    InterfaceMethodListNode(InterfaceMethod *method, InterfaceMethodListNode *methodList = NULL);
	    virtual ~InterfaceMethodListNode();
	    virtual InterfaceMethod* get_method() {return method;};
	    virtual InterfaceMethodListNode* get_methodList() {return methodList;};
	    virtual TreeNode* clone();
	    virtual int type() {return TNINTERFACEMETHODLISTNODE;};
	    virtual int putToString() {cout << "InterfaceMethodListNode \n"; return 0;};
	    virtual string toString(int level = 0, bool recursive = false, string name = "") {
		string methodVal = method ? method->toString() : "<null>";
		string result = getPrefixForLevel( level, name) + "[MethodList] method=" + methodVal + "\n";
		if (recursive) {
		    if (methodList) 
			result += methodList->toString(level+1, true, "methodList");
		}
		return result;
	    }
    };
    
    class InterfaceAttribute: public TreeNode {
	private:
	    string valueName;
	    string typeName;
	public:
	    InterfaceAttribute(string valueName, string typeName);
	    virtual ~InterfaceAttribute();
	    virtual string getValueName() {return valueName;};
	    virtual string getTypeName() {return typeName;};
	    
	    virtual TreeNode* clone();
	    virtual int type() {return TNINTERFACEATTRIBUTE;};
	    virtual int putToString() {cout << "InterfaceAttribute valueName = " + valueName + " type =" + typeName + "\n"; return 0;};
	    virtual string toString(int level = 0, bool recursive = false, string name = "") {
		string result = getPrefixForLevel( level, name) + "[Attribute] + " + valueName + " : " + typeName + "\n";
		return result;
	    }
    };
    
    
    class InterfaceAttributeListNode: public TreeNode {
	private:
	    InterfaceAttribute *attribute;
	    InterfaceAttributeListNode *attributeList;
	public:
	    InterfaceAttributeListNode(InterfaceAttribute *attribute, InterfaceAttributeListNode *attributeList = NULL);
	    virtual ~InterfaceAttributeListNode();
	    virtual InterfaceAttribute* get_attribute() {return attribute;};
	    virtual InterfaceAttributeListNode* get_attributeList() {return attributeList;};
	    virtual TreeNode* clone();
	    virtual int type() {return TNINTERFACEATTRIBUTELISTNODE;};
	    virtual int putToString() { cout << "InterfaceAttributeListNode \n"; return 0;};
	    virtual string toString(int level = 0, bool recursive = false, string name = "") {
		string attributeVal = attribute ? attribute->toString() : "<null>";
		string result = getPrefixForLevel( level, name) + "[AttributeList] attribute=" + attributeVal + "\n";
		if (recursive) {
		    if (attributeList) 
			result += attributeList->toString(level+1, true, "attributeList");
		}
		return result;
	    }
    };
    
    class InterfaceStruct: public TreeNode {
    private:
	InterfaceAttributeListNode* attributeList;
	InterfaceMethodListNode* methodList;
    public: 
	InterfaceStruct(InterfaceAttributeListNode* attributeList, InterfaceMethodListNode* methodList = NULL);
	virtual ~InterfaceStruct();
	virtual InterfaceAttributeListNode* get_attributeList() {return attributeList;};
	virtual InterfaceMethodListNode* get_methodList() {return methodList;};
	virtual TreeNode* clone();
	virtual int type() {return TNINTERFACESTRUCT;};
	virtual int putToString() {cout << "InterfaceStruct\n"; return 0;};
	virtual string toString(int level = 0, bool recursive = false, string name = "") {
	    string attributeListString = attributeList ? attributeList->toString() : "<null>";
	    string methodListString = methodList ? methodList->toString() : "<null>";
	    string result = getPrefixForLevel(level, name) + "[InterfaceStruct] + " + attributeListString + " " + methodListString + "\n";
	    
	    return result;
	}    
    };
    
    class CreateInterfaceNode : public TreeNode {
    private:
	string interfaceName;
	InterfaceStruct* iStruct;
    public:
	CreateInterfaceNode(string interfaceName, InterfaceStruct* iStruct);
	virtual ~CreateInterfaceNode();
	virtual InterfaceStruct* get_iStruct() {return iStruct;};
	virtual string get_interfaceName() {return interfaceName;};
	virtual TreeNode* clone();
	virtual int type() {return TNCREATEINTERFACENODE;};
	virtual int putToString() {cout << "CreateInterfaceNode name=" + interfaceName + "\n"; return 0;};
	virtual string toString(int level = 0, bool recursive = false, string name = "") {
	    string InterfaceStructString = iStruct ? iStruct->toString() : "<null>";
	    string result = getPrefixForLevel(level, name) + "[CreateInterfaceNode] + " + interfaceName + " "  + InterfaceStructString + "\n";
	    
	    return result;
	}    
    };
    
    
    /* Privilige Nodes */
    
    
    
    
    
    class PriviligeListNode: public TreeNode {
	private:
	    Privilige *priv;
	    PriviligeListNode *priv_list;
	public:
	    PriviligeListNode(Privilige *priv, PriviligeListNode *priv_list = NULL);
	    virtual ~PriviligeListNode();
	    
	    /* getters */
	    virtual Privilige* get_priv();
	    virtual PriviligeListNode* try_get_priv_list();

	    /* inherited functions */
	    virtual TreeNode* clone();
	    virtual int type();
	    virtual int putToString();
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        string privVal = priv ? priv->get_priv_name() : "<null>";
        string result = getPrefixForLevel( level, name ) + "[PriviligeList] priv=" + privVal + "\n";

        if( recursive ) {
          if( priv_list )
            result += priv_list->toString( level+1, true, "priv_list" );
        }

        return result;
      }
    };
    
    class NameListNode: public TreeNode {
	private:
	    string name;
	    NameListNode *name_list;
	public:
	    NameListNode(string name, NameListNode *list = NULL);
	    virtual ~NameListNode();
	    
	    /* getters */
	    virtual string get_name();
	    virtual NameListNode* try_get_name_list();

	    /* inherited functions */
	    virtual TreeNode* clone();
	    virtual int type();
	    virtual int putToString();
      virtual string toString( int level = 0, bool recursive = false, string n = "" ) {
        string result = getPrefixForLevel( level, n ) + "[NameList] name='" + name + "'\n";

        if( recursive ) {
          if( name_list )
            result += name_list->toString( level+1, true, "name_list" );
        }

        return result;
      }
    };
    
    class GrantPrivNode: public TreeNode {
	private:
	    PriviligeListNode 	*priv_list;
	    NameListNode	*name_list;
	    string 		user;
	    bool		with_grant_option;
	
	public:
	    GrantPrivNode(PriviligeListNode *priv_list, NameListNode *name_list, string user,bool with_grant_option);
	    virtual ~GrantPrivNode();
	    
	    /* getters */
	    virtual PriviligeListNode*  get_priv_list();
	    virtual NameListNode*	get_name_list();
	    virtual string		get_user();
	    virtual bool		get_with_grant_option();

	    /* inherited functions */
	    virtual TreeNode* clone();
	    virtual int type();
	    virtual int putToString();
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        string result = getPrefixForLevel( level, name ) + "[GrantPriv] user='" + user +
          "', with_grant_option=" + (with_grant_option ? "true" : "false" ) + "\n";

        if( recursive ) {
          if( priv_list )
            result += priv_list->toString( level+1, true, "priv_list" );
          if( name_list )
            result += name_list->toString( level+1, true, "name_list" );
        }

        return result;
      }
    };
    
    class RevokePrivNode: public TreeNode {
	private:
	    PriviligeListNode 	*priv_list;
	    NameListNode	*name_list;	
	    string 		user;
	    
	public:
	    RevokePrivNode(PriviligeListNode *priv_list, NameListNode *name_list, string user );
	    virtual ~RevokePrivNode();
	    
	    /* getters */
	    virtual PriviligeListNode* 	get_priv_list();
	    virtual NameListNode*	get_name_list();
	    virtual string		get_user();	    

	    /* inherited functions */
	    virtual TreeNode* clone();
	    virtual int type();
	    virtual int putToString();
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        string result = getPrefixForLevel( level, name ) + "[RevokePriv] user='" + user + "'\n";

        if( recursive ) {
          if( priv_list )
            result += priv_list->toString( level+1, true, "priv_list" );
          if( name_list )
            result += name_list->toString( level+1, true, "name_list" );
        }

        return result;
      }
    };

    class CreateUserNode: public TreeNode {
	private:
	    string user;
	    string passwd;
	
	public:
	    CreateUserNode(string user, string passwd);
	    
	    /* getters */
	    virtual string get_user();
	    virtual string get_passwd();
	    
	    /* inherited functions */
	    virtual TreeNode* clone();
	    virtual int type();
	    virtual int putToString();	    

      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        return getPrefixForLevel( level, name ) + "[CreateUser] user='" + user + "', passwd='" + passwd + "'\n";
      }
    };
    
    class RemoveUserNode: public TreeNode {
	private:
	    string user;
	
	public:
	    RemoveUserNode(string user);
	    
	    /* getters */
	    virtual string get_user();
	    
	    /* inherited functions */
	    virtual TreeNode* clone();
	    virtual int type();
	    virtual int putToString();	    
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        return getPrefixForLevel( level, name ) + "[RemoveUser] user='" + user + "'\n";
      }
    };


// nodes that have two sons (one may be a NULL) - both alg. and non-alg.
    class TwoArgsNode : public QueryNode
    {

    protected:
	QueryNode* larg;
	QueryNode* rarg;
    public:
	virtual int putToString()=0;
	QueryNode* getLArg() { return larg; }
	QueryNode* getRArg() { return rarg; }

	virtual void setLArg(QueryNode* _larg) {larg = _larg;larg->setParent(this);}
	virtual void setRArg(QueryNode* _rarg) {rarg = _rarg;rarg->setParent(this);}

	virtual void markNeeded(){
			this->setNeeded(true);
			this->getLArg()->markNeeded();
			this->getRArg()->markNeeded();
	}
	
	virtual TreeNode * getDeath(){
		TreeNode *pom = this->getLArg()->getDeath();
		if (pom != NULL) 
			return pom;
		return this->getRArg()->getDeath();
	}

	virtual int swapSon (TreeNode *oldSon, TreeNode *newSon) {
		newSon->setParent(this);
    	    if (this->getLArg() == oldSon) {
    		this->setLArg ((QueryNode *) newSon);	return 0;} 
	    else if (this->getRArg() == oldSon) {
    		this->setRArg ((QueryNode *) newSon); return 0;} 
	    else { /*an error should be reported - oldSon is not my son!*/ 
    		return -1;}        
    }	
    virtual void getInfixList(vector<TreeNode*> *auxVec){
		this->getLArg()->getInfixList(auxVec);
		auxVec->push_back(this);
		this->getRArg()->getInfixList(auxVec);
	}
    };
    
// atomic nodes with string/int/double/bool value
    class ValueNode : public QueryNode 
    {

	public:
	virtual int putToString() {
	    cout << "(_value_)";    
	    return 0;
	}	
	virtual void serialize(){cout << "'value'";};
	virtual int staticEval (StatQResStack *&qres, StatEnvStack *&envs);		
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        return getPrefixForLevel( level, name ) + "[Value]\n";
      }
      virtual void markNeeded(){
      		this->setNeeded(true);
      }
    };    

// query := name
    class NameNode : public QueryNode 
    {
    protected:
	string name;
	int bindSect;
	int stackSize;
	TreeNode *dependsOn;		// for removing death subqueries
			// can be either other name node or group as node
			// node in which static nested result this NameNode is binded
			// in dependsOn is NULL means that this NameNode is binded in base sections
	vector<TreeNode*> *boundIn;	// wezly drzewa w ktorych jest wiazane np(EMP group as 'aaa', DEPT group as 'aaa').aaa - aaa jest wiazane 2 razy
	vector<TreeNode*> *usedBy;	// wezly drzewa ktore sa w nim wiazane
	int statEnvSecSize;			// rozmiar sekcji w ktorej jest wiazany, co jezeli 1, a elementem jest sigcoll?
								// moze trzeba dorobic metode size w sygnaturze zwracajaca 1, przedef w sigcoll zwracajaca sume po size dla wszystkich elem na jej liscie
    public:
	NameNode(string _name) : name(_name) {bindSect = 0; stackSize = 0; dependsOn = NULL; this->usedBy = new vector<TreeNode*>; this->boundIn = new vector<TreeNode*>;}
	virtual TreeNode* getDependsOn(){return this->dependsOn;}
	virtual void setDependsOn(TreeNode *_dependsOn){this->dependsOn = _dependsOn;}
	virtual int getBindSect() { return this->bindSect;}
	virtual int getStackSize() {return this->stackSize;}
	virtual void setBindSect(int newBSect) { this->bindSect = newBSect;}
	virtual void setStackSize(int newSize) { this->stackSize = newSize;}
	virtual vector<TreeNode*> *getBoundIn(){return this->boundIn;}
	virtual void setBoundIn(vector<TreeNode*>* _boundIn) {this->boundIn = _boundIn;}	
	virtual vector<TreeNode*> *getUsedBy(){return this->usedBy;}
	virtual void setUsedBy(vector<TreeNode*>* _usedBy) {this->usedBy = _usedBy;}	
	void calculateBoundUsed(vector<BinderWrap*> *vec);	// wstawia do this->boundIn i dodaje sie do wektorow usedBy w ktorych jest wiazany
														// nie zmienia wlasnego usedBy
	virtual void setStatEnvSecSize(int size){statEnvSecSize = size;};
	virtual int  getStatEnvSecSize(){return statEnvSecSize;};
	virtual TreeNode* clone();
	virtual int type() { return TreeNode::TNNAME; }
	virtual string getName() { return name; }
	virtual int putToString() {
	    cout << "("<< this->getName() <<"["<<bindSect << "," << stackSize << "]";   
	    cout << this->getCard()<<"depOn:" ;
	    if (this->dependsOn != NULL)
	    	cout << (this->dependsOn->getName()); 
	    else cout <<"NULL";
	    cout << ":" << (this->getNeeded()?"needed":"notNeeded");
	    cout << ")";
	    return 0;
	}
	virtual void serialize(){cout << this->getName();};
	virtual void markNeeded(){
		this->setNeeded(true);
		if (this->getDependsOn() != NULL){
			this->getDependsOn()->markNeededUp();
		}
	}
	virtual void markNeededUp(){
		this->setNeeded(true);
		if (this->getParent() != NULL){
			this->getParent()->markNeededUp();	
		}	
		if (this->getDependsOn() != NULL){
			this->getDependsOn()->markNeededUp();
		}
	}
	
//	virtual void evalCard(){;}	// w nameNode biore to z bindera - przy wiazaniu
	
	virtual int staticEval (StatQResStack *&qres, StatEnvStack *&envs);	
	virtual int staticEval2 (StatQResStack *&qres, StatEnvStack *&envs);
      virtual string toString( int level = 0, bool recursive = false, string n = "" ) {
        stringstream c;
        string bindSectS, stackSizeS;
        c << bindSect; c >> bindSectS;
        c << stackSize; c >> stackSizeS;
        return getPrefixForLevel( level, n ) + "[Name] name='" + name + "', bindSect=" + bindSectS  + ", stackSize=" + stackSizeS + "\n";
      }
	
	virtual ~NameNode() {}
	
	virtual string deParse() { string result; result = " " + name + " "; return result; };
    };  


// query := name
    class ParamNode : public QueryNode 
    {
    protected:
	string name;
	int bindSect;
	int stackSize;
    public:
	ParamNode(string _name) : name(_name) {bindSect = 0; stackSize = 0;}

	virtual int getBindSect() { return this->bindSect;}
	virtual int getStackSize() {return this->stackSize;}
	virtual void setBindSect(int newBSect) { this->bindSect = newBSect;}
	virtual void setStackSize(int newSize) { this->stackSize = newSize;}
	
	virtual TreeNode* clone();
	virtual int type() { return TreeNode::TNPARAM; }
	virtual string getName() { return name; }
	virtual int putToString() {
	    cout << "("<< this->getName() <<"["<<bindSect << "," << stackSize << "])";    
	    return 0;
	}
	virtual void serialize(){cout << this->getName();};
	//virtual int staticEval (StatQResStack *&qres, StatEnvStack *&envs);	
	
	virtual ~ParamNode() {}
	
      virtual string toString( int level = 0, bool recursive = false, string n = "" ) {
        stringstream c;
        string bindSectS, stackSizeS;
        c << bindSect; c >> bindSectS;
        c << stackSize; c >> stackSizeS;
        return getPrefixForLevel( level, n ) + "[Param] name='" + name + "', bindSect=" + bindSectS  + ", stackSize=" + stackSizeS + "\n";
      }
      
      virtual string deParse() { string result; result = " " + name + " "; return result; };
      
    };  



// query := integer
    class IntNode : public ValueNode 
    {
    protected:
	int value;
    public:
	IntNode(int _value) : value(_value) {}
	virtual TreeNode* clone();
	virtual int type() { return TreeNode::TNINT; }
	int getValue() {return value;}
  	virtual ~IntNode() {}
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        stringstream c;
        string valueS;
        c << value; c >> valueS;
        return getPrefixForLevel( level, name ) + "[Int] value=" + valueS  + "\n";
      }
    virtual void serialize(){cout << this->getValue();};  
      virtual string deParse() { 
      	stringstream ss;
	string str;
	ss << value;
	ss >> str; 
	string result; 
	result = " " + str + " "; 
	return result; };
    };  

// query := string
    class StringNode : public ValueNode 
    {
    protected:
	string value;
    public:
	StringNode(string _value) : value(_value) {}
	virtual TreeNode* clone();
	virtual int type() { return TreeNode::TNSTRING; }
	string getValue() { return value; }
  	virtual ~StringNode() {}
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        return getPrefixForLevel( level, name ) + "[String] value=" + value  + "\n";
      }
      virtual void serialize(){cout << "\"" << this->getValue() <<"\"";};  
      virtual string deParse() { string result; result = " \"" + value + "\" "; return result; };
    };  

// query := double
    class DoubleNode : public ValueNode 
    {
    protected:
	double value;
    public:
	DoubleNode(double _value) : value(_value) {}
	virtual TreeNode* clone();
	virtual int type() { return TreeNode::TNDOUBLE; }
	double getValue() { return value; }        

	virtual ~DoubleNode() {}
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        stringstream c;
        string valueS;
        c << value; c >> valueS;
        return getPrefixForLevel( level, name ) + "[Double] value=" + valueS  + "\n";
      }
      virtual void serialize(){cout << this->getValue();};  
      virtual string deParse() { 
      	stringstream ss;
	string str;
	ss << value;
	ss >> str; 
	string result; 
	result = " " + str + " "; 
	return result; };
    };  

// TODO: query := bool??? czy tylko powstaje w wyniku jakichs rzeczy .. ? 
// CZY TO W OGOLE POTRZEBNE GDZIEKOLWIEK ? ! 
    class BoolNode : public QueryNode 
    {
    protected:
	bool value;
    public:
	BoolNode(bool _value) : value(_value) {}
	virtual TreeNode* clone();
	virtual int type() { return TreeNode::TNDOUBLE; }
	bool getValue() { return value; }        
	virtual int putToString() {
	    cout << "("<< this->getValue() <<")";    
	    return 0;
	}
	virtual int staticEval (StatQResStack *&qres, StatEnvStack *&envs);		
	virtual ~BoolNode() {}
	virtual void serialize(){cout << this->getValue();};  
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        return getPrefixForLevel( level, name ) + "[Bool] value=" + (value ? "true" : "false")  + "\n";
      }
    };      
    
// query := query "as" name | query "group as" name
    class NameAsNode : public QueryNode 
    {
    protected:
	QueryNode* arg;
	string name;
	bool group;
	vector<TreeNode*> *usedBy;	// wezly drzewa ktore sa w nim wiazane

    public:
	NameAsNode(QueryNode* _arg, string _name, bool _group)
            : arg(_arg), name(_name), group(_group) { arg->setParent(this); this->usedBy = new vector<TreeNode*>; }
	virtual TreeNode* clone();
	virtual int type() { return TreeNode::TNAS; }
	string getName() { return name; }
	TreeNode* getArg() { return arg; }
	virtual void setArg(QueryNode* _arg) { arg = _arg; arg->setParent(this); }    
	virtual vector<TreeNode*> *getUsedBy(){return this->usedBy;}
	virtual void setUsedBy(vector<TreeNode*>* _usedBy) {usedBy = _usedBy;}	
	bool isGrouped() { return group; }
	virtual int putToString() {
	    cout << "(";
	    if (arg!= NULL) arg->putToString();
	    else cout << "___";
	    if (this->isGrouped()) cout <<" group as ";
	    else cout << " as ";
	    
	    cout << this->getName() <<this->getCard()<<")";    
	    return 0;
	}
	virtual void serialize(){
	 	cout << "(";
	    if (arg!= NULL) arg->serialize();
	    else cout << "___";
	    if (this->isGrouped()) cout <<" group as '";
	    else cout << " as '";
	    
	    cout << this->getName() <<"')";    
	
	};  
	virtual int swapSon (TreeNode *oldSon, TreeNode *newSon) {
		newSon->setParent(this);
	    if (arg == oldSon) {this->setArg((QueryNode *) newSon); return 0;}
	    else {/*an error from errorConsole is called;*/ return -1;}
	}    	
	virtual void markNeeded(){
			this->setNeeded(true);
			this->getArg()->markNeeded();	
	}
	virtual void evalCard(){this->setCard(this->getArg()->getCard());}
	virtual TreeNode * getDeath(){
		return this->getArg()->getDeath();
	}
	virtual void getInfixList(vector<TreeNode*> *auxVec){
		auxVec->push_back(this);
		this->getArg()->getInfixList(auxVec);
	}

	virtual int staticEval(StatQResStack *&qres, StatEnvStack *&envs);
	virtual int optimizeTree() {return arg->optimizeTree();}
	virtual ~NameAsNode() { if (arg != NULL) delete arg; }
      virtual string toString( int level = 0, bool recursive = false, string n = "" ) {
        string result = getPrefixForLevel( level, n ) + "[NameAs] name='" + name + "', group=" + (group ? "true" : "false") + "\n";

        if( recursive ) {
          if( arg )
            result += arg->toString( level+1, true, "arg" );
        }

        return result;
      }
      
      virtual string deParse() { 
      	string result, tmp_res; 
	if (group) { tmp_res = "group as "; } 
	else  { tmp_res = "as "; }
	result = arg->deParse() + tmp_res + name + " "; 
	return result; }
    };  

// query := query InfixAlgOp query
    class UnOpNode : public QueryNode 
    {
    public:
	enum unOp { unMinus, count, sum, avg, min, max, distinct, boolNot, deleteOp, deref, ref, nameof};
    protected:
	QueryNode* arg;
	unOp op;
    public:
	UnOpNode(QueryNode* _arg, unOp _op) : arg(_arg), op(_op)
                                        { arg->setParent(this); }
	virtual TreeNode* clone();
	virtual int type() { return TreeNode::TNUNOP; }
	TreeNode* getArg() { return arg; }
	unOp getOp() { return op; }
	virtual void setArg(QueryNode* _arg) { arg = _arg; arg->setParent(this); }
	virtual void setOp(unOp _op) { op = _op; }

	virtual void markNeeded(){
			this->setNeeded(true);
			this->getArg()->markNeeded();	
	}
	
	virtual TreeNode * getDeath(){
		return this->getArg()->getDeath();
	}
	virtual void getInfixList(vector<TreeNode*> *auxVec){
		auxVec->push_back(this);
		this->getArg()->getInfixList(auxVec);
	}

      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        string result = getPrefixForLevel( level, name ) + "[UnOp] op=" + opStr() + "\n";

        if( recursive ) {
          if( arg )
            result += arg->toString( level+1, true, "arg" );
        }

        return result;
      }
	virtual int putToString() {
	    cout << "(";
	    cout << this->opStr() ;
	    if (arg!= NULL) arg->putToString();
	    else cout << "___";
	    
	    cout << ")";    
	    return 0;
	}
	virtual void serialize(){
		cout << "(";
	    cout << this->opStr() ;
	    if (arg!= NULL) arg->serialize();
	    else cout << "___";
	    
	    cout << ")"; 
	}
	virtual string opStr() {
	    int op = this->getOp();
	    switch (op) {
	    	case 0:	return "unMinus";
	    	case 1: return "count";
	    	case 2: return "sum";
	    	case 3: return "avg";
	    	case 4: return "min";
	    	case 5: return "max";
	    	case 6: return "distinct";
	    	case 7: return "boolNot";
	    	case 8: return "deleteOp";
	    	case 9: return "deref";
	    	case 10: return "ref";
	    	case 11: return "nameof";
	    	default: return "~~~";
	    }
	    	    
	}

	virtual int swapSon (TreeNode *oldSon, TreeNode *newSon) {
		newSon->setParent(this);
	    if (arg == oldSon) {this->setArg((QueryNode *) newSon); return 0;}
	    else {/*an error from errorConsole is called;*/ return -1;}
	}    	
	virtual int staticEval (StatQResStack *&qres, StatEnvStack *&envs);		
	virtual int optimizeTree() {return arg->optimizeTree();}
	virtual ~UnOpNode() { if (arg != NULL) delete arg; }
	
	virtual string deParse() { 
		string result = ""; 
		switch (op) {
			case UnOpNode::count:
				result = " count(" + arg->deParse() + ") "; 
				return result;
			case UnOpNode::sum:
				result = " sum(" + arg->deParse() + ") "; 
				return result;
			case UnOpNode::avg:
				result = " avg(" + arg->deParse() + ") "; 
				return result;
			case UnOpNode::min:
				result = " min(" + arg->deParse() + ") "; 
				return result;
			case UnOpNode::max:
				result = " max(" + arg->deParse() + ") "; 
				return result;
			case UnOpNode::deref:
				result = " deref(" + arg->deParse() + ") "; 
				return result;
			case UnOpNode::ref: 
				result = " ref(" + arg->deParse() + ") "; 
				return result;
			case UnOpNode::distinct:
				result = " distinct(" + arg->deParse() + ") ";
				return result;
			case UnOpNode::unMinus:
				result = " -" + arg->deParse() + " "; 
				return result;
			case UnOpNode::boolNot:
				result = " not " + arg->deParse() + " ";
				return result;
			case UnOpNode::deleteOp:
				result = " delete " + arg->deParse() + " "; 
				return result;
			case UnOpNode::nameof:
				result = " name(" + arg->deParse() + ") ";
				return result;
		}
		return result;
	};
    };  

  class VectorNode : public QueryNode
    {
    protected:	
	vector<QueryNode*> queries;
    public:
	VectorNode (QueryNode* singleQuery, VectorNode *tail) {
	    queries = tail->getQueries();
	    queries.push_back(singleQuery);
	    //delete tail;
	}
	VectorNode (QueryNode* singleQuery) {
	    queries.push_back(singleQuery);
	}
	VectorNode () {};

	virtual TreeNode* clone();
	virtual int type() {return TreeNode::TNVECTOR;};
	virtual vector<QueryNode*> getQueries() {return this->queries;}
	virtual QueryNode* getQuery(int i) {return this->queries.at(i);}
	virtual void setQueries(vector<QueryNode*> q) {queries = q;}
	virtual unsigned int howManyParts() { return queries.size(); }
	
	virtual ~VectorNode() {
		for (unsigned int i=0; i < queries.size(); i++) {
			if (queries[i] != NULL) delete queries[i]; }
		queries.clear();
	}
	virtual string toString( int level = 0, bool recursive = false, string _name = "" ) {
        	string result = "";
        	return result;
      	}
	
	virtual int putToString() { return 0; }	
	virtual string deParse() {
		string result = "";
        	return result;
	}
    };

// query := query InfixAlgOp query
    class AlgOpNode : public TwoArgsNode 
    {
    public:
	enum algOp { bagUnion, bagIntersect, bagMinus, plus, minus, times, divide,
   	         eq, refeq, neq, lt, gt, le, ge, boolAnd, boolOr, comma, insert, insertInto, assign, semicolon }; 
   	static QueryNode* newNode(VectorNode* vec, algOp _op) {
        	vector<QueryNode*> q = vec->getQueries();
        	unsigned int q_size = q.size();
        	if (q_size == 0) return NULL;
        	QueryNode* last_node = q[q_size - 1];
        	for (int i = (q_size - 2); i >= 0; i--) {
        		QueryNode* tmp_node = new AlgOpNode(q[i], last_node, _op);
        		last_node = tmp_node;
        	}
        	return last_node;
        }
    protected:
	QueryNode* larg;
	QueryNode* rarg;
	algOp op;
    public:
	AlgOpNode(QueryNode* _larg, QueryNode* _rarg, algOp _op)
		: larg(_larg), rarg(_rarg), op(_op) 
                { larg->setParent(this); rarg->setParent(this); }
	virtual TreeNode* clone();
	virtual int type() { return TreeNode::TNALGOP; }
	QueryNode* getLArg() { return larg; }
	QueryNode* getRArg() { return rarg; }
	algOp getOp() { return op; }
	virtual void setLArg(QueryNode* _larg) {larg = _larg;larg->setParent(this);}
	virtual void setRArg(QueryNode* _rarg) {rarg = _rarg;rarg->setParent(this);}
	virtual void setOp(algOp _op) { op = _op; }

      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        string result = getPrefixForLevel( level, name ) + "[AlgOp] op=" + opStr() + "\n";

        if( recursive ) {
          if( larg )
            result += larg->toString( level+1, true, "larg" );
          if( rarg )
            result += rarg->toString( level+1, true, "rarg" );
        }

        return result;
      }
	virtual int putToString() {
	    cout << "(";
	    if (larg!= NULL) larg->putToString();
	    else cout << "___";
	    cout << this->opStr()<<this->getCard()<<(this->getNeeded()?"Need":"Del");
	    if (rarg!= NULL) rarg->putToString();
	    else cout << "___";
	    
	    cout << ")";    
	    return 0;
	}
	virtual void serialize(){
		cout << "(";
	    if (larg!= NULL) larg->serialize();
	    else cout << "___";
	    cout << this->opStr();
	    if (rarg!= NULL) rarg->serialize();
	    else cout << "___";
	    
	    cout << ")"; 
	};  
	virtual string opStr() {
	    int op = this->getOp();
	    if (op == 0) return "algop";
	    if (op == 1) return "algop";
	    if (op == 2) return "algop";
	    if (op == 3) return "plus";
	    if (op == 4) return "minus";
	    if (op == 5) return "times";
	    if (op == 6) return "divide";
	    if (op == 7) return " = ";	    
	    if (op == 8) return " == ";
	    if (op == 9) return " != ";	    
	    if (op == 10) return " < ";	    
	    if (op == 11) return " > ";	    
	    if (op == 12) return " <= ";
	    if (op == 13) return " >= ";
	    if (op == 14) return " && ";
	    if (op == 15) return " || ";
	    if (op == 16) return " , ";	    
	    if (op == 17) return "insert";
	    if (op == 18) return ":<";
	    if (op == 19) return ":=";
	    if (op == 20) return " ; ";
	    return "~~~";
	}
	
	virtual TreeNode * getDeath(){	
		if (this->getOp() == AlgOpNode::comma){
			if((!this->getRArg()->getNeeded()&&(this->getRArg()->getCard()=="1..1"))){
				return this->getRArg();
			}
			if((!this->getLArg()->getNeeded()&&(this->getLArg()->getCard()=="1..1"))){
				return this->getLArg();
			}
		}
		return TwoArgsNode::getDeath();
	}
	virtual void evalCard(){
		if (this->opStr() != " , ")	this->setCard("1..1");
		else {
			this->setCard(this->mulCard(this->getLArg()->getCard(), this->getRArg()->getCard()));	
		};
	}
	virtual int staticEval (StatQResStack *&qres, StatEnvStack *&envs);	
	virtual int optimizeTree();
	virtual ~AlgOpNode() { if (larg != NULL) delete larg;
                         if (rarg != NULL) delete rarg; }
			 
	virtual string deParse() { 
		string result = ""; 
		switch (op) {
			case AlgOpNode::bagUnion: { result = larg->deParse() + "union" + rarg->deParse(); return result; }
			case AlgOpNode::bagIntersect: { result = larg->deParse() + "intersect" + rarg->deParse(); return result; }
			case AlgOpNode::bagMinus: { result = larg->deParse() + "minus" + rarg->deParse(); return result; }
			case AlgOpNode::plus: { result = larg->deParse() + "+" + rarg->deParse(); return result; }
			case AlgOpNode::minus: { result = larg->deParse() + "-" + rarg->deParse(); return result; }
			case AlgOpNode::times: { result = larg->deParse() + "*" + rarg->deParse(); return result; }
			case AlgOpNode::divide: { result = larg->deParse() + "/" + rarg->deParse(); return result; }
			case AlgOpNode::eq: { result = larg->deParse() + "=" + rarg->deParse(); return result; }
			case AlgOpNode::neq: { result = larg->deParse() + "!=" + rarg->deParse(); return result; }
			case AlgOpNode::lt: { result = larg->deParse() + "<" + rarg->deParse(); return result; }
			case AlgOpNode::gt: { result = larg->deParse() + ">" + rarg->deParse(); return result; }
			case AlgOpNode::le: { result = larg->deParse() + "<=" + rarg->deParse(); return result; }
			case AlgOpNode::ge: { result = larg->deParse() + ">=" + rarg->deParse(); return result; }
			case AlgOpNode::boolAnd: { result = larg->deParse() + "and" + rarg->deParse(); return result; }
			case AlgOpNode::boolOr: { result = larg->deParse() + "or" + rarg->deParse(); return result; }
			case AlgOpNode::comma: { result = larg->deParse() + "," + rarg->deParse(); return result; }
			case AlgOpNode::insertInto: { result = larg->deParse() + ":<" + rarg->deParse(); return result; }
			case AlgOpNode::assign: { result = larg->deParse() + ":=" + rarg->deParse(); return result; }
			case AlgOpNode::semicolon: { result = larg->deParse() + ";" + rarg->deParse(); return result; }
			case AlgOpNode::insert: { return result; }
			case AlgOpNode::refeq: { return result; }
		}
		return result;
	};
    };  

// query := query InfixNonalgOp query
    class NonAlgOpNode : public TwoArgsNode 
    {
    public:
	enum nonAlgOp { dot, join, where, closeBy, closeUniqueBy, leavesBy,
                  leavesUniqueBy, orderBy, exists, forAll, forEach };
    protected:
	QueryNode* larg;
	QueryNode* rarg;
	nonAlgOp op;
	int firstOpenSect;   /* nrs of the ENV section which was open by this operator */	

	int lastOpenSect;	
    public:
	NonAlgOpNode(QueryNode* _larg, QueryNode* _rarg, nonAlgOp _op)
                : larg(_larg), rarg(_rarg), op(_op)
                { larg->setParent(this); rarg->setParent(this); firstOpenSect = 0; 
lastOpenSect = 0; } 
	virtual TreeNode* clone();
	virtual int type() { return TreeNode::TNNONALGOP; }
	QueryNode* getLArg() { return larg; }
	QueryNode* getRArg() { return rarg; }
	nonAlgOp getOp() { return op; }
	virtual void setLArg(QueryNode* _larg) {larg = _larg;larg->setParent(this);}
	virtual void setRArg(QueryNode* _rarg) {rarg = _rarg;rarg->setParent(this);}
	virtual void setOp(nonAlgOp _op) { op = _op; }
	
	virtual void markNeeded(){
		this->setNeeded(true);
		this->putToString();
		this->getRArg()->markNeeded();	
		if (op != 0){		// not a dot
			this->getLArg()->markNeeded();	
		}
	}
	
	virtual TreeNode * getDeath(){	
		if (this->getOp() == NonAlgOpNode::join){
			if((!this->getRArg()->getNeeded())&&(this->getRArg()->getCard()=="1..1")){
				return this->getRArg();
			}
		}
		return TwoArgsNode::getDeath();
	}
	
	virtual void evalCard(){
		if (this->op == NonAlgOpNode::dot){
			this->setCard(this->mulCard(this->getLArg()->getCard(), this->getRArg()->getCard()));	
		} else if (this->op == NonAlgOpNode::join){
			this->setCard(this->mulCard(this->getLArg()->getCard(), this->getRArg()->getCard()));	
		} else if (this->op == NonAlgOpNode::where){
			this->setCard(this->getLArg()->getCard());
		} else {
			this->setCard("0..*");
		}
	}
	
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        stringstream c;
        string firstOpenSectS, lastOpenSectS;

        c << firstOpenSect; c >> firstOpenSectS;
        c << lastOpenSect; c >> lastOpenSectS;

        string result = getPrefixForLevel( level, name ) + "[NonAlgOp] op=" + opStr() +
          ", firstOpenSect='" + firstOpenSectS + "', lastOpenSect='" + lastOpenSectS + "'\n";

        if( recursive ) {
          if( larg )
            result += larg->toString( level+1, true, "larg" );
          if( rarg )
            result += rarg->toString( level+1, true, "rarg" );
        }

        return result;
      }
	virtual int getFirstOpenSect() {return this->firstOpenSect;}
	virtual void setFirstOpenSect(int newSectNr) {this->firstOpenSect = newSectNr;}
	virtual int getLastOpenSect() {return this->lastOpenSect;}
	virtual void setLastOpenSect(int newNr) {this->lastOpenSect = newNr;}
	
//	virtual Signature *combine(Signature *lRes, Signature *rRes);
	
	virtual int putToString() {
	    cout << "(";
	    if (larg!= NULL) larg->putToString();
	    else cout << "___";
	    cout << this->opStr() << this->getCard() << (this->getNeeded()?"Need":"Del");
		cout << "<" << firstOpenSect << ","<< lastOpenSect << ">";
	    if (rarg!= NULL) rarg->putToString();
	    else cout << "___";	    
	    cout << ")";    
	    return 0;
	}
	virtual void serialize(){
		 cout << "(";
	    if (larg!= NULL) larg->serialize();
	    else cout << "___";
	    cout << " " << this->opStr() << " ";
	    if (rarg!= NULL) rarg->serialize();
	    else cout << "___";	    
	    cout << ")";  
	}
	virtual string opStr() {
	    int op = this->getOp();
	    if (op == 0) return ".";
	    if (op == 1) return "join";
	    if (op == 2) return "where";
	    if (op == 3) return "closeBy";
	    if (op == 4) return "closeUniqueBy";
	    if (op == 5) return "leavesBy";
	    if (op == 6) return "leavesUniqueBy";
	    if (op == 7) return "orderBy";	    
	    if (op == 8) return "exists";	    
	    if (op == 9) return "forAll";
	    if (op == 10) return "forEach";	    
	    return "~~~";	}
	virtual int staticEval (StatQResStack *&qres, StatEnvStack *&envs);	
	virtual int optimizeTree();	
//	virtual TreeNode *getHighestIndependant(); nieaktualne... ?
	virtual ~NonAlgOpNode() { if (larg != NULL) delete larg;
                            if (rarg != NULL) delete rarg; }
			    
	virtual string deParse() { 
		string result = ""; 
		switch (op) {
			case NonAlgOpNode::dot: { result = larg->deParse() + "." + rarg->deParse(); return result; }
			case NonAlgOpNode::join: { result = larg->deParse() + "join" + rarg->deParse(); return result; }
			case NonAlgOpNode::where: { result = larg->deParse() + "where" + rarg->deParse(); return result; }
			case NonAlgOpNode::closeBy: { result = larg->deParse() + "close by" + rarg->deParse(); return result; }
			case NonAlgOpNode::closeUniqueBy: { result = larg->deParse() + "close unique by" + rarg->deParse(); return result; }
			case NonAlgOpNode::leavesBy: { result = larg->deParse() + "leaves by" + rarg->deParse(); return result; }
			case NonAlgOpNode::leavesUniqueBy: { result = larg->deParse() + "leaves unique by" + rarg->deParse(); return result; }
			case NonAlgOpNode::orderBy: { result = larg->deParse() + "order by" + rarg->deParse(); return result; }
			case NonAlgOpNode::exists: { result = larg->deParse() + "exists" + rarg->deParse(); return result; }
			case NonAlgOpNode::forAll: { result = larg->deParse() + "for all" + rarg->deParse(); return result; }
			case NonAlgOpNode::forEach: { result = " for each" + larg->deParse() + "do" + rarg->deParse() + "od "; return result; }
		}
		return result;
	}
    };  

    class TransactNode : public TreeNode 
    {
    public:
		enum transactionOp { begin, end, abort }; 
    protected:
		transactionOp op;
    public:
		TransactNode (transactionOp _op) : op(_op) {}
		virtual TreeNode* clone();
		virtual int type() { return TreeNode::TNTRANS; }
		transactionOp getOp() { return op; }
		virtual void setOp(transactionOp _op) { op = _op; }

      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        return getPrefixForLevel( level, name ) + "[Transact] op=" + ((op == begin) ? "begin" : ((op == end) ? "end" : "abort")) + "\n";
      }
        virtual int putToString() {
    	    cout << "(begin/end/abort)";
    	    return 0;
		}
		virtual ~TransactNode() {}
    };  

// query := CREATE NAME  |  CREATE NAME LEFTPAR query RIGHTPAR
    class CreateNode : public QueryNode 
    {
    protected:
		QueryNode* arg;
		string name;
    public:
		CreateNode(string _name, QueryNode* _arg = NULL)
	            : arg(_arg), name(_name)  {}
		
		CreateNode(QueryNode* _arg) : arg(_arg) {name = "";}
		virtual TreeNode* clone();
		virtual int type() { return TreeNode::TNCREATE; }
		string getName() { return name; }
		TreeNode* getArg() { return arg; }
		virtual void setArg(QueryNode* _arg) { arg = _arg; arg->setParent(this); }    
		virtual int putToString() {
		    cout << "(create " << this->getName();
		    if (arg != NULL) arg->putToString();
			cout << ")"; return 0;
		}
      virtual string toString( int level = 0, bool recursive = false, string n = "" ) {
        string result = getPrefixForLevel( level, n ) + "[Create] name='" + name + "'\n";

        if( recursive ) {
          if( arg )
            result += arg->toString( level+1, true, "arg" );
        }

        return result;
      }
		virtual int swapSon (TreeNode *oldSon, TreeNode *newSon) {
			newSon->setParent(this);
		    if (arg == oldSon) {this->setArg((QueryNode *) newSon); return 0;}
		    else {/*an error from errorConsole is called;*/ return -1;}
		}    		
	
        virtual ~CreateNode() { if (arg != NULL) delete arg; }
	
	virtual string deParse() { string result; result = " create" + arg->deParse(); return result; };
    };  


// query := WHILE q DO q  |  IF-THEN  |  IF-THEN-ELSE ...
    class CondNode : public QueryNode
    {
    public:
	enum condOp { ifElsee, iff, whilee };

    protected: 
	QueryNode* condition;
	QueryNode* larg;   /* if c then __QUERY__ (else q2) fi | while c do __QUERY__ od */
	QueryNode* rarg;   /* if c then q else __QUERY__ fi   (w p.p. rarg== NULL) */
	condOp op;
    public:
	CondNode (QueryNode* _condition, QueryNode* _larg, QueryNode* _rarg, condOp _op) 
	    : condition(_condition), larg(_larg), rarg(_rarg), op(_op)    
    	    {larg->setParent(this); rarg->setParent(this); }

	CondNode (QueryNode* _condition, QueryNode* _larg, condOp _op)
	    : condition(_condition), larg(_larg), op(_op)
	    {larg->setParent(this); rarg = NULL; }    
	virtual TreeNode* clone();
	virtual int type() {return TreeNode::TNCOND;}
	virtual QueryNode* getCondition() { return condition; }
	QueryNode* getLArg() { return larg; }
	QueryNode* getRArg() { return rarg; }
	condOp getOp() { return op; }
	
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        string result = getPrefixForLevel( level, name ) + "[Cond] op=" + ((op == ifElsee) ? "ifElses" : ((op == iff) ? "iff" : "whilee")) + "\n";

        if( recursive ) {
          if( condition )
            result += condition->toString( level+1, true, "condition" );
          if( larg )
            result += larg->toString( level+1, true, "larg" );
          if( rarg )
            result += rarg->toString( level+1, true, "rarg" );
        }

        return result;
      }
	virtual int putToString() {
	    cout << "(";
	    if (op == 0 || op == 1) {
		cout << "if ";
		if (condition != NULL) condition->putToString(); else cout << "_/conditiion/_";
		cout << " then ";
		if (larg != NULL) larg->putToString(); else cout << "_/query1/_";
		if (op == 0) {	cout << " else ";
    		    if (rarg != NULL) rarg->putToString(); else cout << "_/query2/_";
		}
		cout << " fi";
	    } else if (op == 2) { /*while*/
		cout << "while ";
		if (condition != NULL) condition->putToString(); else cout << "_/conditiion/_";
		cout << " do ";
		if (larg != NULL) larg->putToString(); else cout << "_/query1/_";
		cout << " od";	    
	    }
	    cout << ")";
	    return 0;
	}
	
	virtual ~CondNode() {if (larg != NULL) delete larg; if (rarg != NULL) delete rarg; if (condition != NULL) delete condition; }
	
	virtual string deParse() { 
		string result = ""; 
		switch (op) {
			case CondNode::iff: { 
				result = " if" + condition->deParse() + "then" + larg->deParse() + "fi "; 
				return result; }
			case CondNode::ifElsee: { 
				result = " if" + condition->deParse() + "then" + larg->deParse() + "else" + rarg->deParse() + "fi "; 
				return result; }
			case CondNode::whilee: { 
				result = " while" + condition->deParse() + "do" + larg->deParse() + "od "; 
				return result; }
		}
		return result;
	};
    };

// query := np.: link "Gdansk" "violet255" 3360 
    class LinkNode : public QueryNode
    {
    protected:
	string nodeName;	/* np. "Gdansk"*/
	string ip;		/* np. "125.100.100..." albo "yellow12"*/
	int port;		
    
    public: 
	LinkNode (string _nodeName, string _ip, int _port)
	    : nodeName(_nodeName), ip(_ip), port(_port) {}
	
	virtual TreeNode* clone();
	virtual int type() {return TreeNode::TNLINK;};
	virtual string getNodeName() {return this->nodeName;}
	virtual string getIp() {return this->ip;}
	virtual int getPort() {return this->port;}	
	
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        stringstream c;
        string portS;

        c << port; c >> portS;
        return getPrefixForLevel( level, name ) + "[Link] nodeName='" + nodeName + "', ip='" + ip + "', port=" + portS + "\n";
      }

	virtual int putToString() {
	    cout << "( link " << this->getNodeName() << " - " << this->getIp()<< " - " << this->getPort() << " )";
	    return 0;	
	}
	virtual ~LinkNode() {}
	
	virtual string deParse() { 
		stringstream ss;
		string str;
		ss << port;
		ss >> str; 
		string result; 
		result = " link \"" + nodeName + "\" \"" + ip + "\" " + str + " "; 
		return result; }
    };
    
    class RemoteNode : public TreeNode
    {
    	private:
    		LogicalID* lid;
    		bool deref;
    		
    	public:
    		virtual int type() {return TreeNode::TNREMOTE;}
    		virtual TreeNode* clone() {return new RemoteNode();}
    		virtual int putToString() {cout << "TreeNode" << endl; return 0;}
    		LogicalID* getLogicalID() {return lid;}
    		void setLogicalID(LogicalID* l) {lid = l;}
    		bool isDeref() {return deref;}
    		void setDeref(bool b) {deref = b;}
    };

    class FixPointNode : public QueryNode
    {
    protected:	
	vector<QueryNode*> queries;
	vector<string>  names;
	int partsNumb;
    public:
	FixPointNode () {}
	FixPointNode ( string singleName, QueryNode* singleQuery) {
	    queries.push_back(singleQuery);
	    names.push_back(singleName);
	    partsNumb = 1;
	}
	
	FixPointNode (string singleName, QueryNode* singleQuery, FixPointNode *tail) {
	    queries = tail->getQueries();
	    names = tail->getNames();
	    queries.push_back(singleQuery);
	    names.push_back(singleName);
	    partsNumb = 1 + tail->howManyParts();
	    	    // ... should I delete tail here...?
	}

      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        stringstream c;
        string partsNumbS;
        string namesS = "[";

        for( unsigned i = 0; i < names.size(); i++ ) {
          namesS += ((i > 0) ? ", " : "") + names[i];
        }
        namesS += "]";

        c << partsNumb; c >> partsNumbS;
        string result = getPrefixForLevel( level, name ) + "[FixPoint] partsNumb=" + partsNumbS + ", names=" + namesS + "\n";

        if( recursive ) {
          for( unsigned int i = 0; i < queries.size(); i++ ) {
            result += (queries[i])->toString( level+1, true, "query" );
          }
        }

        return result;
      }

	virtual TreeNode* clone();
	virtual int type() {return TreeNode::TNFIX;};
	virtual vector<QueryNode*> getQueries() {return this->queries;}
	virtual QueryNode* getQuery(int i) {return this->queries.at(i);}
	virtual vector<string> getNames() {return this->names;}
	virtual string getName(int i) {return this->names.at(i); }
	virtual int howManyParts() {return this->partsNumb;}
	virtual void setQueries(vector<QueryNode*> q) {queries = q;}
	virtual void setNames(vector<string> n) {names = n;}
	virtual void setPartsNumb(int n) {partsNumb = n;}
	virtual int putToString() {
	    cout << " Fixpoint {";
	    for (int i = 0; i < partsNumb; i++) {
		cout << "[" << names.at(i) << " :- ";
		queries.at(i)->putToString();
		cout << "] ";
	    }
	    cout << "} ";
	    return 0;
	}
	
	virtual ~FixPointNode() { 
		for (unsigned int i=0; i < queries.size(); i++) {
			if (queries[i] != NULL) delete queries[i]; } 
		queries.clear(); 
	}
    
	virtual string deParse() { 
		string result = " fixpoint ( "; 
		for (int i = 0; i < partsNumb; i++) {
			result = result + names[i] + " :-" + queries[i]->deParse();
		}
		result = result + ") ";
		return result; };
    };
    
    class ReturnNode : public QueryNode
    {
    protected:
	QueryNode* query;
    public:
    	ReturnNode() { this->query = NULL; }
	ReturnNode(QueryNode *q) { this->query = q; }
	
	virtual TreeNode* clone();
	virtual int type() {return TreeNode::TNRETURN;}
	virtual QueryNode *getQuery() {return this->query;}
	virtual int putToString() {
	    cout << " return < ";
	    if (query != NULL) query->putToString();
	    else cout << "_no_query_";
	    cout << ">";
	    return 0;
	}
	
	virtual ~ReturnNode() {if (query != NULL) delete query;} 
      
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        string result = getPrefixForLevel( level, name ) + "[Return]\n";

        if( recursive ) {
          if( query != NULL )
            result += query->toString( level+1, true, "query" );
        }

        return result;
      }
      
      virtual string deParse() { 
      	string result;
      	if( query != NULL )
      		result = " return " + query->deParse();
      	else
      		result = " break "; 
      	return result; 
      	};
    };
    
    class RegisterProcNode : public QueryNode
    {
    protected:
	QueryNode* query;
    public:
	RegisterProcNode(QueryNode *q) {this->query = q; }
	
	virtual TreeNode* clone();
	virtual int type() {return TreeNode::TNREGPROC;}
	virtual QueryNode *getQuery() {return this->query;}
	virtual int putToString() {
	    cout << " RegisterProc < ";
	    if (query != NULL) query->putToString();
	    else cout << "_no_query_";
	    cout << ">";
	    return 0;
	}
	
	virtual ~RegisterProcNode() {if (query != NULL) delete query;} 
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        string result = getPrefixForLevel( level, name ) + "[RegisterProc]\n";

        if( recursive ) {
          if( query )
            result += query->toString( level+1, true, "query" );
        }

        return result;
      }
      
      virtual string deParse() { string result; result = " create" + query->deParse(); return result; };
    };
    
    
    
    class ProcedureNode : public QueryNode
    {
    protected:
	vector<string> params;
	string name;
	QueryNode* code;
	unsigned int paramsNumb;
    public:
	ProcedureNode() { paramsNumb = 0; }
	ProcedureNode(string single_param) {
		params.push_back(single_param);
		paramsNumb = 1;
	}
	ProcedureNode(string single_param, ProcedureNode *tail) {
		params = tail->getParams();
		params.push_back(single_param);
		paramsNumb = 1 + tail->getParamsNumb();
		//delete tail;
	}
	ProcedureNode(string n, QueryNode* c, vector<string> p, unsigned int pN) {
		name = n;
		code = c;
		params = p;
		paramsNumb = pN;
	}
	
	virtual TreeNode* clone();
	virtual int type() {return TreeNode::TNPROC;}
	virtual ~ProcedureNode() {if (code != NULL) delete code; } 
	
	virtual void addContent(string n, QueryNode* c) {name = n; code = c;}
	virtual void addParam(string p) { params.push_back(p); paramsNumb++; }
	virtual string getName() { return name; }
	virtual QueryNode* getCode() { return code; }
	virtual vector<string> getParams() { return params; }
	virtual unsigned int getParamsNumb() { return paramsNumb; }
      
      	virtual string toString( int level = 0, bool recursive = false, string _name = "" ) {
        	string result = getPrefixForLevel( level, _name ) + "Procedure name - " + name + "\n";
		result += getPrefixForLevel( level + 1, _name ) + "ProcBody - " + code->deParse() + "\n";
        	for (unsigned int i = 0; i < paramsNumb; i++) {
			result += getPrefixForLevel( level + 1, _name ) + " Param - " + params[i] + "\n";
		}
        	return result;
      	}
	
	virtual int putToString() {
	    cout << "Procedure name - " << name << endl;
	    cout << "  ProcBody - " << code->deParse() << endl;
        for (unsigned int i = 0; i < paramsNumb; i++) {
		cout << "  Param - " << params[i] << endl;
	}	    
	    return 0;
	}
	
	virtual string deParse() { 
		string result = " procedure " + name + "("; 
		for (unsigned int i = 0; i < paramsNumb; i++) {
			result = result + params[i];
			if (i < (paramsNumb - 1)) result = result + ",";
		}
		result = result + ") {" + code->deParse() + "} ";
		return result; };
    }; 
    
    class CallProcNode : public QueryNode
    {
    protected:	
	vector<QueryNode*> queries;
	string name;
	unsigned int partsNumb;
    public:
	CallProcNode (string singleName) {
	    name = singleName;
	    partsNumb = 0;
	}
	CallProcNode (string singleName, VectorNode *vec) {
	    queries = vec->getQueries();
	    name = singleName;
	    partsNumb = vec->howManyParts();
	    //delete vec;
	}

      virtual string toString( int level = 0, bool recursive = false, string _name = "" ) {
        stringstream c;
        string partsNumbS;
        string namesS = "[" + name + "]";

        c << partsNumb; c >> partsNumbS;
        string result = getPrefixForLevel( level, name ) + "[CallProc] partsNumb=" + partsNumbS + ", name=" + namesS + "\n";

        if( recursive ) {
          for( unsigned int i = 0; i < queries.size(); i++ ) {
            result += (queries[i])->toString( level+1, true, "query" );
          }
        }

        return result;
      }

	virtual TreeNode* clone();
	virtual int type() {return TreeNode::TNCALLPROC;};
	virtual vector<QueryNode*> getQueries() {return this->queries;}
	virtual QueryNode* getQuery(int i) {return this->queries.at(i);}
	virtual string getName() {return this->name;}
	virtual unsigned int howManyParts() {return this->partsNumb;}
	virtual void setQueries(vector<QueryNode*> q) {queries = q;}
	virtual void setName(string n) {name = n;}
	virtual void setPartsNumb(int n) {partsNumb = n;}
	virtual int putToString() {
	    cout << " CallProc " << name << " (";
	    for (unsigned int i = 0; i < partsNumb; i++) {
		queries.at(i)->putToString();
		if (i < (partsNumb - 1)) cout << " , ";
	    }
	    cout << ") ";
	    return 0;
	}
	
	virtual ~CallProcNode() {
		for (unsigned int i=0; i < queries.size(); i++) {
			if (queries[i] != NULL) delete queries[i]; }
		queries.clear();
	}
	
    	virtual string deParse() { 
		string result = " " + name + "("; 
		for (unsigned int i = 0; i < partsNumb; i++) {
			result = result + queries[i]->deParse();
			if (i < (partsNumb - 1)) result = result + ",";
		}
		result = result + ") ";
		return result; };
    };



class RegisterViewNode : public QueryNode
    {
    protected:
	QueryNode* query;
    public:
	RegisterViewNode(QueryNode *q) {this->query = q; }
	
	virtual TreeNode* clone();
	virtual int type() {return TreeNode::TNREGVIEW;}
	virtual QueryNode *getQuery() {return this->query;}
	virtual int putToString() {
	    cout << " RegisterView < ";
	    if (query != NULL) query->putToString();
	    else cout << "_no_query_";
	    cout << ">";
	    return 0;
	}
	
	virtual ~RegisterViewNode() {if (query != NULL) delete query;} 
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        string result = getPrefixForLevel( level, name ) + "[RegisterView]\n";

        if( recursive ) {
          if( query )
            result += query->toString( level+1, true, "query" );
        }

        return result;
      }
      
      virtual string deParse() { 
      	string result; 
      	result = " create" + query->deParse(); 
      	cout << result;
      	return result; };
    };


class ViewNode : public QueryNode
    {
    protected:
	string name;
	ProcedureNode* virtual_objects;
	vector<QueryNode*> procedures;
	vector<QueryNode*> subviews;
    public:
	ViewNode(ViewNode *v) { subviews.push_back(v); }
	ViewNode(ProcedureNode *p) { procedures.push_back(p); }
	ViewNode(string n) { name = n; }
	
	virtual TreeNode* clone();
	virtual int type() {return TreeNode::TNVIEW;}
	virtual ~ViewNode() {
		for (unsigned int i=0; i < procedures.size(); i++) {
			if (procedures[i] != NULL) delete procedures[i]; }
		procedures.clear();
		for (unsigned int i=0; i < subviews.size(); i++) {
			if (subviews[i] != NULL) delete subviews[i]; }
		subviews.clear();
		if (virtual_objects != NULL) delete virtual_objects;
		
	} 
      
	virtual void setName(string n) { name = n; }
	virtual void setVirtual(string n, QueryNode* c) { 
		virtual_objects = new ProcedureNode();
		virtual_objects->addContent(n, c);
	} 
	virtual void addContent(ViewNode *v) {
		for (unsigned int i=0; i < v->procedures.size(); i++) {
			procedures.push_back(v->procedures[i]);
		}
		for (unsigned int i=0; i < v->subviews.size(); i++) {
			subviews.push_back(v->subviews[i]);
		}
	}
	virtual vector<QueryNode*> getProcedures() { return this->procedures; }
	virtual vector<QueryNode*> getSubviews() { return this->subviews; }
	virtual string getName() { return name; }
	virtual QueryNode* getVirtualObjects() { return virtual_objects; }
      
      	virtual string toString( int level = 0, bool recursive = false, string _name = "" ) {
        	string result = getPrefixForLevel( level, _name ) + "View name - " + name + "\n";
        	result = result + virtual_objects->toString( level+1, true, "query" );
		for (unsigned int i=0; i < procedures.size(); i++) {
			result = result + (procedures[i])->toString( level+1, true, "query" );
		}
		for (unsigned int i=0; i < subviews.size(); i++) {
			result = result + (subviews[i])->toString( level+1, true, "query" );
		}
        	return result;
      	}
	
	virtual int putToString() {
	    cout << "View name - " << name << endl;
	    virtual_objects->putToString();
	    for (unsigned int i=0; i < procedures.size(); i++) {
		procedures.at(i)->putToString();
	    }
	    for (unsigned int i=0; i < subviews.size(); i++) {
		subviews.at(i)->putToString();
	    }
	    return 0;
	}
	
	virtual string deParse() { 
		string result = " view " + name + " { virtual objects " + virtual_objects->getName();
		result = result + " { " + virtual_objects->getCode()->deParse() + " } ";
		for (unsigned int i=0; i < procedures.size(); i++) {
			result = result + procedures[i]->deParse();
		}
		for (unsigned int i=0; i < subviews.size(); i++) {
			result = result + subviews[i]->deParse();
		}
		result = result + " } ";
		cout << result;
		return result; };
    }; 


    

class RegisterClassNode : public QueryNode
    {
    protected:
    QueryNode* query;
    public:
    RegisterClassNode(QueryNode *q) {this->query = q; }
    
    virtual TreeNode* clone();
    virtual int type() {return TreeNode::TNREGCLASS;}
    virtual QueryNode *getQuery() {return this->query;}
    virtual int putToString() {
        cout << " RegisterClass < ";
        if (query != NULL) query->putToString();
        else cout << "_no_query_";
        cout << ">";
        return 0;
    }
    
    virtual ~RegisterClassNode() {if (query != NULL) delete query;} 
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        string result = getPrefixForLevel( level, name ) + "[RegisterClass]\n";

        if( recursive ) {
          if( query )
            result += query->toString( level+1, true, "query" );
        }

        return result;
      }
      
      virtual string deParse() { 
        string result; 
        result = " create" + query->deParse(); 
        cout << result;
        return result; };
    };


class ClassNode : public QueryNode
    {
    protected:
    string name;
    string invariant;
    NameListNode* extends;
    NameListNode* fields;
    //ProcedureNode* virtual_objects;
    vector<QueryNode*> procedures;
    //vector<QueryNode*> subviews;

    virtual void emptyInit(){
        extends = NULL; 
        fields = NULL;
    }
    public:
    //ViewNode(ViewNode *v) { subviews.push_back(v); }
    ClassNode() {emptyInit();}
    ClassNode(ProcedureNode *p) { procedures.push_back(p); emptyInit(); }
    ClassNode(string n) { name = n; emptyInit(); }

    virtual TreeNode* clone();
    virtual int type() {return TreeNode::TNCLASS;}
    virtual ~ClassNode() {
        for (unsigned int i=0; i < procedures.size(); i++) {
            if (procedures[i] != NULL) delete procedures[i]; }
        procedures.clear();
        /*for (unsigned int i=0; i < subviews.size(); i++) {
            if (subviews[i] != NULL) delete subviews[i]; }
        subviews.clear();
        if (virtual_objects != NULL) delete virtual_objects;
        */
    } 

    virtual void setExtends(NameListNode* extends) { this->extends = extends; }
    virtual void setFields(NameListNode* fields) { this->fields = fields; }
    virtual void setName(string n) { name = n; }
    virtual void setInvariant(string invariant) { this->invariant = invariant; }
    /*virtual void setVirtual(string n, QueryNode* c) { 
        virtual_objects = new ProcedureNode();
        virtual_objects->addContent(n, c);
    } */
    virtual void addContent(ClassNode *cn) {
        for (unsigned int i=0; i < cn->procedures.size(); i++) {
            procedures.push_back(cn->procedures[i]);
        }
        /*for (unsigned int i=0; i < v->subviews.size(); i++) {
            subviews.push_back(v->subviews[i]);
        }*/
    }
    virtual vector<QueryNode*> getProcedures() { return this->procedures; }
    //virtual vector<QueryNode*> getSubviews() { return this->subviews; }
    virtual string getName() { return name; }
    /*virtual QueryNode* getVirtualObjects() { return virtual_objects; }

        virtual string toString( int level = 0, bool recursive = false, string _name = "" ) {
            string result = getPrefixForLevel( level, _name ) + "View name - " + name + "\n";
            result = result + virtual_objects->toString( level+1, true, "query" );
        for (unsigned int i=0; i < procedures.size(); i++) {
            result = result + (procedures[i])->toString( level+1, true, "query" );
        }
        for (unsigned int i=0; i < subviews.size(); i++) {
            result = result + (subviews[i])->toString( level+1, true, "query" );
        }
            return result;
        }
    */
    virtual int putToString() {
        cout << "Class name - " << name;
        if(extends){
            cout << " extends " << extends->putToString() << endl;
        }
        if(!invariant.empty()){
            cout << " invariant " << invariant << endl;
        }
        if(fields){
            cout << " fields " << fields->putToString() << endl;
        }
        //virtual_objects->putToString();
        for (unsigned int i=0; i < procedures.size(); i++) {
            procedures.at(i)->putToString();
        }
        /*for (unsigned int i=0; i < subviews.size(); i++) {
        subviews.at(i)->putToString();
        }*/
        return 0;
    }

    virtual string deParse() { 
        string result = " class " + name + " ";
        for (unsigned int i=0; i < procedures.size(); i++) {
            result = result + procedures[i]->deParse();
        }
        /*for (unsigned int i=0; i < subviews.size(); i++) {
            result = result + subviews[i]->deParse();
        }*/
        result = result + " } ";
        cout << result;
        return result; 
    };
	}; 





class DMLNode : public TreeNode 
    {
    public:
		enum dmlInst { reload }; 
    protected:
		dmlInst inst;
    public:
		DMLNode (dmlInst _inst);
		virtual TreeNode* clone();
		virtual int type() { return TreeNode::TNDML; }
		dmlInst getInst() { return inst; }
		virtual void setInst(dmlInst _inst) { inst = _inst; }

        virtual int putToString() {
    	    cout << "dml reloadScheme";
    	    return 0;
		}
		virtual ~DMLNode() {}
    };  

}
#endif

