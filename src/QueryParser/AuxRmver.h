#ifndef AUXRMVER_H_
#define AUXRMVER_H_

#include <iostream>
#include <vector>
#include <string>
#include <QueryParser/ClassNames.h>
#include <QueryParser/TreeNode.h>

namespace QParser{
	class QueryParser;
	class AuxRmver
	{
		protected:
			QueryParser *qParser;	
			TreeNode 	*qTree;


			bool canTryToRemoveAux(NameAsNode * auxNode, std::vector<NameNode*>* nameVec);

			// auxNode - name as node, toRemoveVec - names that are bound in auxNode , helpers
			bool removedAux(NameAsNode * auxNode, std::vector<NameNode*> *toRemoveVec, TreeNode *&qTree);



		public:
			AuxRmver(QueryParser *qParser);
			virtual ~AuxRmver();
			int rmvAux(TreeNode *&qTree);

	};
}
#endif /*AUXRMVER_H_*/
