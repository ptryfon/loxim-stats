#ifndef QUERYOPTIMIZER_H_
#define QUERYOPTIMIZER_H_

#include <QueryParser/TreeNode.h>
#include <Indexes/IndexManager.h>
#include <Indexes/OptimizedField.h>

namespace Indexes {

	class QueryOptimizer {
		public:
			/** wolane z parsera, wykonuje optymalizacje uzywajac indeksow */
			static void optimizeWithIndexes(TreeNode* &tree);
			static void setImplicitIndexCall(bool implicitIndexCall);
			QueryOptimizer();
			virtual ~QueryOptimizer();
						
		protected:
			static bool implicitIndexCall;
			IndexManager::indexesMap::iterator indexedFields;
			
			virtual void whereReplace(TreeNode* tree);
			void goodNode();
			void differentNode();
			void checkComparison(TreeNode* field, TreeNode* value, AlgOpNode::algOp boundType);
			bool optimizationPossible();
			void createResult(TreeNode* &tree);
			void findPossibleFields(TreeNode* node);
			OptimizedField* chooseBestField();
			AlgOpNode::algOp opposite(AlgOpNode::algOp op);
			
			/** wycina z drzewa wartosc value i czesc drzewa z nia zwiazana.
			 * wywolywane tylko gdy jest przynajmniej jeden and */
			void cutOff(TreeNode* value);
			
			/** czy wartosc do ktorej porownujemy pole jest literalem */
			bool eligibleValue(TreeNode* value);
			//enum optimizationStatus {IMPOSSIBLE, FULL_REPLACE, PARTIAL_REPLACE}
		
			typedef map<string, OptimizedField*> optimizedFields_t;  
			optimizedFields_t optimizedFields;
			bool firstUpdate;
			bool correctNode;
			int differentNodeCount;
			OptimizedField* bestField;
			string indexName;
			
			friend class Tester;
	};

}

#endif /*QUERYOPTIMIZER_H_*/
