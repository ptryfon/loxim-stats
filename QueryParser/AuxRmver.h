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
	
	
	bool canTryToRemoveAux(NameAsNode * auxNode, vector<NameNode*>* nameVec);
	
	// auxNode - name as node, toRemoveVec - nazwy ktore sa wiazane w auxNode - pomocnicze
	bool removedAux(NameAsNode * auxNode, vector<NameNode*> *toRemoveVec, TreeNode *&qTree);
	
	

public:
	AuxRmver(QueryParser *qParser);
	virtual ~AuxRmver();
	int rmvAux(TreeNode *&qTree);

};
}
#endif /*AUXRMVER_H_*/
