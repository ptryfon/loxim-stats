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
//using namespace std;

namespace QParser {

// statement (the starting symbol)
    class TreeNode 
    {
    protected:
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
	    TNPROC, TNCALLPROC, TNRETURN, TNVALIDATION, TNCREATEUSER, TNREMOVEUSER, TNPRIVLIST, TNNAMELIST,
	    TNGRANTPRIV, TNREVOKEPRIV, TNREMOTE};
	TreeNode() : parent(NULL) {this->needed = false;}
	TreeNode* getParent() { return parent; }
	void setParent(TreeNode* _parent) { parent = _parent; }
	virtual ~TreeNode() {};

	virtual TreeNode* clone()=0;
	virtual int type()=0;
	virtual int putToString()=0;
	

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
		cerr << "jsi unimplemented markNeeded" << this->getName() << endl;
		cerr << "jsi unimplemented markNeeded" << this->getName() << endl;
	};
	virtual void markNeededUp(){
		cerr << "jsi markNeededUP start\n";
		cout << "jsi markNeededUP start\n";
		this->setNeeded(true);
		cout << "jsi mnu parent == this " << (this->getParent() == this) << endl;
		cerr << "jsi mnu parent == this " << (this->getParent() == this) << endl;
		if (this->getParent() != NULL){
			cout << "jsi wola mnu w parencie\n";
			cerr << "jsi wola mnu w parencie\n";
			this->getParent()->markNeededUp();	
			cout << "jsi po mnu w parencie\n";
			cerr << "jsi po mnu w parencie\n";
		}	
		cerr << "jsi markNeededUP end\n";
		cerr << "jsi markNeededUP end\n";
	}
	virtual TreeNode * getDeath(){	
		return NULL;
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
			cerr << "jsi twoargsnode1\n"; 
			this->setNeeded(true);
			cerr << "jsi twoargsnode2\n";
			this->getLArg()->markNeeded();
			cerr << "jsi twoargsnode3\n";
			this->getRArg()->markNeeded();
			cerr << "jsi twoargsnode4\n";	
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
    };
    
