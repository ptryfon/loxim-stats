#include "TransactionImpact.h"

namespace Indexes {

	TransactionImpact::TransactionImpact() {
		finishedBefore = new set<tid_t>();
		next = prev = this;
		horizon = 0;
		//entryCounter = 0;
	}
	
	TransactionImpact::~TransactionImpact() {
		forgetFinishedBefore();
	}
	
	TransactionImpact* TransactionImpact::getNext() {
		return next;
	}
	
	TransactionImpact* TransactionImpact::getPrev() {
		return prev;
	}
	
	void TransactionImpact::addFinished(tid_t id) {
		finishedBefore->insert(id);
		if (id > horizon) {
			horizon = id;
		}
	}
	
	void TransactionImpact::insertAfter(TransactionImpact* tr) {
		
		TransactionImpact* next = this->next;
		
		next->prev = tr;
		this->next = tr;
		
		tr->prev = this;
		tr->next = next;
		
	}
	
	tid_t TransactionImpact::getHorizon() {
		return horizon;
	}
	
	set<tid_t>* TransactionImpact::getFinishedBefore() {
		return finishedBefore;
	}
	
	void TransactionImpact::forgetFinishedBefore() {
		if (finishedBefore) {
			delete finishedBefore;
			finishedBefore = NULL;
		}
	}
	
	void TransactionImpact::remove() {
		next->prev = prev;
		prev->next = next;
		
		prev = next = NULL;
	}
	
}
