#include "TreeNode.h"

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

}
