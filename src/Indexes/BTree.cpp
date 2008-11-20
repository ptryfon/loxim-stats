#include <algorithm>
#include <assert.h>
#include <Indexes/BTree.h>
#include <Indexes/IndexHandler.h>
#include <Indexes/Node.h>
#include <Indexes/Comparator.h>
#include <Indexes/Cleaner.h>
#include <Indexes/VisibilityResolver.h>
#include <Indexes/IndexManager.h>
//#include <TemplateComparator.h>

namespace Indexes
{
	bool BTREE_DEBUG_MODE = false;

	/**
	 * wskaznik przed kluczem A wskazuje na wartosci mniejsze, rowne od klucza A
	 */
	class IndexHandler;

	int BTree::createRoot() {
		int err;
		CachedNode *cnode;
		Node *node;
		if ((err = ih->getNode(cnode, NEW_NODE, true))) {
			return err;
		}
		rootLevel = ROOT_PRIORITY;
		if ((err = cnode->getNode(node, rootLevel, NODE_WRITE))) {
			return err;
		}
		node->leaf = true;
		rootAddr = node->id;
		node->nextLeaf = NEW_NODE;
		
		if (BTREE_DEBUG_MODE) {
			cout << endl << defaultComparator->nodeToString(node) << endl;
		}
		
		ih->freeNode(cnode);
		return 0;
	}

	template <class I>
	int BTree::splitChild(CachedNode* cparent, CachedNode* cchild, char* toChild, I* type) {
		
		Node *parent, *child;
		
		cparent->getNode(parent);
		cchild->getNode(child);
		
		int err;
		I *entry = defaultComparator->getItem(0, child, entry);
		I *half = (I*) (((char*)child) + (MAX_NODE_CAPACITY / 2));
		I *prevEntry = NULL;
		
		if (BTREE_DEBUG_MODE) {
			cout << endl << "parent: " << defaultComparator->nodeToString(parent) << endl;
			cout << endl << "child: " << defaultComparator->nodeToString(child) << endl;
		}
		
		//pierwszy wpis musi zaczynac sie przed polowa wolnego miejsca. (zeby petla choc raz sie wykonala)
		while (entry < half) {
			prevEntry = entry;
			entry = defaultComparator->getNextItem(entry);
		}
		
		/* teraz prevEntry przechodzi przez polowe albo konczy sie w polowie
		   entry to pierwszy wpis ktory musi zostac przeniesiony
		   prevEntry musi byc skopiowany do ojca
		 */
		
		//skopiowac polowe wpisow do nowego wezla
		CachedNode* cnewChild;
		Node* newChild;
		
		if ((err = ih->getNode(cnewChild, NEW_NODE))) {
			return err;
		}
		if ((err = cnewChild->getNode(newChild, cchild->siblingPriority(), NODE_WRITE))) {
			return err;
		}
		
		newChild->leaf = child->leaf;
		char* dest = (char*) defaultComparator->getItem(0, newChild, entry);
		nodeSize_t size = child->distanceInNode(entry, child->afterAddr()); 
		
		memcpy(dest, entry, size);
		newChild->left -= size;
		child->left += size; 
		//wszystkie dane oprocz skrajnie lewego wskaznika sa juz przeniesione do nowego wezla
		
		//zrob miejsce w parent		
		nodeSize_t keySize = child->distanceInNode(prevEntry, defaultComparator->getNextItem(prevEntry)) - sizeof(I);
		nodeSize_t sizeInParent = keySize + sizeof(NodeEntry);
		
		//size = child->distanceInNode(prevEntry, defaultComparator->getNextItem(prevEntry)); //rozmiar prevEntry razem z kluczem
		dest = toChild + sizeInParent;
		//juz wiemy o ile. teraz trzeba sprawdzic jak duzo danych trzeba przesunac
		nodeSize_t sizeToMove = parent->distanceInNode(toChild, parent->afterAddr());
		
		memmove(dest, toChild, sizeToMove);
		
		//w parent jest juz miejsce na prevEntry
		// skopiuj prevEntry do ojca
		Comparator *c = defaultComparator->emptyClone();
		c->getValue(prevEntry);
		c->putKey((NodeEntry*)toChild);
		//memcpy(toChild + sizeof(NodeEntry), ((char*)prevEntry) + sizeof(I), keySize);
		((NodeEntry*) toChild)->child = newChild->id;
		((NodeEntry*) toChild)->logicalID = prevEntry->logicalID; 
		
		parent->left -= sizeInParent; //bo dodalismy wpis o tej wielkosci
		//jesli jest wewnetrzny to mozna go skrocic o size;
		
		if (child->leaf) {
			newChild->nextLeaf = child->nextLeaf;
			child->nextLeaf = newChild->id;
		} else {
			// bez rzutowania typ generyczny sie nie kompilowal. a jesli child jest wewnetrzny to prevEntry na pewno bedzie typu NodeEntry*
			newChild->firstChild = ((NodeEntry*) prevEntry)->child;
			//z wezla wewnetrznego mozemy usunac ostatni (przeniesiony w gore) klucz
			child->left += (keySize + sizeof(I)); //mogloby byc sizeof(NodeEntry) bo tylko wtedy usuwanie 
		}
		
		if (BTREE_DEBUG_MODE) {
			cout << endl << "parent: " << defaultComparator->nodeToString(parent) << endl;
			cout << endl << "child: " << defaultComparator->nodeToString(child) << endl;
			cout << endl << "newChild: " << defaultComparator->nodeToString(newChild) << endl;
		}
		
		cparent->setDirty();
		cchild->setDirty();
		
		//czy to na pewno dobry pomysl? moze warto zwalniac poza ta metoda gdy juz wiemy do ktorego dziecka bedziemy wchodzic
		ih->freeNode(cnewChild);
		ih->freeNode(cchild);
		return 0;
	}