// atomic nodes with string/int/double/bool value
    class ValueNode : public QueryNode 
    {

	public:
	virtual int putToString() {
	    cout << "(_value_)";    
	    return 0;
	}	
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
	
    public:
	NameNode(string _name) : name(_name) {bindSect = 0; stackSize = 0; dependsOn = NULL; }
	virtual TreeNode* getDependsOn(){return this->dependsOn;}
	virtual void setDependsOn(TreeNode *_dependsOn){this->dependsOn = _dependsOn;}
	virtual int getBindSect() { return this->bindSect;}
	virtual int getStackSize() {return this->stackSize;}
	virtual void setBindSect(int newBSect) { this->bindSect = newBSect;}
	virtual void setStackSize(int newSize) { this->stackSize = newSize;}
	
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
	virtual void markNeeded(){
		cerr<<"jsi markNeeded w namenode start " << name << endl;
		cout<<"jsi markNeeded w namenode start " << name << endl;
		this->setNeeded(true);
		cerr << "jsi namenode wola mnu \n";
		cout << "jsi namenode wola mnu \n";
	//	this->markNeededUp();				// idzie w kierunku korzenia 
		if (this->getDependsOn() != NULL){
			cout << "jsi wola w dependsOn\n";
			cerr << "jsi wola w dependsOn\n";
			this->getDependsOn()->markNeededUp();
			cout << "jsi po w dependsOn\n";
			cerr << "jsi po w dependsOn\n";
		}
			
		cerr<<"jsi markNeeded w namenode end " << name << endl;
		cout<<"jsi markNeeded w namenode end " << name << endl;
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
	
//	virtual void evalCard(){;}	// to musi byc wziete ze skladu danych - przy wiazaniu
	
	virtual int staticEval (StatQResStack *&qres, StatEnvStack *&envs);	
      virtual string toString( int level = 0, bool recursive = false, string n = "" ) {
        stringstream c;
        string bindSectS, stackSizeS;
        c << bindSect; c >> bindSectS;
        c << stackSize; c >> stackSizeS;
        return getPrefixForLevel( level, n ) + "[Name] name='" + name + "', bindSect=" + bindSectS  + ", stackSize=" + stackSizeS + "\n";
      }
	
	virtual ~NameNode() {}
	
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
	//virtual int staticEval (StatQResStack *&qres, StatEnvStack *&envs);	
	
	virtual ~ParamNode() {}
	
      virtual string toString( int level = 0, bool recursive = false, string n = "" ) {
        stringstream c;
        string bindSectS, stackSizeS;
        c << bindSect; c >> bindSectS;
        c << stackSize; c >> stackSizeS;
        return getPrefixForLevel( level, n ) + "[Param] name='" + name + "', bindSect=" + bindSectS  + ", stackSize=" + stackSizeS + "\n";
      }
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
    public:
	NameAsNode(QueryNode* _arg, string _name, bool _group)
            : arg(_arg), name(_name), group(_group) { arg->setParent(this); }
	virtual TreeNode* clone();
	virtual int type() { return TreeNode::TNAS; }
	string getName() { return name; }
	TreeNode* getArg() { return arg; }
	virtual void setArg(QueryNode* _arg) { arg = _arg; arg->setParent(this); }    
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
    };  

// query := query InfixAlgOp query
    class UnOpNode : public QueryNode 
    {
    public:
	enum unOp { unMinus, count, sum, avg, min, max, distinct, boolNot, deleteOp, deref, ref};
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
	
	virtual string opStr() {
	    int op = this->getOp();
	    if (op == 0) return "unMinus";
	    if (op == 1) return "count";	    
	    if (op == 2) return "sum";	    
	    if (op == 3) return "avg";
	    if (op == 4) return "min";
	    if (op == 5) return "max";
	    if (op == 6) return "distinct";
	    if (op == 7) return "boolNot";	    
	    if (op == 8) return "deleteOp";	    
	    if (op == 9) return "deref";	    
		if (op == 10) return "ref";
	    return "~~~";
	}

	virtual int swapSon (TreeNode *oldSon, TreeNode *newSon) {
		newSon->setParent(this);
	    if (arg == oldSon) {this->setArg((QueryNode *) newSon); return 0;}
	    else {/*an error from errorConsole is called;*/ return -1;}
	}    	
	virtual int staticEval (StatQResStack *&qres, StatEnvStack *&envs);		
	virtual int optimizeTree() {return arg->optimizeTree();}
	virtual ~UnOpNode() { if (arg != NULL) delete arg; }
    };  

// query := query InfixAlgOp query
    class AlgOpNode : public TwoArgsNode 
    {
    public:
	enum algOp { bagUnion, bagIntersect, bagMinus, plus, minus, times, divide,
   	         eq, refeq, neq, lt, gt, le, ge, boolAnd, boolOr, comma, insert, insertInto, assign, semicolon }; 
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
		cerr << "jsi nonalgopnode markNeeded start " << opStr() << endl;
		this->setNeeded(true);
		cerr << "jsi rarg NULL " << (rarg == NULL) <<endl;
		cerr << "jsi wola this put to string \n";
		cout << "jsi wola this put to string \n";
		this->putToString();
		cerr << "jsi wola larg put to string \n";
		cout << "jsi wola larg put to string \n";
	//	this->larg->putToString();
		cerr << "jsi wola rarg put to string \n";
		cout << "jsi wola rarg put to string \n";
	//	this->rarg->putToString();
		cerr << "----------\n";
		cout << "----------\n";
		cerr << "jsi larg name \n"; // << this->getLArg()->getName() << "." << endl;
		cout << "jsi larg name \n"; //
	//	cout << this->getLArg()->getName() << "." << endl;
		cerr << "jsi wola rarggetname\n";
		cout << "jsi wola rarggetname\n";
//		cerr << "jsi rarg name " << this->getRArg()->getName() << "." << endl;
//		cout << "jsi rarg name " << this->getRArg()->getName() << "." << endl;
		cout << "!!!!!!!!!!!!!!!!!!!!!!!!!\n";
		cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!\n";
		this->getRArg()->markNeeded();	
		cerr << "jsi after right \n";
		cout << "jsi after right \n";
		if (op != 0){		// not a dot
			cerr << "jsi not a dot, calling on left\n";
			this->getLArg()->markNeeded();	
			cerr << "jsi after left\n";
		}
		cerr << "jsi nonalgopnode markNeeded end " << opStr()  << " \n";
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
	virtual string opStr() {
	    int op = this->getOp();
	    if (op == 0) return "_ . _";
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
	
	virtual ~CondNode() {if (larg != NULL) delete larg; if (rarg != NULL) delete rarg; }
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
          for( int i = 0; i < queries.size(); i++ ) {
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
	
	virtual ~FixPointNode() {}
    
    };
    
    class ReturnNode : public QueryNode
    {
    protected:
	QueryNode* query;
    public:
	ReturnNode(QueryNode *q) {this->query = q; }
	
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
          if( query )
            result += query->toString( level+1, true, "query" );
        }

        return result;
      }
    };
    
    
    
    
    // parametr formalny - nazwa + "in", "out" lub "none"
    class FormPar
    {
    protected:
	string name;
	string qualifier;
    public:
	FormPar (string n, string q) {
	    this->name = n; this->qualifier = q; }
	virtual string getName() {return name;}
	virtual string getQualifier() {return qualifier;}
	virtual ~FormPar() {}
    };    

    class ProcedureNode : public QueryNode
    {
    protected:
	string name;
	vector<string> params;
	vector<string> inouts;
	int parNumb; //liczba parametrow
	StringNode *content;  // tesc, czyli instrukcje w postaci stringa. (TODO: zmienic to?)
    public:
	ProcedureNode (FormPar *par) {		
	    params.push_back (par->getName());
	    inouts.push_back (par->getQualifier());
	    parNumb = 1;
	}
	ProcedureNode (FormPar *par, ProcedureNode *tail) {
	    params = tail->getParams();
	    inouts = tail->getInouts();
	    params.push_back (par->getName());
	    inouts.push_back (par->getQualifier());
	    parNumb = 1 + tail->getParNumb();
		//... should I delete tail here ? 
	}
	ProcedureNode (string n, StringNode* cont) {
	    name = n;
	    content = cont;
	    parNumb = 0; 
	}
      virtual string toString( int level = 0, bool recursive = false, string n = "" ) {
        stringstream c;
        string parNumbS;
        string paramsS = "[";
        string inoutsS = "[";

        for( unsigned i = 0; i < params.size(); i++ ) {
          paramsS += ((i > 0) ? ", " : "") + params[i];
        }
        paramsS += "]";

        for( unsigned i = 0; i < inouts.size(); i++ ) {
          inoutsS += ((i > 0) ? ", " : "") + inouts[i];
        }
        inoutsS += "]";

        c << parNumb; c >> parNumbS;
        string result = getPrefixForLevel( level, n ) + "[Procedure] name='" + name + "', parNumb="
          + parNumbS + ", params=" + paramsS + ", inouts=" + inoutsS +"\n";

        if( recursive ) {
          if( content )
            result += content->toString( level+1, true, "content" );
        }

        return result;
      }

	virtual TreeNode* clone();
	virtual int type() {return TreeNode::TNPROC;};
	virtual vector<string> getParams() {return this->params;}
	virtual string getParam(int i) {return this->params.at(i);}
	virtual vector<string> getInouts() {return this->inouts;}
	virtual string getInout(int i) {return this->inouts.at(i);}
	virtual int getParNumb() {return this->parNumb;}
	virtual string getName() {return this->name;}
	virtual StringNode *getContent() {return this->content;}
	virtual void setParNumb(int i) {parNumb = i;}
	virtual void setParams(vector<string> p) {params = p;}
	virtual void setInouts(vector<string> i) {inouts = i;}
	virtual ProcedureNode* addContent (string n, StringNode* cont) {
	    this->name = n;  this->content = cont;
	    return this;
	}
	virtual int putToString() {
	    cout << " Procedure " << name << " (";
	    for (int i = 0; i < parNumb; i++) {
		cout << "[" << inouts.at(i) << "] " << params.at(i) << "; ";
	    }
	    cout << ") { ";
		content->putToString();
	    cout << "}";
	    return 0;
	}

	virtual ~ProcedureNode() {}	
    };
    
    class CallProcNode : public QueryNode
    {
    protected:
	string name;
	vector<QueryNode*> params;
	int parNumb; //liczba parametrow
    public:
	CallProcNode (QueryNode *q) {		
	    params.push_back (q);
	    parNumb = 1;
	}
	CallProcNode (QueryNode *q, CallProcNode *tail) {
	    params = tail->getParams();
	    params.push_back (q);
	    parNumb = 1 + tail->getParNumb();
		//... should I delete tail here ? 
	}
	CallProcNode (string n) {name = n; parNumb = 0;}
	
      virtual string toString( int level = 0, bool recursive = false, string n = "" ) {
        string parNumbS;
        stringstream c;

        c << parNumb; c >> parNumbS;
        string result = getPrefixForLevel( level, n ) + "[CallProc] name='" + name + "', parNumb=" + parNumbS + "\n";

        if( recursive ) {
          for( int i = 0; i < params.size(); i++ ) {
            result += (params[i])->toString( level+1, true, "param" );
          }
        }

        return result;
      }

	virtual TreeNode* clone();
	virtual int type() {return TreeNode::TNCALLPROC;};
	virtual vector<QueryNode*> getParams() {return this->params;}
	virtual QueryNode *getParam(int i) {return this->params.at(i);}
	virtual int getParNumb() {return this->parNumb;}
	virtual string getName() {return this->name;}
	virtual void setParNumb(int i) {parNumb = i;}
	virtual void setParams(vector<QueryNode*> p) {params = p;}
	virtual CallProcNode* setName (string n) {
	    this->name = n; return this; }
	virtual int putToString() {
	    cout << " callProc " << name << " (";
	    for (int i = 0; i < parNumb; i++) {
		cout << "[";
		params.at(i)->putToString();
		cout  << "]; ";
	    }
	    cout << ") ";
	    return 0;
	}

	virtual ~CallProcNode() {}	
    };
    
    
    

}
    

#endif

