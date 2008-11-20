#ifndef DEATHRMVER_H_
#define DEATHRMVER_H_

#include <iostream>
#include <vector>
#include <string>
#include <QueryParser/ClassNames.h>
#include <QueryParser/TreeNode.h>
#include <QueryParser/QueryParser.h>


using namespace std;

namespace QParser{

class DeathRmver
{
protected:
	QueryParser *qParser;	
	TreeNode 	*qTree;
	
	
public:
	DeathRmver(QueryParser *qParser);
	virtual ~DeathRmver();
	int rmvDeath(TreeNode *&qTree);
};
}
#endif /*DEATHRMVER_H_*/
