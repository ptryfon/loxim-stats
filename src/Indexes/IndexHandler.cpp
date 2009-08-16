#include <Indexes/IndexHandler.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <Indexes/Cleaner.h>
#include <Indexes/IndexManager.h>
#include <Util/Concurrency.h>

using namespace std;
using namespace Util;
namespace Indexes {

	//inicjalizacja zmiennych statycznych
	Mutex* IndexHandler::newNodeMutex = NULL;
	unsigned long IndexHandler::usedCount = 0;
	nodeAddress_t IndexHandler::droppedNode = NEW_NODE;
	nodeAddress_t IndexHandler::droppedLastNode = NEW_NODE;
	nodeAddress_t IndexHandler::nextFreeNodeAddr = 0;
	unsigned long IndexHandler::cachedCount = 0;
	Mutex* IndexHandler::IOMutex = NULL;
	Monitor* IndexHandler::cacheMonitor = NULL;
	list<nodeAddress_t> IndexHandler::droppedCachedNodes;
	int IndexHandler::index_fd = -1;
	IndexHandler::cacheMapT IndexHandler::cache;
	IndexHandler::freeHandlers_t IndexHandler::freeHandlers;
	
	IndexHandler::IndexHandler(string name, string root, string field, LogicalID* lid) {
		this->indexHandler(name, root, field);
		setLid(lid);
	}
	
	IndexHandler::IndexHandler(string name, string root, string field) {
		this->indexHandler(name, root, field);
	}
	
	void IndexHandler::indexHandler(string name, string root, string field) {
		this->name = name;
		this->root = root;
		this->field = field;
		
		firstNode = lastNode = NEW_NODE;
		tree = NULL;
		statusMonitor = new Monitor();
		state = BUILDING;
		readingTransaction = 0;
	}
	
	
	string IndexHandler::getName() {return name;}
	string IndexHandler::getRoot() {return root;}
	string IndexHandler::getField() {return field;}
	string IndexHandler::getType() {return tree->getNewComparator()->typeToString();}
	LogicalID* IndexHandler::getLogicalID() {return lid;}
	lid_t IndexHandler::getId() {
		assert(lid != NULL);
		return lid->toInteger();
	}
	
	int IndexHandler::addEntry(DataValue *value, RootEntry* entry, bool buildingTransaction) {
		int err = 0, err1 = 0;
		
		if ((err = userEnter(buildingTransaction))) {
			return err;
		}
		
		Comparator* comp = tree->getNewComparator();
		comp->setEntry(entry);

		if (!(err1 = comp->setValue(value))) {
			err1 = tree->addEntry(comp);
		} 
		
		delete comp;
		if ((err = userLeave(buildingTransaction))) {
			return err;
		}
		
		return err1;
	}
	
	int IndexHandler::createNew(Comparator* comp) {
		tree = new BTree(comp, this);
		return reset();	
	}
	
	int IndexHandler::reset() {
		return tree->createRoot();
	}
	
	void IndexHandler::load(Comparator* comp) {
		tree = new BTree(comp, this);
		//tree->init(rootAddr, rootLevel);
	}
	
	CachedNode* IndexHandler::getDropped2flush() {

		//splukujemy ostatni z usunietych
		CachedNode* cnode = cacheLookup(droppedCachedNodes.back());
		droppedCachedNodes.pop_back();
		assert(cnode->users == 0);
		cnode->users = 1; //na potrzeby testow i asercji
		cnode->getNode();
		cnode->users = 0; //aby przywrocic normalny stan
		cnode->setDirty();
		return cnode;
	}
	
	int IndexHandler::removeFromList(Node* node) {
		//return 0;
		nodeAddress_t prev, next, droppedId;
		prev = node->prevNode;
		next = node->nextNode;
		droppedId = node->id;
		int err = 0;
		CachedNode* listNode;
		if (prev != NEW_NODE) {
			err = getNode(listNode, prev);
			err = listNode->getNode(node); //z pominieciem blokady zapisu/odczytu
			node->nextNode = next;
			listNode->setDirty();
			err = freeNode(listNode, true, false);
		} else {
			//cnode to pierwszy wezel na liscie
			firstNode = next;
		}
		if (next != NEW_NODE) {
			err = getNode(listNode, next);
			err = listNode->getNode(node); //z pominieciem blokady zapisu/odczytu
			node->prevNode = prev;
			listNode->setDirty();
			err = freeNode(listNode, true, false);
		} else {
			//usuwamy ostatni wezel indeksu
			lastNode = prev;
		}
		return err;
	}
	