	int BTree::splitChild(CachedNode* parent, CachedNode* child, char* toChild) {
		Node* nchild;
		child->getNode(nchild);
		if (nchild->leaf) {
			RootEntry* e;
			return splitChild(parent, child, toChild, e);
		} else {
			NodeEntry* e;
			return splitChild(parent, child, toChild, e);
		}
	}

	int BTree::removeRoot(CachedNode* croot) {
		rootAddr = croot->getNode()->firstChild;
		
		int err = 0;
		if ((err = ih->dropNode(croot))) {
			return err;
		}
		
		ih->rootChange(rootLevel, croot->childPriority());
		return err;
	}
	
	NodeCapacityType BTree::canTakeExtraKey(CachedNode* cnode) {
		Node* node = cnode->getNode();
		if (defaultComparator->canTakeExtraKey(node)) {
			return checkForDelete(node);
		}
		//jesli sie nie miesci probujemy przeprowadzic czyszczenie
		Cleaner::getHandle()->clean(cnode, defaultComparator, ih);
		if (defaultComparator->canTakeExtraKey(node)) {
			return checkForDelete(node);
		}
		return SPLIT_NEEDED;
	}
	
	NodeCapacityType BTree::checkForDelete(Node* node) {
		if (node->size() < defaultComparator->MIN_NODE_SIZE_ALLOWED) {
			return DELETE_NEEDED;
		} else {
			return PROPER_SIZE;
		}
	}
	
	int BTree::rootPriority() {
		return rootLevel;
	}
	
	void BTree::setRootPriority(int level) {
		rootLevel = level;
	}
	
	void  BTree::rootChange(CachedNode* newRoot, CachedNode* oldRoot) {
		rootAddr = newRoot->getNodeID();
		ih->rootChange(rootLevel, newRoot->depth);
		//rootLevel = newRoot->depth;
	}
	
	bool BTree::shouldRemoveRoot(Node* root) {
		if (root->leaf) {
			//nie usuwamy ostatniego wezla
			return false;
		}
		char* begin = root->afterHeader();
		char* after = root->afterAddr();
		//usuwamy tylko jesli w root'cie jest jeden wskaznik - fistChild
		return (after == begin);
	}
	
	int BTree::addEntry(Comparator *filledComparator) {
		int err;
		CachedNode* croot;
		Node* root;
		rootSem->lock_write();
		
		repeatAdd:
		
		if ((err = ih->getNode(croot, rootAddr, true))) {
			return err;
		}
		if ((err = croot->getNode(root, rootLevel, NODE_WRITE))) {
			return err;
		}
		
		if (BTREE_DEBUG_MODE) {
			cout << endl << "root: " << defaultComparator->nodeToString(root) << endl;
		}
		
		switch (canTakeExtraKey(croot)) {
			case DELETE_NEEDED:
				if (shouldRemoveRoot(root)) {
					if ((err = removeRoot(croot))) {
						return err;
					}
					goto repeatAdd;
				}
			default:
				rootSem->unlock();
				return addEntry(croot, filledComparator);
			case SPLIT_NEEDED:
				// za malo miejsca w korzeniu
				//Node* oldRoot = root;
				Node* node;
				CachedNode* cnode;
				if ((err = ih->getNode(cnode, NEW_NODE))) {
					return err;
				}
				if ((err = cnode->getNode(node, croot->parentPriority(), NODE_WRITE))) {
					return err;
				}
				
				rootChange(cnode, croot);
				rootSem->unlock();
				
				node->leaf = false;
				//filledComparator->putPtr(0, root, oldRoot);
				node->firstChild = croot->getNode()->id;
				
				char* afterPtrToChild = ((char*) node) + sizeof(Node);
				if ((err = splitChild(cnode, croot, afterPtrToChild))) {
					return err;
				}
				return addEntry(cnode, filledComparator);
		}
	}

