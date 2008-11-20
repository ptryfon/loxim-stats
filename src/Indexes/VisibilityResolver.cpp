#include <assert.h>

#include <Indexes/VisibilityResolver.h>

namespace Indexes {

	VisibilityResolver::VisibilityResolver() {
		tail = new TransactionImpact();
		horizon = 0; //bez znaczenia - pierwsza transakcja zakonczona juz w trakcie inicjalizacji to ustawi
		access = new SemaphoreLib::Mutex();
		access->init();
	}
	
	VisibilityResolver::~VisibilityResolver() {
		TransactionImpact* tr;
		while (tail->getNext() != tail) {
			tr = tail->getNext();
			tr->remove();
			delete tr;
		}
		delete tail;
		delete access;
	}
	
	bool VisibilityResolver::isRolledBack(tid_t tid, lid_t index) {
		if (tid == ROLLED_BACK) {
			return true;
		}
		rolledBack_t::const_iterator it;
		it = rolledBack.find(index);
		if (it == rolledBack.end()) {
			return false;
		}
		return (it->second.find(tid) != it->second.end());
	}
	
	bool VisibilityResolver::isBeyondHorizon(tid_t tid) {
		return (tid <= horizon);// && (tid != ROLLED_BACK);
	}
	
	bool VisibilityResolver::isVisible(tid_t from, ts_t begin, lid_t index) {
		if (from == BEYOND_HORIZON) {
			return true;
		}
		bool result;
		access->down();
		//jesli jest w mapie rollbacku to false
		if (isRolledBack(from, index)) {
			result = false;
		} else {
			commitTs_t::iterator it = commitTs.find(from);
			if (it == commitTs.end()) {
				//transakcja from jest albo poza horyzontem albo jest aktywna
				//jesli from <= granica to ok
				result = isBeyondHorizon(from);
			} else {
				ts_t commitTime = it->second;
				result = commitTime < begin;
			}
		}
		access->up();
		return result;
	}
	
	void VisibilityResolver::begin(tid_t id) {
		access->down();
		active.insert(pair<tid_t, TransactionImpact*>(id, tail));
		TransactionImpact* newTail = new TransactionImpact();
		tail->insertAfter(newTail);
		tail = newTail;
		access->up();
	}
	
	void VisibilityResolver::abort(tid_t id) {
		access->down();
		TransactionImpact* tr = prepare2finish(id);
		TransactionImpact::impact_t tmp;
		tr->entryCounter.swap(tmp);
		finish(tr);
		if (!tmp.empty()) {
			//tylko jesli byly jakies zmiany w indeksach, wpp nie ma wplywu na indeksy
			TransactionImpact::impact_t::iterator it;
			for (it = tmp.begin(); it != tmp.end(); it++) {
				rolledBack[it->first].insert(pair<tid_t, unsigned long>(id, it->second));
			}
		}
		access->up();
	}
	
	void VisibilityResolver::commit(tid_t id, ts_t timestamp) {
		access->down();
		TransactionImpact* tr = prepare2finish(id);
		if (!tr->entryCounter.empty()) {
			//tylko jesli byly jakies zmiany w indeksach, wpp nie ma wplywu na indeksy
			commitTs.insert(pair<tid_t, ts_t>(id, timestamp)); //powinno byc przed find bo jesli bedzie za horyzontem to od razu find ja usunie	
		}
		//tr = prepare2finish(id);//mozna rozdzielic jesli wyjmiemy z 
		finish(tr);
		access->up();
	}
	
	TransactionImpact* VisibilityResolver::prepare2finish(tid_t id) {
		active_t::iterator activeIt = active.find(id); 
		TransactionImpact* tr = activeIt->second;
		active.erase(activeIt);
		tail->addFinished(id); //dodanie kolejnej transakcji zakonczonej (tuz przed tail == +inf)
		return tr;
	}
	
	void VisibilityResolver::finish(TransactionImpact* tr) {
		TransactionImpact* nextTr = tr->getNext(); //nastepna aktywna
		//tail->addFinished(id); //dodanie kolejnej transakcji zakonczonej (tuz przed tail == +inf)
		if (tr->getPrev() == tail) {
			//zakonczyla sie pierwsza z aktywnych transakcji
			horizon = nextTr->getHorizon();
			TransactionImpact::finishedBefore_t::iterator it;
			for (it = nextTr->getFinishedBefore()->begin(); it != nextTr->getFinishedBefore()->end(); it++) {
				commitTs.erase(*it); //zapomnij czasy commitu dla wszystkiego przed horyzontem
			}
			//pierwsza aktywna transakcja - nic sie przed nia nie zmieni. nie potrzebujemu juz tych informacji
			nextTr->forgetFinishedBefore();
		}
		tr->remove(); //usuwanie z puli aktywnych
		//active.erase(activeIt);
		delete tr;
	}
	
	void VisibilityResolver::indexDropped(lid_t index) {
		access->down();
		rolledBack.erase(index);
		access->up();
	}
	
	void VisibilityResolver::entryDropped(tid_t tid, lid_t index, TransactionEntryStatus status) {
		access->down();
		TransactionImpact* ti;
		active_t::iterator ait;
		rolledBack_t::iterator rit;
		TransactionImpact::impact_t::iterator iit;
		rolledBackIndex_t::iterator riit;
		switch(status) {
			case ACTIVE_ENTRY:
				ait = active.find(tid);
				assert(ait != active.end());
				ti = ait->second;
				iit = ti->entryCounter.find(index);
				assert(iit != ti->entryCounter.end());
				if (iit->second == 1) {
					ti->entryCounter.erase(iit);
				} else {
					iit->second--;
				}
				break;
			case ROLLED_BACK_ENTRY:
				if (tid == ROLLED_BACK) {
					//ten wpis juz zostal wczesniej wycofany
					break;
				}
				rit = rolledBack.find(index);
				assert(rit != rolledBack.end());
				riit = rit->second.find(tid);
				assert(riit != rit->second.end());
				if (riit->second == 1) {
					rit->second.erase(riit);
					if (rit->second.empty()) {
						rolledBack.erase(rit);
					}
				} else {
					riit->second--;
				}
			default:
				;
		}
		access->up();
	}
	
	void VisibilityResolver::entryAdded(tid_t tid, lid_t index) {
		access->down();
		active_t::iterator it = active.find(tid);
		assert(it != active.end());
		it->second->entryCounter[index]++;
		access->up();
	}
}