	int IndexHandler::addToList(Node* node) {
		//return 0;
		CachedNode* listNode;
		Node* loadedNode;
		int err = 0;
		//ta metoda jest wywolywana wewnatrz mutexa
		if (firstNode != NEW_NODE) {
			err = getNode(listNode, firstNode);
			err = listNode->getNode(loadedNode);
			loadedNode->prevNode = node->id;
			listNode->setDirty();
			//node->prevNode = 
			err = freeNode(listNode, true, false);
		} else {
			lastNode = node->id;
		}
		//node bedzie teraz pierwszy na liscie
		node->prevNode = NEW_NODE;
		node->nextNode = firstNode;
		firstNode = node->id;
		return err;
	}
	
	int IndexHandler::dropNode(CachedNode* cnode) {
		/* cnode na pewno bedzie juz mial ustawiony lock do zapisu.
		 * to znaczy ze wszyscy inni zrobili juz freeNode */
		Node* node;
		int err;
		if ((err = cnode->getNode(node))) {
			return err;
		}
		
		err = removeFromList(node);
		cnode->setDirty();
	
		{	
			Mutex::Locker l(*newNodeMutex);

			nodeAddress_t droppedTmp = droppedNode;
			droppedNode = node->id;
			node->nextNode = droppedTmp;
			//dodajemy na poczatku
			droppedCachedNodes.push_front(droppedNode);
			if (droppedLastNode == NEW_NODE) {
				//lista usunietych byla pusta
				droppedLastNode = droppedNode;
			}

			err = freeNode(cnode, false);
		}
		return err;
	}
	
	int IndexHandler::getNode(CachedNode *&cNode, nodeAddress_t nodeID, bool first) {
		int err;
		//CachedNode* cNode;
		Node* node;
		if (nodeID == NEW_NODE) {
			//mutex write nocache down
			
			newNodeMutex->lock();		
			if (droppedNode != NEW_NODE) {
				// mamy jakies zwolnione bloki, trzeba je uzyc aby zmniejszyc fragmentacje
				//musimy wczytac zeby dostac adres nastepnego wolnego
				//trzeba usunac ten wezel ze zbioru usunietych bo za chwile bedzie pobierany
				//pobieramy pierwszy w kolejce jesli jeszcze jest w cache'u
				if ((!droppedCachedNodes.empty())/* && (droppedCachedNodes.front() == droppedNode)*/) {
					droppedCachedNodes.pop_front();
				}
				if ((err = getCachedNode(cNode, droppedNode, first))) {
					return err;
				}
				if ((err = cNode->getNode(node))) {
					return err;
				}
				//a moze po jednym dla kazdego pliku?
				if (droppedNode == droppedLastNode) {
					//wlasnie wyjmujemy ostatni z kolejki usunietych
					droppedLastNode = NEW_NODE;
				}
				droppedNode = node->init(droppedNode);
				assert(((droppedNode != NEW_NODE) && (droppedLastNode != NEW_NODE)) || (droppedNode == droppedLastNode));
				cNode->setDirty();
				if ((err = addToList(node))) {
					return err;
				}
				newNodeMutex->unlock();
				
				return err;
			} else {
				//nie mamy zadnych nieuzywanych blokow, trzeba zarezerwowac kolejny wolny
				if (nextFreeNodeAddr >= MAX_NODE_ADDR) {
					//mozna tu pewnie jeszcze jeden zarezerwowac. ale trzeba uwazac na przekroczenie zakresu i 0
					return EFBIG | ErrIndexes;
				}
				nodeAddress_t newNodeID = nextFreeNodeAddr;
				nextFreeNodeAddr += NODE_SIZE;
				
				newNodeMutex->unlock();
				
				if ((err = cacheMonitor->lock())) {
					return err;
				}
				while ((usedCount >= MAX_CACHED_NODES) && first) {
					cacheMonitor->wait();
				}
				
				if ((err = flushIfNecessary())) {
					return err;
				}
				node = (Node*) malloc(NODE_SIZE);
				if (node == NULL) {
					return errno | ErrIndexes;
				}
				node->init(newNodeID);
				
				cNode = new CachedNode(node);
				cNode->setDirty();
				addToCacheMap(cNode);
				err = cacheMonitor->unlock();
				newNodeMutex->unlock();
				err = addToList(node);
				newNodeMutex->lock();
				return err;
			}
		} else { //nodeID != NEW_NODE
			
			if ((err = getCachedNode(cNode, nodeID, first))) { //jesli nie bedzie to cos wyrzuci i odda pusty obiekt
				return err; 
			}	
			return err;
		} // nodeID != NEW_NODE
			return 0;
	}

