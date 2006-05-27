#ifndef _TREE_NODE_H_
#define _TREE_NODE_H_

#include <iostream>

#include <vector>
#include <string>
#include "Stack.h"
#include "ClassNames.h"
//using namespace std;

namespace QParser {

// statement (the starting symbol)
    class TreeNode 
    {
    protected:
	TreeNode* parent;


    public:
	enum TreeNodeType { TNINT, TNSTRING, TNDOUBLE, TNVECTOR, TNNAME, 
	    TNAS, TNUNOP, TNALGOP, TNNONALGOP, TNTRANS, TNCREATE, TNCOND, TNLINK, TNPARAM, TNFIX};
	TreeNode() : parent(NULL) {}
	TreeNode* getParent() { return parent; }
	void setParent(TreeNode* _parent) { parent = _parent; }
	virtual TreeNode* clone()=0;
	virtual int type()=0;
	virtual ~TreeNode() {}
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
    };

// statement := query
    class QueryNode : public TreeNode 
    {
    public:
	virtual TreeNode* clone()=0;
	virtual int type()=0;
	virtual int putToString()=0;

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

	virtual int swapSon (TreeNode *oldSon, TreeNode *newSon) {
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
    };    

// query := name
    class NameNode : public QueryNode 
    {
    protected:
	string name;
	int bindSect;
	int stackSize;
    public:
	NameNode(string _name) : name(_name) {bindSect = 0; stackSize = 0;}

	virtual int getBindSect() { return this->bindSect;}
	virtual int getStackSize() {return this->stackSize;}
	virtual void setBindSect(int newBSect) { this->bindSect = newBSect;}
	virtual void setStackSize(int newSize) { this->stackSize = newSize;}
	
	virtual TreeNode* clone();
	virtual int type() { return TreeNode::TNNAME; }
	virtual string getName() { return name; }
	virtual int putToString() {
	    cout << "("<< this->getName() <<"["<<bindSect << "," << stackSize << "])";    
	    return 0;
	}
	virtual int staticEval (StatQResStack *&qres, StatEnvStack *&envs);	
	
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
	    
	    cout << this->getName() <<")";    
	    return 0;
	}
	virtual int swapSon (TreeNode *oldSon, TreeNode *newSon) {
	    if (arg == oldSon) {this->setArg((QueryNode *) newSon); return 0;}
	    else {/*an error from errorConsole is called;*/ return -1;}
	}    	

	virtual int staticEval(StatQResStack *&qres, StatEnvStack *&envs);
	virtual int optimizeTree() {return arg->optimizeTree();}
	virtual ~NameAsNode() { if (arg != NULL) delete arg; }
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

	virtual int putToString() {
	    cout << "(";
	    if (larg!= NULL) larg->putToString();
	    else cout << "___";
	    cout << this->opStr();
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
	
	virtual int getFirstOpenSect() {return this->firstOpenSect;}
	virtual void setFirstOpenSect(int newSectNr) {this->firstOpenSect = newSectNr;}
	virtual int getLastOpenSect() {return this->lastOpenSect;}
	virtual void setLastOpenSect(int newNr) {this->lastOpenSect = newNr;}
	
//	virtual Signature *combine(Signature *lRes, Signature *rRes);
	
	virtual int putToString() {
	    cout << "(";
	    if (larg!= NULL) larg->putToString();
	    else cout << "___";
	    cout << this->opStr();
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
		virtual int swapSon (TreeNode *oldSon, TreeNode *newSon) {
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
	
	virtual int putToString() {
	    cout << "( link " << this->getNodeName() << " - " << this->getIp()<< " - " << this->getPort() << " )";
	    return 0;	
	}
	virtual ~LinkNode() {}
	
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



}
    

#endif

