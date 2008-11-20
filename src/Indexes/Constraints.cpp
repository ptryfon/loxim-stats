#include "Constraints.h"
#include "IndexHandler.h"
#include <assert.h>

#include <memory>

namespace Indexes
{

	Constraints::Constraints() {}
	
	Constraints::~Constraints() {}
	
	int Constraints::search(BTree* tree, EntrySieve* sieve) {
		traveler = 	NULL;
		traveler = new LeafTraveller(tree, sieve);
		int err = find(tree);
		delete traveler;
		return err;
		
	};
	
	vector<QueryNode*> Constraints::getSubqueries() {
		return subqueries;
	}
	
	QueryResult* Constraints::convert(QueryNode* value) {
		switch (value->type()) {
			case TreeNode::TNINT:
				return new QueryIntResult(dynamic_cast<IntNode*>(value)->getValue());
			case TreeNode::TNDOUBLE:
				return new QueryDoubleResult(dynamic_cast<DoubleNode*>(value)->getValue());
			case TreeNode::TNSTRING:
				return new QueryStringResult(dynamic_cast<StringNode*>(value)->getValue());
			default:
				return NULL; //innych nie konvertujemy
		}
	}
	
	/**************************************
	 * 	ExactConstraints
	 *************************************/
	
	
	ExactConstraints::ExactConstraints(QParser::QueryNode* value) : value(convert(value)) {
		subqueries.push_back(value);
	}
	
	ExactConstraints::~ExactConstraints() {};
	
	int ExactConstraints::find(BTree* tree) {
		assert(value);
		int err;
		auto_ptr<Comparator> c(tree->getNewComparator());
		
		if ((err = c->setValue(value))) {
			return err;
		}
		CachedNode* node;
		RootEntry* entry;
		if ((err = tree->find(c.get(), node, entry))) {
			return err;
		}
		
		if (entry == NULL) {
			//nic nie znaleziono
			return 0;
		}
		
		if (c->lessThan(entry)) {
			//doszlismy az do elementu wiekszego i nic nie znalezlismy -> nie ma poszukiwanego elementu
			err = tree->ih->freeNode(node);
			return err;
		}
		
		//znaleziono poszukiwany klucz. trzeba znalezc je wszystkie. musi zwalniac po sobie wszystkie wezly
		
		traveler->setPosition(node, entry)->setGrab(true);
		err = traveler->travelToGreater();
		
		return err;
	}
	
	void ExactConstraints::setSubresults(vector<QueryResult*> subresults) {
		assert(subresults.size() == 1);
		value = subresults[0];
	}
	
	string ExactConstraints::toString() {
		return "=" + subqueries[0]->deParse();
	}
	
	/**************************************
	 * 	OneSideBoundedConstraints
	 *************************************/
	 
	OneSideBoundedConstraints::OneSideBoundedConstraints(bool inclusive, QueryNode *value) : inclusive(inclusive), value(convert(value)) {
		subqueries.push_back(value);
	};
	
	OneSideBoundedConstraints::~OneSideBoundedConstraints(){};
	
	void OneSideBoundedConstraints::setSubresults(vector<QueryResult*> subresults) {
		assert(subresults.size() == 1);
		value = subresults[0];
	}
	
	/**************************************
	 * 	LeftBoundedConstraints
	 *************************************/
	 
	LeftBoundedConstraints::LeftBoundedConstraints(bool leftInclusive, QueryNode* leftValue)
		: OneSideBoundedConstraints(leftInclusive, leftValue){};
	
	LeftBoundedConstraints::~LeftBoundedConstraints(){};
	
	int LeftBoundedConstraints::find(BTree* tree) {
		assert(value);
		int err;
		auto_ptr<Comparator> c(tree->getNewComparator());
		if ((err = c->setValue(value))) {
			return err;
		}
		CachedNode* node;
		RootEntry* entry;
		if ((err = tree->find(c.get(), node, entry))) {
			return err;
		}
		
		// tylko przy zalozeniu ze nie szukamy w nieostatnim wezle wiekszego
		if (entry == NULL) {
			return 0;
		}
		
		// czy znalezlismy dokladnie poszukiwany element i mamy go pominac
		bool stepOver = (!inclusive) && (entry != NULL) && (c->equal(entry));
 
 		traveler->setPosition(node, entry);
 
		if (stepOver) {
			traveler->setGrab(false);
			traveler->travelToGreater();
		}
		traveler->setGrab(true);
		traveler->travelToEnd();
		
		return 0;
	}
	