	CachedNode* IndexHandler::cacheLookup(nodeAddress_t nodeID) {
		CachedNode* cnode = NULL;
		cacheMapT::iterator it = cache.find(nodeID);
		if (it != cache.end()) {
			cnode = it->second;
		}
		return cnode;
	}
	
	const char* IndexHandler::printHandlers() {
		freeHandlers_t::iterator it;
		stringstream s;
		for(it = freeHandlers.begin(); it != freeHandlers.end(); it++) {
			s << "[ " << it->first << " -> " << it->second.size() << " ] ";
		}
		return s.str().c_str();
	}
	
	void IndexHandler::addHandler(int priority) {
		freeHandlers[priority].insert(this);
	}
	
	void IndexHandler::removeHandler(int priority) {
		freeHandlers_t::iterator it = freeHandlers.find(priority);
		assert(it != freeHandlers.end());
		//usuwamy wiec aktualny handler z tego priorytetu
		if ((it->second).size() == 1) {
			//ten priorytet ma tylko aktualnie usuwany cnode. mozna wyrzucic caly wpis
			freeHandlers.erase(it);
		} else {
			//sa jeszcze inne obiekty o tym priorytecie. trzeba usunac tylko ten handler 
			(it->second).erase(this);
		}
	}
	
	void IndexHandler::changePriority(int oldp,  int newp) {
		removeHandler(oldp);
		//dodajemy handler pod nowy priorytet
		addHandler(newp);
	}
	
	void IndexHandler::setFree(CachedNode* cnode) {
		//tree->rootSem->lock_read(); //jestem wewnatrz locku cache? czy tu trzeba lockowac rootSem? jaka kolejnosc?
		
		int newPriority = cnode->depth;
		int depth = tree->rootPriority() - cnode->depth;
		if (freeNodes.empty()) {
			addHandler(depth);
		//	int s = freeHandlers[newPriority].size();
		//	cout << s << endl;
		} else {
			int oldPriority = freeNodes.begin()->first;
			
			if (newPriority < oldPriority) {
				//nowy wezel lezy glebiej
				changePriority(tree->rootPriority() - oldPriority, depth);
			}	
		}
		freeNodes[newPriority].insert(cnode);
		//const char* txt = printHandlers();
		//cout << txt << endl;
		//tree->rootSem->unlock();
	}
	
	void IndexHandler::setNotFree(CachedNode* cnode) {
		//tree->rootSem->lock_read(); //jestem wewnatrz locku cache? czy tu trzeba lockowac rootSem? jaka kolejnosc?
		//trzeba znalezc zbior wezlow o tej glebokosci
		int oldPriority = cnode->depth; //oldPriority = odleglosc wezla od korzenia
		int depth = tree->rootPriority() - cnode->depth;
		freeNodes_t::iterator it = freeNodes.find(oldPriority); 
		if (it == freeNodes.end()) {
			//ten cnode nigdy nie byl oznaczony jako wolny - nie ma go w strukturze 
			//tree->rootSem->unlock();
			return;
		}
		set<CachedNode*> &s = it->second;
		set<CachedNode*>::iterator cit = s.find(cnode);
		if (cit == s.end()) {
			//ten cnode nigdy nie byl oznaczony jako wolny - nie ma go w strukturze
			//tree->rootSem->unlock();
			return;
		}
		//ze zbioru usuwamy zadany wezel
		s.erase(cit);
		if (s.empty()) {
			freeNodes.erase(it);
		}
		if (freeNodes.empty()) {
			removeHandler(depth);
		} else {
			int newPriority = freeNodes.begin()->first; 
			if (newPriority > oldPriority) {
				//usunelismy ostatni wezel lezacy tak gleboko, wszystkie inne leza plycej
				changePriority(depth, tree->rootPriority() - newPriority);
			}
		}
		//tree->rootSem->unlock();
	}
	
