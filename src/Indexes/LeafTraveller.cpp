#include <Indexes/LeafTraveller.h>

#include <Indexes/IndexHandler.h>
#include <assert.h>

namespace Indexes {

	LeafTraveller::LeafTraveller(BTree* tree, EntrySieve* sieve) : tree(tree), sieve(sieve) {
		cleaf = NULL;
		entry = NULL;
		destinationComp = NULL;
		grab = false;
		endReached = false;
	}
	
	LeafTraveller::~LeafTraveller() {
		if (cleaf) {
			int err = tree->ih->freeNode(cleaf);
			err = err; //a dirty hack to supress "unused variable" warning in release mode
			assert(!err);
		}
	}
	
	LeafTraveller* LeafTraveller::setGrab(bool grab) {
		this->grab = grab;
		return this;
	}
	
	//zaklada ze leaf jest wczytany i zablokowany
	LeafTraveller* LeafTraveller::setPosition(CachedNode* leaf, RootEntry* entry) {
		this->cleaf = leaf;
		this->entry = entry;
		
		return this;
	}
	
	LeafTraveller* LeafTraveller::setDestination(Comparator* destinationComp) {
		this->destinationComp = destinationComp;
		return this;
	}
	
	bool LeafTraveller::isDestinationExceeded() {
		return destinationExceeded;
	}
	
	
	/** przesuwa sie az do wartosci wiekszej od zawartej w comparatorze */
	int LeafTraveller::travelTo(Comparator* comp, bool (Comparator::*condition)(RootEntry*)) {
		int err;
		
		if (endReached) {
			//cale drzewo zostalo przejrzane juz w poprzednim przebiegu
			return 0;
		}
		
		Node* leaf = cleaf->getNode();
		assert(endReached || (leaf && entry));
		
		
		if (entry == NULL) {
			//w tym nodzie juz nic nie ma, trzeba zaczac od nastepnego. trzba ustawic sie na koncu aktualnego liscia
			entry = (RootEntry*) leaf->afterAddr();
		}
			
		RootEntry* after;
			
		while (true) { //przejscie po lisciu
			
			after = (RootEntry*) leaf->afterAddr();
			
			//cout << endl << comp->nodeToString(leaf) << endl;
			
			while (entry < after) {
				if ((comp->*condition)(entry)) {
					// warunek zatrzymania zostal spelniony
				//	err = tree->ih->freeNode(cleaf);
					return 0;  					
				} else {
					// nadal sie przesuwamy
					if (grab) {
						sieve->add(entry); //nie klonujemy bo wskaznik nie bedzie przechowywany
					}
					entry = comp->getNextItem(entry);
				}
			}
			
			
			if (leaf->nextLeaf == NEW_NODE) {
				//ostatni lisc
				err = tree->ih->freeNode(cleaf);
				leaf = NULL;
				cleaf = NULL;
				endReached = true;
				return err;
			} else {
				CachedNode* cleaf2;
				if ((err = tree->ih->getNode(cleaf2, leaf->nextLeaf))) {
					return err;
				}
				
				if ((err = cleaf2->getNode(leaf, cleaf->siblingPriority(), NODE_READ))) {
					return err;
				}
				
				if ((err = tree->ih->freeNode(cleaf))) {
					return err;
				}
				cleaf = cleaf2;
				
			}
			
			entry = comp->getItem(0, leaf, entry);
				
		}
		
		
	}
	
	// przejscie do konca
	int LeafTraveller::travelToEnd() {
		Comparator* comp = tree->getNewComparator();
		
		int err;
		err = travelTo(comp, &Comparator::infinityReached);
		delete comp;
		return err;	
	}
	
	// przejscie do nastepnego wpisu roznego od obecnego
	int LeafTraveller::travelToGreater() {
		int err;
		Comparator* comp;
		if (entry == NULL) {
			comp = new MinComparator();
		} else {
			//comp = comparator i setValue(entry -> klucz);
			comp = tree->getNewComparator();
			comp->getValue(entry);
		}
		//trzeba sie przesuwac az zawartosc comparatora bedzie mniejsza od tego co w drzewie
		err = travelTo(comp, &Comparator::lessThan);//nie bierze pod uwage lid
		delete comp;
		return err;	
	}
	
	//przejscie do drugiej wartosci
	int LeafTraveller::travelToDestination() {
		int err;
		
		assert(destinationComp);
		
		err = travelTo(destinationComp, &Comparator::lessOrEqual);//nie bierze pod uwage lid
		
		if (!err && cleaf) {
			//cel przekroczono jesli comparator jest mniejszy niz zawartosc drzewa
			destinationExceeded = destinationComp->lessThan(entry); //nie bierze pod uwage lid
		}
		
		//delete destinationComp;
		return err;
	}
}