	int BTree::addEntry(CachedNode* cnode, Comparator *comp) {
		int err, comparison;
		bool putNewEntry = true;
		while (true) { // iteracja zamiast rekurencji
			
			Node* node;
			if ((err = cnode->getNode(node))) {
				return err;
			}
			assert(defaultComparator->canTakeExtraKey(node) != SPLIT_NEEDED); //niezmiennik petli
			
			if (BTREE_DEBUG_MODE) {
				cout << endl << "wstawiam do node: "<< defaultComparator->nodeToString(node) << endl;
			}
			
			Cleaner::getHandle()->clean(cnode, defaultComparator, ih);
			if (BTREE_DEBUG_MODE) {
				cout << endl << "po oczyszczeniu wstawiam do node: "<< defaultComparator->nodeToString(node) << endl;
			}
			
			
			if (node->leaf) {
				RootEntry* after = (RootEntry*) node->afterAddr();  
				RootEntry* entry = comp->getItem(0, node, entry);
				
				while (entry < after) {
					//Cleaner::getHandle()->checkChangeNeed(node, entry);
					
					comparison = comp->compare(entry); 
					if (comparison > 0) {
						// zawartosc comparatora jest wieksza niz klucza w wezle. trzeba sprawdzic kolejny klucz
						entry = comp->getNextItem(entry);				
					} else {						
						if (comparison == 0){
							putNewEntry = !Cleaner::getHandle()->putEqualLid(entry, comp->getEntry());
							if (!putNewEntry) {
								// nie wstawiamy nowego i usuwamy stary wpis
								IndexManager::getHandle()->getResolver()->entryDropped(entry->modyfierTransactionId, ih->getId(), ACTIVE_ENTRY);
								comp->removeEntry(node, entry);
								cnode->setDirty();
								//checkForDelete(node);
								//nie trzeba poprawiac after bo jesli cos usuniemy to nie zaczynamy kolejnego obrotu petli
								if (BTREE_DEBUG_MODE) {
									cout << endl << "wstawiam do node: "<< defaultComparator->nodeToString(node) << endl;
								}
								break;
							}
							// zawartosc comparatora jest rowna aktualnie ogladanej. 
						}
						//zawartosc comparatora jest mniejsza niz klucza w wezle. trzeba przesunac reszte pamieci
						long space = comp->getSpaceNeeded(entry);
						void* dest = ((char*) entry) + space;
						long bytes2copy = ((char*)after) - ((char*) entry);
						memmove(dest, entry, bytes2copy);
						break;
					}
				}
				// klucz w comparatorze jest najwiekszy w tym wezle, albo juz przesunelismy pamiec i mamy miejsce. tu wstawiamy, poczynajac od entry
				//czy tutaj kiedykolwiek wstawiamy cos na koniec wezla (chyba ze root, wtedy ok)?
				if (putNewEntry) {
					comp->putValue(entry);
					cnode->setDirty();
					IndexManager::getHandle()->getResolver()->entryAdded(entry->modyfierTransactionId, ih->getId());
					nodeEntry_off_t spaceN =  comp->getSpaceNeeded(entry);
					node->left -= spaceN;
				}
				
				if (BTREE_DEBUG_MODE) {
					cout << endl << "po wstawieniu: "<< defaultComparator->nodeToString(node) << endl;
				}
				
				err = ih->freeNode(cnode);
				return err;
			} else {
				// jestesmy w wezle wewnetrznym
				NodeEntry* entry = comp->getItem(0, node, entry);
				NodeEntry* after = (NodeEntry*)node->afterAddr();
				nodeAddress_t child = node->firstChild;
				NodeEntry* prev = NULL, *childEntry = NULL;
				
				while (entry < after) {
					if (comp->compare(entry) > 0) {
						// zawartosc comparatora jest wieksza niz klucza w wezle. trzeba sprawdzic kolejny klucz
						child = entry->child;
						prev = childEntry;
						childEntry = entry;
						entry = comp->getNextItem(entry);
					} else {
						// zawartosc comparatora jest mniejsza lub rowna od aktualnego klucza. trzeba wstawic do drzewa przed aktualnym kluczem
						
						break; // wyjscie z petli while. w child jest wskaznik do wezla przed aktualnym kluczem
					}
				}
				//albo jestesmy za wszystkimi kluczami, albo bylo break. w child jest wlasciwy wskaznik. entry jest za wskaznikiem
				CachedNode* cchildN;
				Node* childN;
				if ((err = ih->getNode(cchildN, child))) {
					return err;
				}
				if ((err = cchildN->getNode(childN, cnode->childPriority(), NODE_WRITE))) {
					return err;
				}
				if (BTREE_DEBUG_MODE) {
					cout << endl << "syn: "<< defaultComparator->nodeToString(childN) << endl;
				}
				NodeEntry* next;
				testSize:
				switch (canTakeExtraKey(cchildN)) {
					case SPLIT_NEEDED:
						if ((err = splitChild(cnode, cchildN, (char*)entry))) {
							return err;
						} //on zwolnil childN (moze nieslusznie)
						if (comp->greaterThan(entry)) {
							child = entry->child;
						}
						if ((err = ih->getNode(cchildN, child))) {
							return err;
						}
						if ((err = cchildN->getNode(childN, cnode->childPriority(), NODE_WRITE))) {
							return err;
						}
						break;
					case DELETE_NEEDED:
						//trzeba zapewnic ze po usunieciu childN i cchildN bedzie dobrze podpiety bo dalej sie ich uzywa
						next = (char*)entry >= node->afterAddr() ? NULL : entry;
						if ((err = remove(prev, cchildN, next, childEntry, cnode))) {
							return err;
						}
						if (!defaultComparator->canTakeExtraKey(node)) {
							goto testSize; //na wypadek gdyby usuwanie przesunelo dlugi klucz do cnode'a i nie mogl on przyjac nowego klucza
						}
						//tutaj nadal musimy miec cnode'a i childN
						break;
					default:
						;
				}
				//tutaj musi byc zapewnione ze childN moze przyjac kolejny klucz. balansowanie i usuniecie tego nie zapewniaja
				if (BTREE_DEBUG_MODE) {
					cout << endl << "po wstawieniu wewnetrzny: "<< defaultComparator->nodeToString(node) << endl;
				}
				
				if ((err = ih->freeNode(cnode))) {
					return err;
				}
				node = cchildN->getNode();
				cnode = cchildN;
			} // if (node->leaf)
		} //while true
	} // addEntry