	void IndexHandler::rootChange(int oldLevel, int newLevel) {
		assert(oldLevel != newLevel);
		//freeNodes_t::iterator it;
		set<CachedNode*>::iterator cit;
		vector<CachedNode*> tmp;
		cacheMonitor->lock();
		
		if (!freeNodes.empty()) {
			int priority = freeNodes.begin()->first;
			changePriority(oldLevel - priority, newLevel - priority);
		}
		
		/*
		for (it = freeNodes.begin(); it != freeNodes.end(); it++) {
			for (cit = it->second.begin(); cit != it->second.end(); cit++) {
				tmp.push_back(*cit);
			}
		}
		
		for (unsigned int i = 0; i < tmp.size(); i++) {
			setNotFree(tmp.at(i));
		}
		*/
		tree->setRootPriority(newLevel);
		/*
		for (unsigned int i = 0; i < tmp.size(); i++) {
			setFree(tmp.at(i));
		}
		*/
		cacheMonitor->unlock();
	}
	
	CachedNode* IndexHandler::localChoose2flush(freeHandlers_t::iterator /*handlerIt*/) {
		assert(freeNodes.size() > 0);
		freeNodes_t::iterator freeIt = freeNodes.begin();
		set<CachedNode*> &s = freeIt->second;
		assert(!s.empty());
		set<CachedNode*>::iterator it = s.begin();
		CachedNode* result = *it;
		
		setNotFree(result);
		/*
		s.erase(it);
		if (s.empty()) {
			//nie ma juz wezlow o tym priorytecie
			freeNodes.erase(freeIt);
			
			//uaktualnienie globalnej struktury
			
			//usuwany wpis z mapy bo nie ma juz wolnych wezlow o tej glebokosci
			freeHandlers.erase(handlerIt);
			if (!freeNodes.empty()) {
				//jesli sa jeszcze jakies wezly w tym handlerze to trzeba go dodac do wlasciwego priorytetu
				int priority = handlerIt->first;
				freeHandlers[priority].insert(this);
			}
		}
		*/
		return result;
	}
	
	CachedNode* IndexHandler::choose2flush() {
		CachedNode* cnode = NULL;
		{
			Mutex::Locker l(*newNodeMutex);
			assert(!(freeHandlers.empty() && droppedCachedNodes.empty()));
			if (!droppedCachedNodes.empty()) {
				cnode = getDropped2flush();		
			}
		}
		
		if (cnode) {
			return cnode;
		}
		
		freeHandlers_t::iterator handlerIt = freeHandlers.end(); //ostatni element mapy wskazuje na najwieksza glebokosc wolnych wezlow
		handlerIt--;
		//cout << handlerIt->first << endl;
		set<IndexHandler*> &s = handlerIt->second; //zbior handlerow o najwiekszej glebokosci wolnych wezlow
		assert(!s.empty());
		set<IndexHandler*>::iterator it = s.begin(); //wskaznik na handlera
		IndexHandler* ih = *it;
		return ih->localChoose2flush(handlerIt);
	}
	
	/** operacje tylko na mapie i liczbie uzyc. i wymuszony zrzut na dysk
	 */
	int IndexHandler::getCachedNode(CachedNode* &cnode, nodeAddress_t nodeID, bool first) {
		//used mutex 1 down
		//cachemap write down
		int err;
		cacheMonitor->lock();
		
		//jesli wszystkie uzywane lub cacheLookup == NULL to czekaj
		while ( (!(cnode = cacheLookup(nodeID))) && (usedCount >= MAX_CACHED_NODES) && first) {
			cacheMonitor->wait();
		}
		
		if (cnode) {
			//taki wezel juz w cache'u, my tez go uzyjemy
			if (cnode->users == 0) { //jesli nie jest uzywany to trzeba zaznaczyc ze juz jest
				usedCount++;
				//usunac ze struktury wolnych
				//jesli byl w cache'u to ma juz ustawiony priorytet
				setNotFree(cnode);
			}
			cnode->users++;
			cacheMonitor->unlock();
			return 0;
		}
		
		err = flushIfNecessary();
		cnode = new CachedNode(nodeID);
		addToCacheMap(cnode);
		
		cacheMonitor->unlock();
				
		
		return err;
	}
	
	int IndexHandler::removeAndFlush(CachedNode* flushCNode) {
		cache.erase(flushCNode->getNodeID());
		cachedCount--;
		
		int err = flush(flushCNode);
		if (err) {
			printf("Indexes::CachedNode::flush() FATAL! Index inconsistence. errCode: %s\n", Errors::SBQLstrerror(err).c_str());
			exit(1);
		}
		delete flushCNode;
		return err;
	}
	
	// metoda zaklada ze jest wewnatrz monitora. 
	int IndexHandler::flushIfNecessary() {
		if ((cachedCount >= MAX_CACHED_NODES) && (usedCount < cachedCount)) {
			//caly cache jest wykorzystany, trzeba cos wyrzucic
			CachedNode* flushCNode = choose2flush();
			return removeAndFlush(flushCNode);
		}
		return 0;
	}
	
