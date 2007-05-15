#ifndef AUXRMVER_H_
#define AUXRMVER_H_

#include <iostream>
#include <vector>
#include <string>
#include "ClassNames.h"
#include "TreeNode.h"
#include "QueryParser.h"

using namespace std;

namespace QParser{

class AuxRmver
{
protected:
	QueryParser *qParser;	
	TreeNode 	*qTree;
	// adds to boundVec all nodes from treeVec, that are bound in node
	void getBoundIn(TreeNode *node, vector<TreeNode*> *treeVec, vector<TreeNode*> *boundVec);
	// 
	void resetUsedNeeded(TreeNode *qtree);
	
	bool canTryToRemoveAux(NameAsNode * auxNode, vector<NameNode*>* nameVec);
	
	// auxNode - name as node, toRemoveVec - nazwy ktore sa wiazane w auxNode - pomocnicze
	bool removedAux(NameAsNode * auxNode, vector<NameNode*> *toRemoveVec, TreeNode *&qTree);
	
	TreeNode* getNodeByOid(vector<TreeNode*>* listVec, long oid);

public:
	AuxRmver(QueryParser *qParser);
	virtual ~AuxRmver();
	int rmvAux(TreeNode *&qTree);

};
}
#endif /*AUXRMVER_H_*/