	string LeftBoundedConstraints::toString() {
		return (inclusive ? "<" : "(") + subqueries[0]->deParse() + " to +inf)";
	}
	
	/**************************************
	 * 	RightBoundedConstraints
	 *************************************/
	 
	RightBoundedConstraints::RightBoundedConstraints(QueryNode* rightValue, bool rightInclusive)
		: OneSideBoundedConstraints(rightInclusive, rightValue) {};
		
	RightBoundedConstraints::~RightBoundedConstraints() {}
	
	int RightBoundedConstraints::find(BTree* tree) {
		assert(value);
		auto_ptr<Comparator> 
			c( new MinComparator()),
			destination(tree->getNewComparator());
		CachedNode* node;
		RootEntry* entry;
		int err;
		if ((err = destination->setValue(value))) {	
			return err;
		}
		
		traveler->setDestination(destination.get());
		
		if ((err = tree->find(c.get(), node, entry))) {
			return err;
		}
		
		// tylko przy zalozeniu ze nie szukamy w nieostatnim wezle wiekszego
		if (entry == NULL) {
			//bo drzewo moze byc puste
			return 0;
		}
		
		traveler->setPosition(node, entry)->setGrab(true);
		
		if ((err = traveler->travelToDestination())) {
			return err;
		}
		
		//entry jest najmniejsza wartoscia w drzewie
		if (inclusive && !traveler->isDestinationExceeded()) {
			traveler->travelToGreater();
		}
		
		return 0;
	}
	
	string RightBoundedConstraints::toString() {
		return "(-inf to " + subqueries[0]->deParse() + (inclusive ? ">" : ")");
	}
	
	/**************************************
	 * 	TwoSideConstraints
	 *************************************/
	TwoSideConstraints::TwoSideConstraints(bool inclusiveLeft, QParser::QueryNode* leftValue, QParser::QueryNode* rightValue, bool inclusiveRight)
		: leftValue(convert(leftValue)), rightValue(convert(rightValue)), inclusiveLeft(inclusiveLeft), inclusiveRight(inclusiveRight) {
		subqueries.push_back(leftValue); //wazna kolejnosc
		subqueries.push_back(rightValue);
	};
	
	TwoSideConstraints::~TwoSideConstraints(){}
	
	int TwoSideConstraints::find(BTree* tree) {
		assert(leftValue);
		assert(rightValue);
		int err;
		auto_ptr<Comparator>
			c(tree->getNewComparator()),
			destination(tree->getNewComparator());
		
		if ((err = c->setValue(leftValue))) {
			return err;
		}
			
		if ((err = destination->setValue(rightValue))) {	
			return err;
		}	
		
		traveler->setDestination(destination.get());
		CachedNode* node;
		RootEntry* entry;
		if ((err = tree->find(c.get(), node, entry))) {
			return err;
		}
		// tylko przy zalozeniu ze nie szukamy w nieostatnim wezle wiekszego
		if (entry == NULL) {
			return 0;
		}
		
		traveler->setPosition(node, entry);
		bool equal = c->equal(entry);
		
		if (equal && !inclusiveLeft) {
				traveler->setGrab(false);
				traveler->travelToGreater();
		}
		traveler->setGrab(true);
		traveler->travelToDestination();
		
		if (inclusiveRight && !traveler->isDestinationExceeded()) {
			traveler->travelToGreater();
		}
		return 0;
	}
	
	void TwoSideConstraints::setSubresults(vector<QueryResult*> subresults) {
		assert(subresults.size() == 2);
		leftValue = subresults[0]; //wazna kolejnosc
		rightValue = subresults[1];
	}
	
	string TwoSideConstraints::toString() {
		return (inclusiveLeft ? "<" : "(") + subqueries[0]->deParse() + " to " + subqueries[1]->deParse() + (inclusiveRight ? ">" : ")");
	}
}