	int IndexHandler::flush(CachedNode* cnode) {
		if (!cnode->dirty) {
			return 0;
		}
		int err;
		{
			Mutex::Locker l(*IOMutex);
			err = flushNotSync(cnode);
		}
		return err;
	}
	
	int IndexHandler::flushNotSync(CachedNode* cnode) {
		if (lseek(index_fd, cnode->getNodeID(), SEEK_SET) == (off_t)-1) {
			return errno | ErrIndexes;
		}
		nodeSize_t toWrite = NODE_SIZE;
		Node* node;
		assert(cnode->users == 0);
		cnode->users = 1; //do testow
		cnode->getNode(node);
		char* buf = (char*) node;
		ssize_t count;
		while (toWrite > 0) {
			count = write(index_fd, buf, toWrite);
			//	jesli read > 0 lub errno == EINTR to wszystko ok. trzeba ponowic petle
			if ((count < 0) && (errno != EINTR)) {
				cout << "write error" << endl;
				return errno | ErrIndexes;
			}
			toWrite -= count;
			buf += count;
		}
		cnode->users = 0; //do testow. nie powinno to byc potrzebne bo zaraz po tej metodzie bedzie delete
		cnode->dirty = false;
		return 0;
	}
	
	//moznaby dodac jakis niskopriorytetowy watek zrzucajacy gdy nic sie nie dzieje
	
	int IndexHandler::readNodeNotSync(Node* node, nodeAddress_t nodeID) {
		if (lseek(index_fd, nodeID, SEEK_SET) == (off_t)-1) {
			return errno | ErrIndexes;
		}
		
		nodeSize_t toRead = NODE_SIZE;
		char* buf = (char*) node;
		ssize_t count;
		while (toRead > 0) {
			count = read(index_fd, buf, toRead);
			//	jesli count > 0 lub errno == EINTR to wszystko ok. trzeba ponowic petle
			if ((count <= 0) && (errno != EINTR)) {
				if (count < 0) {
					return errno | ErrIndexes;
				} else {
					//EOF? impossible. blad krytyczny
					//assert(false);
					return errno | ErrIndexes;
				}
			}
			toRead -= count;
			buf += count;
		}
		return 0;
	}
	
	int IndexHandler::readNode(Node* node, nodeAddress_t nodeID) {
		int err;
		Mutex::Locker l(*IOMutex);
		err = readNodeNotSync(node, nodeID);
		return err;
	}
	
	//setFree to to samo co keepOnDisk
	int IndexHandler::freeNode(CachedNode* cnode, bool setFree, bool unlock) {
		assert(cnode);
		assert(cnode->users >= 1);
		int err = 0;
		cacheMonitor->lock();
		if (unlock) {
			cnode->unlock();
		}
		//CachedNode * cnode = cacheLookup(node->id);
		if ((--cnode->users) == 0) {
			usedCount--;
			if (setFree) {
				this->setFree(cnode);
			} else {
				//err = removeAndFlush(cnode);
			}
			if (usedCount < MAX_CACHED_NODES) {
				cacheMonitor->signal();
			} else {
				//jesli zwolniono wezel i nadal uzywa sie nie mniej niz limit to wyrzuc z cache'a
				if (setFree) {
					//jesli !setFree to uz zostal zrzucony na dysk
					if ((err = flushIfNecessary())) {
						return err;
					}
				}
			}
		}
		cacheMonitor->unlock();
		return err;
	}
	
	void IndexHandler::clearCache() {
		cache.clear();
		usedCount = 0;
		cachedCount = 0;
	}
	
	int IndexHandler::init(string indexFileName) {
		int err;
		
		droppedNode = NEW_NODE;
		droppedLastNode = NEW_NODE;
		nextFreeNodeAddr = 0; //zaczynamy od 0
		index_fd = -1;
		freeHandlers.clear();
		droppedCachedNodes.clear();
		
		clearCache();
		cacheMonitor = new Monitor();
		newNodeMutex = new RecursiveMutex();
		IOMutex = new Mutex();
		if( ( index_fd = ::open( indexFileName.c_str(), O_RDWR | O_CREAT, S_IWUSR | S_IRUSR ) ) < 0 ) {
			err = errno; 
			printf("Cannot open file %s\n", indexFileName.c_str());
			return err | ErrIndexes;
		}
		return 0;
	}
	