	void BTree::changeKey(Node* node, NodeEntry* entry, Comparator* comp) {
		
		char* after = node->afterAddr();
		char* next = (char*) comp->getNextItem(entry);
		
		long currentKeySize = next - (char*)entry - sizeof(NodeEntry);  
		long newKeySize = comp->getSpaceNeeded();
		long distance = newKeySize - currentKeySize;

		//defaultComparator->checkSize(node, distance);
		assert (node->size() + distance <= MAX_NODE_CAPACITY);
		
		if (distance != 0) {
			//jest roznica dlugosci miedzy starym kluczem a nowym. prawdopodobnie trzeba bedzie cos przesuwac
			char* dest = next + distance;
			long bytes2move = after - next;
			if (bytes2move > 0) {
				//klucz ktory podmieniamy nie jest ostatni, trzeba przesunac reszte wpisow
				memmove(dest, next, bytes2move);
			}
			node->left -= distance;
			
			defaultComparator->checkSize(node);
		}
		
		comp->putKey(entry);
	}
	
	template <class I>
	void BTree::changeParentKey(I* entryWithKey, Node* parent, NodeEntry* nextPtrEntry) {
		Comparator* comp = defaultComparator->emptyClone();
		comp->getValue(entryWithKey);
		
		if (BTREE_DEBUG_MODE) {
			cout << endl << "comp: " << comp->toString() << endl; 
		}
		
		changeKey(parent, nextPtrEntry, comp);
		delete comp;
	}
	
	nodeSize_t BTree::balanceSize(nodeSize_t secondSize, Node* node) {
		int bothSize = node->size() + secondSize;
		return node->size() - (bothSize / 2); //tyle danych trzeba przeniesc?
	}
	
