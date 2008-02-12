#ifndef OPTIMIZEDFIELD_H_
#define OPTIMIZEDFIELD_H_

#include "QueryParser/TreeNode.h"

namespace Indexes {

	class OptimizedField {
		public:
			OptimizedField(IndexHandler* ih);
			virtual ~OptimizedField();
			
			/** wynik mowi czy dane ograniczenie bedzie uwzgledniane w zwroconym indeksSelectNode. jesli nie to trzeba bedzie dokleic je po wywolaniu indeksu */
			bool addBound(QueryNode* value, AlgOpNode::algOp boundType);
			IndexSelectNode* getSelectNode();
			
			/** czy niektore ograniczenia ktore wczesniej zostaly przyjete potem musialy zostac odrzucone */
			bool someBoundsRejected();
			
			/** zwraca te ogranicznia ktore zostaly uzyte do skonstruowania IndexSelectNode'a i mozna je usunac z drzewa */
			vector<TreeNode*> getUsedBounds();
			
			/** mowi jak restrykcyjne bedzie wyszukiwanie po tym indeksie.
			 * im wieksza wartosc tym bardziej restrykcyjne, tym mniejszy powinien byc wynik, tym lepiej wybrac ta pole
			 * heurystyka */
			int howRestrictive();
		
		private:
			bool boundsRejected;
			vector<TreeNode*> usedBounds;
			IndexSelectNode *exact;
			IndexBoundaryNode *greaterThan, *smallerThan;
			IndexHandler* ih;
	};

}

#endif /*OPTIMIZEDFIELD_H_*/
