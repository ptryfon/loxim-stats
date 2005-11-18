#ifndef _TREE_NODE_H_
#define _TREE_NODE_H_

#include <vector>
#include <string>
//#include "../Store/Store.h"
using namespace std;

namespace QParser {

// statement (the starting symbol)
class TreeNode {
 protected:
  TreeNode* parent;
 public:
  enum TreeNodeType { TNINT, TNSTRING, TNDOUBLE, TNVECTOR, TNNAME, 
TNAS, TNUNOP, TNALGOP, TNNONALGOP, TNTRANS, TNCREATE};
  TreeNode() : parent(NULL) {}
  TreeNode* getParent() { return parent; }
  void setParent(TreeNode* _parent) { parent = _parent; }
  virtual TreeNode* clone()=0;
  virtual int type()=0;
  virtual ~TreeNode() {}
  
  // cos takiego chce Executor::
  virtual string getName() {return (string) NULL;}
    virtual TreeNode* getArg() {return (TreeNode *) NULL;}
  // virtual QueryNode* getArg() {return (QueryNode *)NULL;}; // to nie dziala
//  virtual void *getValue() {return (void *)NULL;}

};
// statement := query
class QueryNode : public TreeNode {
 public:
  virtual TreeNode* clone()=0;
  virtual int type()=0;
};

// query := name
class NameNode : public QueryNode {
 protected:
  string name;
 public:
  NameNode(string _name) : name(_name) {}
  virtual TreeNode* clone();
  virtual int type() { return TreeNode::TNNAME; }
  string getName() { return name; }
  virtual ~NameNode() {}
};  

// query := integer
class IntNode : public QueryNode {  
 protected:
  int value;
 public:
  IntNode(int _value) : value(_value) {}
  virtual TreeNode* clone();
  virtual int type() { return TreeNode::TNINT; }
  int getValue() {return value;}

};  

// query := string
class StringNode : public QueryNode {
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
class DoubleNode : public QueryNode {
 protected:
  double value;
 public:
  DoubleNode(double _value) : value(_value) {}
  virtual TreeNode* clone();
//virtual string type() { return "double"; }
  virtual int type() { return TreeNode::TNDOUBLE; }
  double getValue() { return value; }
        
};  

// query := query "as" name
// query := query "group as" name
class NameAsNode : public QueryNode {
 protected:
  QueryNode* arg;
  string name;
  bool group;
 public:
  NameAsNode(QueryNode* _arg, string _name, bool _group)
            : arg(_arg), name(_name), group(_group) { arg->setParent(this); }
  virtual TreeNode* clone();
//virtual string type() { return "as"; }
  virtual int type() { return TreeNode::TNAS; }
  string getName() { return name; }
  //QueryNode* getArg() { return arg; }
  TreeNode* getArg() { return arg; }
  virtual void setArg(QueryNode* _arg) { arg = _arg; arg->setParent(this); }    
  bool isGrouped() { return group; }
  virtual ~NameAsNode() { if (arg != NULL) delete arg; }
};  


// query := query InfixAlgOp query
class UnOpNode : public QueryNode {
 public:
  enum unOp { unMinus, count, sum, avg, min, max, distinct, boolNot, deleteOp, deref };
 protected:
  QueryNode* arg;
  unOp op;
 public:
  UnOpNode(QueryNode* _arg, unOp _op) : arg(_arg), op(_op)
                                        { arg->setParent(this); }
  virtual TreeNode* clone();
//  virtual string type() { return "unop"; }
  virtual int type() { return TreeNode::TNUNOP; }
  //QueryNode* getArg() { return arg; }
  TreeNode* getArg() { return arg; }
  unOp getOp() { return op; }
  virtual void setArg(QueryNode* _arg) { arg = _arg; arg->setParent(this); }
  virtual void setOp(unOp _op) { op = _op; }
  virtual ~UnOpNode() { if (arg != NULL) delete arg; }
};  

// query := query InfixAlgOp query
class AlgOpNode : public QueryNode {
 public:
  enum algOp { bagUnion, bagIntersect, bagMinus, plus, minus, times, divide,
   	         eq, neq, lt, gt, le, ge, boolAnd, boolOr, comma, insert }; 
 protected:
  QueryNode* larg;
  QueryNode* rarg;
  algOp op;
 public:
  AlgOpNode(QueryNode* _larg, QueryNode* _rarg, algOp _op)
		: larg(_larg), rarg(_rarg), op(_op) 
                { larg->setParent(this); rarg->setParent(this); }
  virtual TreeNode* clone();
//  virtual string type() { return "algop"; }
  virtual int type() { return TreeNode::TNALGOP; }
  QueryNode* getLArg() { return larg; }
  QueryNode* getRArg() { return rarg; }
  algOp getOp() { return op; }
  virtual void setLArg(QueryNode* _larg) {larg = _larg;larg->setParent(this);}
  virtual void setRArg(QueryNode* _rarg) {rarg = _rarg;rarg->setParent(this);}
  virtual void setOp(algOp _op) { op = _op; }
  virtual ~AlgOpNode() { if (larg != NULL) delete larg;
                         if (rarg != NULL) delete rarg; }
};  

// query := query InfixNonalgOp query
class NonAlgOpNode : public QueryNode {
 public:
  enum nonAlgOp { dot, join, where, closeBy, closeUniqueBy, leavesBy,
                  leavesUniqueBy, orderBy, exists, forAll };
 protected:
  QueryNode* larg;
  QueryNode* rarg;
  nonAlgOp op;
 public:
  NonAlgOpNode(QueryNode* _larg, QueryNode* _rarg, nonAlgOp _op)
                : larg(_larg), rarg(_rarg), op(_op)
                { larg->setParent(this); rarg->setParent(this); } 
  virtual TreeNode* clone();
//  virtual string type() { return "nonalgop"; }
  virtual int type() { return TreeNode::TNNONALGOP; }
  QueryNode* getLArg() { return larg; }
  QueryNode* getRArg() { return rarg; }
  nonAlgOp getOp() { return op; }
  virtual void setLArg(QueryNode* _larg) {larg = _larg;larg->setParent(this);}
  virtual void setRArg(QueryNode* _rarg) {rarg = _rarg;rarg->setParent(this);}
  virtual void setOp(nonAlgOp _op) { op = _op; }
  virtual ~NonAlgOpNode() { if (larg != NULL) delete larg;
                            if (rarg != NULL) delete rarg; }
};  

class TransactNode : public TreeNode {
 public:
  enum transactionOp { begin, end, abort }; 
 protected:
  transactionOp op;
 public:
  TransactNode (transactionOp _op) : op(_op) {}
  virtual TreeNode* clone();
//  virtual string type() { return "transaction"; }
  virtual int type() { return TreeNode::TNTRANS; }
  transactionOp getOp() { return op; }
  virtual void setOp(transactionOp _op) { op = _op; }
  virtual ~TransactNode() {}
};  

// query := CREATE NAME LEFTPAR query RIGHTPAR
// query := CREATE NAME 
class CreateNode : public QueryNode {
 protected:
  QueryNode* arg;
  string name;
 public:
  CreateNode(string _name, QueryNode* _arg = NULL)
            : arg(_arg), name(_name)  {}
  virtual TreeNode* clone();
//  virtual string type() { return "create"; }
  virtual int type() { return TreeNode::TNCREATE; }
  string getName() { return name; }
//  QueryNode* getArg() { return arg; }
    TreeNode* getArg() { return arg; }
  virtual void setArg(QueryNode* _arg) { arg = _arg; arg->setParent(this); }    
  virtual ~CreateNode() { if (arg != NULL) delete arg; }
};  

}

#endif