	template <class I>
	char* BTree::getBalanceBorder(Node* from, nodeSize_t toSize, I* &lastEntry, bool frontTransfer, NodeEntry* secondPtrEntry) {
		//to na pewno dziala gdy balansujemy liscie
		lastEntry = NULL;
		nodeEntry_off_t parentEntrySize = Node::distanceInNode(secondPtrEntry, defaultComparator->getNextItem(secondPtrEntry)); 
		long fromSize, toSizeTmp, fromSizeTmp, newDiff, diff;
		int times, distance;
		I* tmp;
		//long parentEntrySize = xxx;
		
		if (frontTransfer) {
			fromSize = from->size();
			times = 1; //toSize bedzie sie zwiekszal
		} else {
			fromSize = 0;
			toSize += from->size();
			times = -1; //toSize bedzie sie zmniejszal
		}
		
		toSizeTmp = toSize;
		fromSizeTmp = fromSize;
		I* after = (I*)from->afterAddr();
		newDiff = diff = abs(fromSize - toSize);
		I* entry = (I*)from->afterHeader();
		
		while (true) {	
			tmp = defaultComparator->getNextItem(entry);
			if (tmp >= after) {
				//przenosimy tylko ostatni wpis
				break;
			}
			distance = Node::distanceInNode(entry, tmp);
			
			if (!from->leaf) {
				toSizeTmp += times * parentEntrySize;
				fromSizeTmp -= times * distance;
				parentEntrySize = distance;
				newDiff = abs(fromSizeTmp - toSizeTmp);
			} else {
				newDiff -= 2*distance;
			}
			
			if (abs(newDiff) >= abs(diff)) {
				break;
			}
			
			toSize = toSizeTmp;
			fromSize = fromSizeTmp;
			diff = newDiff;
			lastEntry = entry;
			entry = tmp;
			
		}
		
	//	assert((toSize >= defaultComparator->MIN_NODE_SIZE_ALLOWED) && (toSize <= MAX_NODE_CAPACITY));
	//	assert((fromSize >= defaultComparator->MIN_NODE_SIZE_ALLOWED) && (fromSize <= MAX_NODE_CAPACITY));
		assert(lastEntry);
		return (char*) entry;
	}
	
	template <class I>
	void BTree::balanceBack(Node* to, Node* from, Node* parent, NodeEntry* fromPtrEntry, I* entryType) {
		I* lastEntry = NULL;
		nodeEntry_off_t parentEntrySize = 0;
		//wyliczyc granice
		//I* balancePtr = (I*) (from->afterHeader() + balanceSize(to->size(), from));
		
		if (BTREE_DEBUG_MODE) {
			cout << endl << "parent: "<< defaultComparator->nodeToString(parent) << endl;
			cout << endl << "przenosze z wezla: "<< defaultComparator->nodeToString(from) << endl;
			cout << endl << "przenosze do wezla: "<< defaultComparator->nodeToString(to) << endl;
		}
		
		//przesuwamy z nastepnika do poprzednika
		//char* afterFrom = from->afterAddr();
		char* endFrom = getBalanceBorder(from, to->size(), lastEntry, true, fromPtrEntry); //zwraca pierwszy bajt poza obszarem do przeniesienia
		
		if (!to->leaf) {
			//skopiowac klucz z ojca
			Comparator* comp = defaultComparator->emptyClone();
			comp->getValue(fromPtrEntry);
			// nowy wpis na koncu to
			NodeEntry* parentEntry = (NodeEntry*) to->afterAddr();
			parentEntry->logicalID = fromPtrEntry->logicalID;
			parentEntry->child = from->firstChild;
			//wstawic klucz na koniec to
			comp->putKey(parentEntry);
			
			from->firstChild = ((NodeEntry*)lastEntry)->child;
			//wstaw za to->afterAddr, ustaw temu wpisowi firstChild
			//ustaw firstChild z lastEntry
			parentEntrySize = comp->getSpaceNeeded(parentEntry);
			to->left -= parentEntrySize;
			
		}
		
		//przed przesuwaniem danych trzeba skopiowac klucz do ocja
		changeParentKey(lastEntry, parent, fromPtrEntry);
		
		char* src = from->afterHeader();
		char* startingPoint = to->leaf ? endFrom : ((char*)lastEntry);
		long bytesBalanced = startingPoint - src;
		char* dest = to->afterAddr();
		
		//przesuwamy dane pomiedzy wezlami. pomijajac wpis ktory powinien byc w ojcu
		memcpy(dest, src, bytesBalanced);
		
		//trzeba jeszcze dosunac do poczatku from pozostale w nim dane
		dest = src; //dosuwamy do poczatku
		src = endFrom; //zaczynamy od pierwszego niezbalansowanego
		long bytes2copy = from->afterAddr() - endFrom; //az do konca wezla from
		memmove(dest, src, bytes2copy);
		
		to->left -= bytesBalanced;
		from->left += bytesBalanced;
		
		defaultComparator->checkSize(to);
		defaultComparator->checkSize(from);
		
		if (!to->leaf) {
			from->left += parentEntrySize;
		}
		
		if (BTREE_DEBUG_MODE) {
			cout << endl << "parent: "<< defaultComparator->nodeToString(parent) << endl;
			cout << endl << "przeniesiono z wezla: "<< defaultComparator->nodeToString(from) << endl;
			cout << endl << "przeniesiono do wezla: "<< defaultComparator->nodeToString(to) << endl;
		}
		
	}
	