	IndexHandler::~IndexHandler() {
		if (tree) {
			delete tree;
		}
		delete statusMonitor;
	}
	
	void IndexHandler::addToCacheMap(CachedNode* cnode) {
		cache[cnode->getNodeID()] = cnode;
		cnode->users++;
		cachedCount++;
		usedCount++;
	}
	
	int IndexHandler::flushAll() {
		cacheMonitor->lock();
		int err = 0;
		cacheMapT::iterator it;
		for (it = cache.begin(); it != cache.end(); it++) {
			(it->second)->users = 0; //do testow. podczas pracy aplikacji nie powinno byc potrzebne
			if ((err = flush(it->second))) {
				return err;
			}
		}
		clearCache();
		cacheMonitor->unlock();
		return 0;
	}
	
	void IndexHandler::setLid(LogicalID* lid) {
		this->lid = lid;
	}
	
	int IndexHandler::destroy() {
		
		//flushAll();
		fsync(index_fd);
		close(index_fd);
		
		delete cacheMonitor;
		delete newNodeMutex;
		delete IOMutex;
		return 0;
	}
	
	int IndexHandler::drop() {
		int err = 0;
		CachedNode* cnode = NULL;
		bool inCache = true;

		freeNodes_t::iterator it;
		set<CachedNode*>::iterator cit;

		freeNodes_t tmp = freeNodes;
		for(it = tmp.begin(); it != tmp.end(); it++) {
			for (cit = it->second.begin(); cit != it->second.end(); cit++) {
				cnode = *cit;
				setNotFree(cnode);
				if ((err = removeAndFlush(cnode))) {
					return err;
				}
			}
		}
		{
			Mutex::Locker l(*newNodeMutex);
		
			if (droppedNode != NEW_NODE) {
				//lista usunietych jest niepusta

				cacheMonitor->lock();

				cnode = cacheLookup(droppedLastNode);
				if (!cnode) {
					inCache = false;
					cnode = new CachedNode(droppedLastNode);
				}

				Node* node;
				assert(cnode->users == 0);
				cnode->users = 1; //na potrzeby testow
				if ((err = cnode->getNode(node))) {
					return err;
				}
				cnode->users = 0; //na potrzeby testow

				node->nextNode = firstNode;

				cnode->setDirty();

				if (!inCache) {
					//jesli nie bylo go w cache'u to zrzuc na dysk
					if ((err = flush(cnode))) {
						return err;
					}
				}

				cacheMonitor->unlock();
			} else {
				//lista usunietych jest pusta
				droppedNode = firstNode;
			}

			droppedLastNode = lastNode;
		}
		IndexManager::getHandle()->getResolver()->indexDropped(getId());
		return err;
	}
	
	int IndexHandler::search(Constraints* constr, EntrySieve* sieve) {
		return constr->search(tree, sieve);
		
	}
	
	int IndexHandler::changeState(handlerState from, handlerState to) {
		assert(from != to);
		int err = 0, result = 0;
		if ((err = statusMonitor->lock())) {
			return err;
		}
		if (state != from) {
			result = ErrIndexes | ENoIndex;
		} else {
			state = to;
			//jesli ktos czyta indeks to status byl ready i trzeba poczekac az wszyscy skoncza go czytac 
			if (readingTransaction > 0) {
				if ((err = statusMonitor->wait())) {
					return err;
				}
			}
		}
		if ((err = statusMonitor->unlock())) {
			return err;
		}
		return result;
	}
	
	int IndexHandler::userEnter(bool buildingTransaction) {
		if (buildingTransaction) {
			//zadnych restrykcji na transakcje budujaca
			return 0;
		}
		int err = 0, result = 0;
		if ((err = statusMonitor->lock())) {
			return err;
		}
		if (state != READY) {
			result = ErrIndexes | ENoIndex;
		} else {
			readingTransaction++;
		}
		if ((err = statusMonitor->unlock())) {
			return err;
		}
		return result;
	}
	
	int IndexHandler::userLeave(bool buildingTransaction) {
		if (buildingTransaction) {
			//zadnych restrykcji na transakcje budujaca
			return 0;
		}
		int err = 0;
		if ((err = statusMonitor->lock())) {
			return err;
		}
		readingTransaction--;
		//jesli juz nikt nie uzywa indeksu to moze ktos czeka na usuniecie
		if (readingTransaction == 0) {
			if ((err = statusMonitor->signal())) {
				return err;
			}
		}
		err = statusMonitor->unlock();
		return err;
	}
}
