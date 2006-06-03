#include <stdio.h>
#include <string>
#include <vector>

#include "QueryResult.h"
#include "TransactionManager/Transaction.h"
#include "Store/Store.h"
#include "Store/DBDataValue.h"
#include "Store/DBLogicalID.h"
#include "QueryParser/QueryParser.h"
#include "QueryParser/TreeNode.h"
#include "QueryExecutor.h"
#include "Errors/Errors.h"
#include "Errors/ErrorConsole.h"

using namespace QParser;
using namespace TManager;
using namespace Errors;
using namespace Store;
using namespace std;

namespace QExecutor {

EnvironmentStack::EnvironmentStack() { sectionDBnumber = 0; ec = new ErrorConsole("QueryExecutor"); }
EnvironmentStack::~EnvironmentStack() { this->deleteAll(); if (ec != NULL) delete ec; }

int EnvironmentStack::push(QueryBagResult *r) {
	es.push_back(r);
	*ec << "[QE] Environment Stack pushed";
	return 0;
}

int EnvironmentStack::pop(){ 
	if (es.empty()) { 
		*ec << (ErrQExecutor | EQEmptySet);
		return ErrQExecutor | EQEmptySet;
	}
	if (es.size() == sectionDBnumber) {
		*ec << "error: trying to pop() Data Base section"; 
		*ec << (ErrQExecutor | EQEUnexpectedErr);
		return ErrQExecutor | EQEUnexpectedErr;
	}
	// delete ??
	es.pop_back();
	*ec << "[QE] Environment Stack popped"; 
	return 0;
}

int EnvironmentStack::pushDBsection() {
	QueryResult *r = new QueryBagResult();
	es.push_back((QueryBagResult *)r);
	sectionDBnumber = es.size();
	*ec << "[QE] Data Base section pushed on Environment Stack";
	return 0;
}

int EnvironmentStack::popDBsection() {
	if (es.empty()) { 
		*ec << (ErrQExecutor | EQEmptySet);
		return ErrQExecutor | EQEmptySet;
	}
	if (es.size() != sectionDBnumber) {
		*ec << "error: trying to popDBsection() on not Data Base section"; 
		*ec << (ErrQExecutor | EQEUnexpectedErr);
		return ErrQExecutor | EQEUnexpectedErr;
	}
	// delete ??
	es.pop_back();
	sectionDBnumber = 0;
	*ec << "[QE] Data Base section popped from Environment Stack"; 
	return 0;
}

int EnvironmentStack::top(QueryBagResult *&r) {
	if (es.empty()) {
		*ec << (ErrQExecutor | EQEmptySet);
		return ErrQExecutor | EQEmptySet;
	}
	if (es.size() == sectionDBnumber) {
		*ec << "error: trying to top() Data Base section"; 
		*ec << (ErrQExecutor | EQEUnexpectedErr);
		return ErrQExecutor | EQEUnexpectedErr;
	}
	r=(es.back());
	return 0;
}

bool EnvironmentStack::empty() { return es.empty(); }
int EnvironmentStack::size() { return es.size(); }

int EnvironmentStack::bindName(string name, int sectionNo, Transaction *&tr, QueryResult *&r) {
	int errcode;
	*ec << "[QE] Name binding on ES";
	unsigned int number = (es.size());
	ec->printf("[QE] bindName: ES got %u sections\n", number);
	r = new QueryBagResult();
	bool found_one = false;
	QueryBagResult *section;
	unsigned int sectionSize;
	QueryResult *sth;
	string current;
	for (unsigned int i = number; i >= 1; i--) {
		if (i == sectionDBnumber) {
			ec->printf("[QE] bindName: ES section %u is Data Base section\n", i);
			vector<LogicalID*>* vec;
			if ((errcode = tr->getRootsLID(name, vec)) != 0) {
				tr->abort();
				tr = NULL;
				return errcode;
			}
			int vecSize = vec->size();
			ec->printf("[QE] %d Roots LID by name taken\n", vecSize);
			for (int i = 0; i < vecSize; i++ ) {
				found_one = true;
				LogicalID *lid = vec->at(i);
				*ec << "[QE] LogicalID received";
				QueryReferenceResult *lidres = new QueryReferenceResult(lid);
				r->addResult(lidres);
			}
			delete vec;
		}
		else {
			section = (es.at(i - 1));
			sectionSize = (section->size());
			ec->printf("[QE] bindName: ES section %u got %u elements\n", i, sectionSize);
			for (unsigned int j = 0; j < sectionSize; j++) {
				errcode = (section->at(j,sth));
				if (errcode != 0) return errcode;
				if ((sth->type()) == (QueryResult::QBINDER)) {
					current = (((QueryBinderResult *) sth)->getName());
					ec->printf("[QE] bindName: current %u name is: %s\n", j, current.c_str());
					if (current == name) {
						found_one = true;
						*ec << "[QE] bindName: Object added to Result";
						r->addResult(((QueryBinderResult *) sth)->getItem());
					}
				}
			}
		}
		if (found_one) {
			ec->printf("[QE] bindName: bind at %d section, Parser said it should be binded at %d\n", i, sectionNo);
			return 0;
		}
	}
	ec->printf("[QE] bindName: name not binded on ES, Parser said it should be binded at %d\n", sectionNo);
	return 0;
}

void EnvironmentStack::deleteAll() {
	for (unsigned int i = 0; i < (es.size()); i++) {
		delete es.at(i);
	};
}





// nested function returns bag of binders, which will be pushed on the environment stack

int QuerySequenceResult::nested(Transaction *&tr, QueryResult *&r) {
	*ec << "[QE] nested(): ERROR! QuerySequenceResult shouldn't be nested";
	*ec << (ErrQExecutor | EOtherResExp);
	return ErrQExecutor | EOtherResExp; 
	// nested () function is applied to rows of a QueryResult and so, it shouldn't be applied to sequences and bags
}

int QueryBagResult::nested(Transaction *&tr, QueryResult *&r) {
	*ec << "[QE] nested(): ERROR! QueryBagResult shouldn't be nested";
	*ec << (ErrQExecutor | EOtherResExp);
	return ErrQExecutor | EOtherResExp; 
	// nested () function is applied to rows of a QueryResult and so, it shouldn't be applied to sequences and bags
}

int QueryStructResult::nested(Transaction *&tr, QueryResult *&r) {
	*ec << "[QE] nested(): QueryStructResult";
	int errcode;
	for (unsigned int i = 0; i < str.size(); i++) {
		if ((str.at(i))->type() == QueryResult::QSTRUCT) {
			*ec << (ErrQExecutor | EOtherResExp);
			return ErrQExecutor | EOtherResExp; // one row shouldn't contain another row;
		}
		else {
			errcode = ((str.at(i))->nested(tr, r));
			if (errcode != 0) return errcode;
		}
	}
	return 0;
}

int QueryStringResult::nested(Transaction *&tr, QueryResult *&r) {
	*ec << "[QE] nested(): QueryStringResult can't be nested";
	return 0;
}

int QueryIntResult::nested(Transaction *&tr, QueryResult *&r) {
	*ec << "[QE] nested(): QueryIntResult can't be nested";
	return 0;
}

int QueryDoubleResult::nested(Transaction *&tr, QueryResult *&r) {
	*ec << "[QE] nested(): QueryDoubleResult can't be nested";
	return 0;
}

int QueryBoolResult::nested(Transaction *&tr, QueryResult *&r) {
	*ec << "[QE] nested(): QueryBoolResult can't be nested";
	return 0;
}

int QueryNothingResult::nested(Transaction *&tr, QueryResult *&r) {
	*ec << "[QE] nested(): QueryNothingResult can't be nested";
	return 0;
}

int QueryBinderResult::nested(Transaction *&tr, QueryResult *&r) {
	if (item != NULL) {
		QueryBinderResult *tmp_value = new QueryBinderResult(name,item);
		ec->printf("[QE] nested(): QueryBinderResult copy returned name: %s\n", name.c_str());
		r->addResult(tmp_value);
	}
	return 0;
}

int QueryReferenceResult::nested(Transaction *&tr, QueryResult *&r) {
	DataValue* tmp_data_value;
	int errcode;
	ObjectPointer *optr;
	if (value != NULL) {
		if ((errcode = tr->getObjectPointer(value, Store::Read, optr)) != 0) {
			*ec << "[QE] Error in getObjectPointer";
			tr->abort();
			tr = NULL;
			return errcode;
		}
		tmp_data_value = optr->getValue();
		int vType = tmp_data_value->getType();
		switch (vType) {
			case Store::Integer: {
				*ec << "[QE] nested(): QueryReferenceResult pointing integer value - can't be nested";
				break;
			}
			case Store::Double: {
				*ec << "[QE] nested(): QueryReferenceResult pointing double value - can't be nested";
				break;
			}
			case Store::String: {
				*ec << "[QE] nested(): QueryReferenceResult pointing string value - can't be nested";
				break;
			}
			case Store::Pointer: {
				LogicalID *tmp_logID = (tmp_data_value->getPointer());
				if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, optr)) != 0) {
				*ec << "[QE] Error in getObjectPointer";
					tr->abort();
					tr = NULL;
					return errcode;
				}
				string tmp_name = optr->getName();
				QueryReferenceResult *final_ref = new QueryReferenceResult(tmp_logID);
				QueryBinderResult *final_binder = new QueryBinderResult(tmp_name, final_ref);
				*ec << "[QE] nested(): QueryReferenceResult pointing reference value";
				r->addResult(final_binder);
				ec->printf("[QE] nested(): new QueryBinderResult returned name: %s\n", tmp_name.c_str());
				break;
			}
			case Store::Vector: {
				vector<LogicalID*>* tmp_vec = (tmp_data_value->getVector());
				*ec << "[QE] nested(): QueryReferenceResult pointing vector value";
				int vec_size = tmp_vec->size();
				for (int i = 0; i < vec_size; i++ ) {
					LogicalID *tmp_logID = tmp_vec->at(i);
					if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, optr)) != 0) {
					*ec << "[QE] Error in getObjectPointer";
						tr->abort();
						tr = NULL;
						return errcode;
					}
					string tmp_name = optr->getName();
					QueryReferenceResult *final_ref = new QueryReferenceResult(tmp_logID);
					QueryBinderResult *final_binder = new QueryBinderResult(tmp_name, final_ref);
					ec->printf("[QE] nested(): vector element number %d\n", i);
					r->addResult(final_binder);
					ec->printf("[QE] nested(): new QueryBinderResult returned name: %s\n", tmp_name.c_str());
				}
				break;
			}
			default : {
				*ec << "[QE] nested(): ERROR! QueryReferenceResult pointing unknown format value";
				*ec << (ErrQExecutor | EUnknownValue);
				return ErrQExecutor | EUnknownValue;
				break;
			}
		}
	}
	return 0;
}

}
