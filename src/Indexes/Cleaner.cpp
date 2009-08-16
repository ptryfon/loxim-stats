#include <cassert>
#include <Store/NamedItems.h>
#include <Indexes/Cleaner.h>
#include <Indexes/Node.h>
#include <Indexes/VisibilityResolver.h>
#include <Indexes/IndexManager.h>

using namespace std;

namespace Indexes {

	auto_ptr<Cleaner> Cleaner::instance(new Cleaner());

	Cleaner::Cleaner() {
	}
	
	Cleaner::~Cleaner() {
	}
	
	Cleaner* Cleaner::getHandle() {
		return instance.get();
	}
	
	bool Cleaner::putEqualLid(RootEntry* inTree, RootEntry* toPut) {
		if (inTree->modyfierTransactionId == toPut->modyfierTransactionId) {
			assert (
					!((inTree->addTime > 0) && (toPut->addTime > 0)) 
					&&
					!((inTree->delTime > 0) && (toPut->delTime > 0))
					);
			return true;
		}
		//sa w roznych transakcjach oba wpisy musza byc widoczne
		return false;
	}

	void Cleaner::clean(CachedNode* cnode, Comparator* comp, IndexHandler* ih) {
		lid_t index = ih->getId();
		Node* node = cnode->getNode();
		if (node->leaf) {
			lid_t lid = 0;
			bool started = false, add = false, del = false;
		
			RootEntry* firstToRemove = NULL;
			RootEntry* entry = NULL;
		   	entry = comp->getItem(0, node, entry);
			
			VisibilityResolver* v = IndexManager::getHandle()->resolver.get();
			
			while (entry < (RootEntry*) node->afterAddr()) {
				
				if (!started) {
					started = true;
					lid = entry->logicalID;
					firstToRemove = entry;
				}
				
				if (v->isRolledBack(entry->modyfierTransactionId, ih->getId())) {
					if (entry->modyfierTransactionId != ROLLED_BACK) {
						v->entryDropped(entry->modyfierTransactionId, index, ROLLED_BACK_ENTRY);
					}
					comp->removeEntry(node, entry);
					cnode->setDirty();
					continue; //nie bierzemy kolejnej entry bo usuwamy aktualna wiec reszta sie przesuwa
					
				} else
					if (v->isBeyondHorizon(entry->modyfierTransactionId)) {
						if (entry->modyfierTransactionId != BEYOND_HORIZON) {
							entry->modyfierTransactionId = BEYOND_HORIZON;
							cnode->setDirty();
						}
						
						//sprawdzenie czy tr dodajaca i usuwajaca wyszly poza horyzont
						if (entry->logicalID != lid) {
							add = del = false;
							lid = entry->logicalID;
							firstToRemove = entry;
						}
						if (entry->addTime != STORE_IXR_NULLVALUE) {
							add = true;
						} else if (entry->delTime != STORE_IXR_NULLVALUE) {
							del = true;
						}
						if (add && del) {
							comp->removeEntry(node, firstToRemove);
							comp->removeEntry(node, firstToRemove); //po usunieciu druga znajdzie sie w miejscu pierwszej 
							/* o usunieciu nie trzeba informowac visibilityResolvera bo te wpisy zostaly zatwierdzone
							 * i wyszly poza horyzont. resolver nie przechowuje juz zadnych danych dla tych transakcji */ 
							cnode->setDirty();
						}		
				}
				//mozna zrobic dodatkowe pole i tam trzymac czy jest ze jest commited, zeby nie powtarzac wyszukiwania po rolledBack
				entry = comp->getNextItem(entry);
			}
			
			//node->cleanNeeded = false;
			//delete comp;
		}
	}
}