	template <class I>
	void BTree::balanceForward(Node* from, Node* to, Node* parent, NodeEntry* toPtrEntry, I* entryType) {
		I* lastEntry = NULL;
		//nodeSize_t bs = balanceSize(to->size(), from);
		//I* balancePtr = (I*) (from->afterAddr() - bs);
		
		if (BTREE_DEBUG_MODE) {
			cout << endl << "parent: "<< defaultComparator->nodeToString(parent) << endl;
			cout << endl << "przenosze z wezla: "<< defaultComparator->nodeToString(from) << endl;
			cout << endl << "przenosze do wezla: "<< defaultComparator->nodeToString(to) << endl;
		}
		//znajdz wpis od ktorego mamy przesuwac i ostatni tuz przed nim
		char* endFrom = getBalanceBorder(from, to->size(), lastEntry, false, toPtrEntry); //zwraca pierwszy bajt poza obszarem do przeniesienia  
		
		NodeEntry* fromParentEntry;
		long parentEntrySize, bytes2copy = 0;
		Comparator *comp;
		char* fromTo;
		long spaceNeeded = 0, newParentEntrySize;
		if (!to->leaf) {
			comp = defaultComparator->emptyClone();
			comp->getValue(toPtrEntry); //pobrac klucz z ojca
			parentEntrySize = comp->getSpaceNeeded(lastEntry); // trzeba przesunac dalej zeby zmiescil sie wpis z ojca 
			spaceNeeded += parentEntrySize; //trzeba przesunac dalej zeby znmiescil sie wpis z ojca
			lastEntry = (I*)endFrom; //w lastEntry mamy wpis ktory przejdzie do ojca
			endFrom = (char*)defaultComparator->getNextItem((I*)endFrom); // a przenosic do nastepnika zaczynamy od nastepnego wpisu
			fromTo = (char*)lastEntry; //przenosimy wpisy az do wpisu ktory przejdzie do ojca
			newParentEntrySize = Node::distanceInNode(lastEntry, endFrom); //zmniejszamy o rozmiar wpisu ktory przejdzie do ojca
			//spaceNeeded -= newParentEntrySize;
			bytes2copy = - parentEntrySize;
		} else {
			
		}
		
		//zrob miejsce w nastepniku
		//spaceNeeded +=
		spaceNeeded += Node::distanceInNode(endFrom, from->afterAddr());
		bytes2copy += spaceNeeded;
		char* dest = to->afterHeader() + spaceNeeded;
		memmove(dest, to->afterHeader(), to->size());
		
		//przenies z poprzednika
		memcpy(to->afterHeader(), endFrom, bytes2copy);
		
		//popraw left
		to->left -= spaceNeeded;
		from->left += bytes2copy;
		
		if (!to->leaf) {
			from->left += newParentEntrySize;
			fromParentEntry = (NodeEntry*) (to->afterHeader() + bytes2copy); //wpis z ojca bedzie wstawiany pomiedzy nwpisami z poprzednika a wpisami nastepnika
			comp->putKey(fromParentEntry);
			fromParentEntry->child = to->firstChild;
			//fromParentEntry->logicalID = toPtrEntry->logicalID
			//to->left -= parentEntrySize;
			to->firstChild = ((NodeEntry*)lastEntry)->child;
		}
		
		//skopiuj ostatni klucz (ktory nie bedzie przenoszony) do ojca. do entry wskazujacego na nastepnika
		changeParentKey(lastEntry, parent, toPtrEntry);
		
		defaultComparator->checkSize(to);
		defaultComparator->checkSize(from);
		
		if (BTREE_DEBUG_MODE) {
			cout << endl << "parent: "<< defaultComparator->nodeToString(parent) << endl;
			cout << endl << "przeniesiono z wezla: "<< defaultComparator->nodeToString(from) << endl;
			cout << endl << "przeniesiono do wezla: "<< defaultComparator->nodeToString(to) << endl;
		}
	}
	
