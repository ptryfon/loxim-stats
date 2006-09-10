#ifndef DEATHRMVER_H_
#define DEATHRMVER_H_

#include <iostream>
#include <vector>
#include <string>
#include "ClassNames.h"
#include "TreeNode.h"


using namespace std;

namespace QParser{

class DeathRmver
{
protected:
	QueryParser *&qParser;	
	TreeNode 	*&qTree;
	
	
public:
	DeathRmver(QueryParser *&qParser);
	virtual ~DeathRmver();
	int rmvDeath(TreeNode *&qTree);
};

protected:
	int doUsuniecia;
/*
	void fillDependencyGraph();
*/

}
#endif /*DEATHRMVER_H_*/
