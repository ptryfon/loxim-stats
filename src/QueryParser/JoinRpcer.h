#ifndef JOINRPCER_H_
#define JOINRPCER_H_

#include <iostream>
#include <vector>
#include <string>
#include <QueryParser/ClassNames.h>
#include <QueryParser/TreeNode.h>

using namespace std;

namespace QParser{
class QueryParser;
class JoinRpcer
{
protected:
	QueryParser *qParser;	
	TreeNode 	*qTree;
	
	virtual void getSupposed(vector<NonAlgOpNode*> *toReplaceVec, TreeNode *&qTree);
	virtual bool canReplace(NonAlgOpNode* dotNode);
	virtual void replaceJoinWithDot(NonAlgOpNode* joinNode);
	
public:
	JoinRpcer(QueryParser *qParser);
	virtual ~JoinRpcer();
	virtual int replaceJoin(TreeNode *&qTree);
};
}
#endif /*JOINRPCER_H_*/