	int BTree::remove(NodeEntry* prev, CachedNode* &cchildN, NodeEntry* next, NodeEntry* childEntry, CachedNode* cparent) {
		//cchildN jest zarezerwowany i wczytany do zapisu
		assert(prev || next); //musi byc dostepny przynajmniej jeden brat
		cparent->setDirty();
		int err = 0;
		CachedNode* cbrother;
		Node *brother, *child;
		child = cchildN->getNode();
		if (next) {
			//cout << "1sza opcja remove" << endl;
			//mozemy zalockowac kolejny wezel bez obawy o deadlock
			if ((err = ih->getNode(cbrother, next->child))) {
				return err;
			}
			if ((err = cbrother->getNode(brother, cchildN->siblingPriority(), NODE_WRITE))) {
				return err;
			}
			
			cchildN->setDirty();
			cbrother->setDirty();
			if (isBalancePossible(brother, child)) {
				//cout << "1sza opcja balance" << endl;
				if (child->leaf) {
					RootEntry* type;
					balanceBack(child, brother, cparent->getNode(), next, type);
				} else {
					NodeEntry* type;
					balanceBack(child, brother, cparent->getNode(), next, type);
				}
				if ((err = ih->freeNode(cbrother))) {
					return err;
				}
			} else {
				//cout << "1sza opcja merge" << endl;
				
				if ((err = merge(cchildN, cbrother, cparent, next))) {
					return err; //przesun next do child i usun entry wskazujace na next
				}
				//cbrother jest usuwany, cchildN pozostaje
			}
			
			return err;
		}
		//cout << "2ga opcja remove" << endl;
		//return 0;
		//nie bylo next wiec jest prev
		if ((err = ih->getNode(cbrother, prev->child))) {
			return err;
		}
		if (cbrother->tryGetNode(brother, cchildN->siblingPriority())) {
			/* nie udalo sie zablokowac poprzedniego brata do zapisu,
			 * trzeba zwolnic wezel, przepuscic czytelnika po lisciach i zalockowac oba wezly
			 */
			if ((err = cchildN->unlock())) {
				return err;
			}
			if ((err = cbrother->getNode(brother, cchildN->siblingPriority(), NODE_WRITE))) {
				return err;
			}
			if ((err = cchildN->getNode(child, cbrother->siblingPriority(), NODE_WRITE))) {
				return err;	
			}
		}
		cchildN->setDirty();
		cbrother->setDirty();
		// mamy zalockowane oba wezly
		if (isBalancePossible(brother, child)) {
			if (child->leaf) {
				RootEntry* type;
				balanceForward(brother, child, cparent->getNode(), childEntry, type);
			} else {
				NodeEntry* type;
				balanceForward(brother, child, cparent->getNode(), childEntry, type);
			}
			if ((err = ih->freeNode(cbrother))) {
				return err;
			}
		} else {
			//polacz prev i child
			if ((err = merge(cbrother, cchildN, cparent, childEntry))) {
				return err;
			}
			//cchildN jest usuwany, cbrother zostaje
			cchildN = cbrother;
		}
		
		return err;
	} //remove
	
	int BTree::merge(CachedNode* cto, CachedNode* cfrom, CachedNode* cparent, NodeEntry* fromPtrEntry) {
		int err;
		Node *to = cto->getNode();
		Node *from = cfrom->getNode();
		Node* parent = cparent->getNode();
		
		assert(to->size() + from->size() + defaultComparator->maxEntrySize() <= MAX_NODE_CAPACITY);
		
		if (!to->leaf) {
			//skopiowac klucz z parent
			Comparator* comp = defaultComparator->emptyClone();
			comp->getValue(fromPtrEntry);

			NodeEntry* copiedEntry = (NodeEntry*) to->afterAddr();
			copiedEntry->child = from->firstChild;
			copiedEntry->logicalID = fromPtrEntry->logicalID;
			to->left -= comp->getSpaceNeeded(copiedEntry);
			
			assert(to->size() + from->size() + defaultComparator->maxEntrySize() <= MAX_NODE_CAPACITY);
			
			comp->putKey(copiedEntry);
			delete comp;
		} else {
			to->nextLeaf = from->nextLeaf;
		}
		
		//usun fromPtrEntry razem z kluczem
		char* src = (char*) defaultComparator->getNextItem(fromPtrEntry);
		long droppedSize = Node::distanceInNode(fromPtrEntry, src);
		long bytes2copy = Node::distanceInNode(src, parent->afterAddr());
		assert((bytes2copy >= 0) && (bytes2copy <= MAX_NODE_CAPACITY));
		memmove(fromPtrEntry, src, bytes2copy);
		parent->left += droppedSize;
		
		//przeniesc zawartosc z from
		memcpy(to->afterAddr(), from->afterHeader(), from->size());

		to->left -= from->size();
		
		if ((err = ih->dropNode(cfrom))) {
			return err;
		}
		
		return 0;
	}
	
