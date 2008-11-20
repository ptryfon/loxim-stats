#ifndef LEAFTRAVELLER_H_
#define LEAFTRAVELLER_H_

#include <Indexes/Comparator.h>
#include <Indexes/EntrySieve.h>

namespace Indexes {

	class LeafTraveller {
		protected:
			
			/** nie zwalnia ostatnio wczytanego wezla, chyba ze dotarl do konca
			 *  zaklada ze cleaf jest juz zarezerwowany i wczytany do pamieci */
			int travelTo(Comparator* comp, bool (Comparator::*condition)(RootEntry*));
			/** aktualna pozycja travelera */
			CachedNode* cleaf;
			/** aktualna pozycja travelera */
			RootEntry* entry;
			Comparator* destinationComp;
			bool grab, destinationExceeded;
			BTree* tree;
			EntrySieve *sieve;
			bool endReached;
			
		public:
			LeafTraveller(BTree* tree, EntrySieve* sieve);
			virtual ~LeafTraveller();
			
			LeafTraveller* setPosition(CachedNode* leaf, RootEntry* entry);
			LeafTraveller* setGrab(bool grab);
			LeafTraveller* setDestination(Comparator* destinationComp);
			bool isDestinationExceeded();
			int travelToEnd();
			int travelToGreater();
			int travelToDestination();
			
			
			
	};

}

#endif /*LEAFTRAVELLER_H_*/