	bool BTree::isBalancePossible(Node* from, Node* to) {
		
		//assert(from->size() + to->size() < 2* )
		
		// 2 * defaultComparator->maxEntrySize() bo jeden wpis przejdzie z ojca a drugi chcemy dodac
		return (from->size() + to->size() + 2 * defaultComparator->maxEntrySize() > MAX_NODE_CAPACITY);
		
		/*
		return (
				(from->size() >= defaultComparator->MIN_NODE_SIZE_ALLOWED + defaultComparator->maxEntrySize())
				&&
				(to->size() <= MAX_NODE_CAPACITY - 2 * defaultComparator->maxEntrySize())
				&&
				(from->size() + to->size() >= 2 * defaultComparator->MIN_NODE_SIZE_ALLOWED + defaultComparator->maxEntrySize() - 1)
				);
				*/
	}
	
	int BTree::find(Comparator *comp, CachedNode *&nodeResult, RootEntry *&entryResult) {
		int err = 0, childPriority;;
		CachedNode* ccurrentNode;
		Node* currentNode;
		rootSem->lock_read();
		if ((err = ih->getNode(ccurrentNode, rootAddr, true))) {
			return err;
		}
		if ((err = ccurrentNode->getNode(currentNode, rootLevel, NODE_READ))) {
			return err;
		}
		
		if ((err = rootSem->unlock())) {
			return err;
		}
		
	//	cout << endl << comp->nodeToString(currentNode) << endl;
		while (true) { //iteracja zamiast rekurencji
			
			if (BTREE_DEBUG_MODE) {
				cout << endl << "wyszukuje w wezle: "<< defaultComparator->nodeToString(currentNode) << endl;
			}
			
			if (currentNode->leaf) {
					RootEntry* after = (RootEntry*) currentNode->afterAddr();  
					RootEntry* entry = comp->getItem(0, currentNode, entry);
					
					while (entry < after) {
						if (comp->compare(entry) > 0) {
							// zawartosc comparatora jest wieksza niz klucza w wezle. trzeba sprawdzic kolejny klucz
							entry = comp->getNextItem(entry); 					
						} else {
							// zawartosc comparatora jest mniejsza lub rowna aktualnie ogladanej
							nodeResult = ccurrentNode;
							entryResult = entry;//->clone();
							return 0; 
						}
					}
					// klucz w comparatorze jest najwiekszy w tym wezle. nie znalezlismy poszukiwanego klucza
					entryResult = NULL;
					/** jesli mozliwa jest sytuacja ze w lisciu nieostatnim bedziemy szukac klucza wiekszego niz najwiekszy w tym lisciu
					 *  to trzeba to zmienic. wtedy nie powinnismy zwalniac wezla nawet jesli nic nie znajdziemy?
					 */
					err = ih->freeNode(ccurrentNode);
					return err;
				} else {
					NodeEntry* entry = comp->getItem(0, currentNode, entry);
					NodeEntry* after = (NodeEntry*)currentNode->afterAddr();
					nodeAddress_t child = currentNode->firstChild;
					
					while (entry < after) {
						if (comp->compare(entry) > 0) {
							// zawartosc comparatora jest wieksza niz klucza w wezle. trzeba sprawdzic kolejny klucz
							child = entry->child;
							entry = comp->getNextItem(entry);
						} else {
							// zawartosc comparatora jest mniejsza lub rowna od aktualnego klucza. trzeba szukac w drzewie przed aktualnym kluczem
							
							break; // wyjscie z petli while. w child jest wskaznik do wezla przed aktualnym kluczem
						}
					} // while
					childPriority = ccurrentNode->childPriority();
					if ((err = ih->freeNode(ccurrentNode))) {
						return err;
					}//nie trzeba najpierw zajac a potem zwolnic. zadna inna transakcja go nie zmodyfikuje
					if ((err = ih->getNode(ccurrentNode, child))) {
						return err;
					}
					if ((err = ccurrentNode->getNode(currentNode, childPriority, NODE_READ))) {
						return err;
					}
			} // if else (node->leaf)
			
		} // while true
	} //find
	
	BTree::BTree(Comparator* defaultComparator, IndexHandler* ih) {
		this->defaultComparator = defaultComparator;
		this->ih = ih;
		rootSem = new RWUJSemaphore();
		rootSem->init();
		rootLevel = ROOT_PRIORITY;
	}
	
	void BTree::init(nodeAddress_t rootAddr, int rootLevel) {
		this->rootAddr = rootAddr;
		this->rootLevel = rootLevel;
	}
	
	BTree::~BTree() {
		delete defaultComparator;
		delete rootSem;
	}
	
	Comparator* BTree::getNewComparator() {
		return defaultComparator->emptyClone();
	}
	
	/*int BTree::getNode(Node *&node, nodeAddress_t nodeID, int priority) {
		return IndexHandler::getNode(node, nodeID, priority, defaultComparator);
	}*/
	
	
		
}
