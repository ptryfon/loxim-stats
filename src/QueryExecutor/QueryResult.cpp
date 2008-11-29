#include <stdio.h>
#include <string>
#include <vector>
   
#include <QueryExecutor/QueryResult.h>
#include <TransactionManager/Transaction.h>
#include <Store/Store.h>
#include <Store/DBDataValue.h>
#include <Store/DBLogicalID.h>
#include <QueryParser/QueryParser.h>
#include <QueryParser/TreeNode.h>
#include <Errors/Errors.h>
#include <Errors/ErrorConsole.h>

using namespace QParser;
using namespace TManager;
using namespace Errors;
using namespace Store;
using namespace std;

namespace QExecutor {

//constructors
QuerySequenceResult::QuerySequenceResult() 			{ ec = new ErrorConsole("QueryExecutor"); }
QuerySequenceResult::QuerySequenceResult(vector<QueryResult*> s){ seq = s; ec = new ErrorConsole("QueryExecutor"); }
QueryBagResult::QueryBagResult() 				{ ec = new ErrorConsole("QueryExecutor"); }
QueryBagResult::QueryBagResult(vector<QueryResult*> b)		{ bag = b; ec = new ErrorConsole("QueryExecutor");}
QueryStructResult::QueryStructResult() 				{ ec = new ErrorConsole("QueryExecutor"); }
QueryStructResult::QueryStructResult(vector<QueryResult*> s)	{ str = s; ec = new ErrorConsole("QueryExecutor"); }
QueryStructResult::QueryStructResult(QueryResult *elem, QueryResult *value) { 
	str.push_back(elem); str.push_back(value); ec = new ErrorConsole("QueryExecutor"); }
QueryBinderResult::QueryBinderResult()				{ ec = new ErrorConsole("QueryExecutor"); }
QueryBinderResult::QueryBinderResult(string n, QueryResult* r) 	{ name=n; item=r; ec = new ErrorConsole("QueryExecutor"); }
QueryStringResult::QueryStringResult()				{ ec = new ErrorConsole("QueryExecutor"); }
QueryStringResult::QueryStringResult(string v) 			{ value=v; ec = new ErrorConsole("QueryExecutor"); }
QueryIntResult::QueryIntResult()				{ ec = new ErrorConsole("QueryExecutor"); }
QueryIntResult::QueryIntResult(int v) 				{ value=v; ec = new ErrorConsole("QueryExecutor"); }
QueryDoubleResult::QueryDoubleResult()				{ ec = new ErrorConsole("QueryExecutor"); }
QueryDoubleResult::QueryDoubleResult(double v) 			{ value=v; ec = new ErrorConsole("QueryExecutor"); }
QueryBoolResult::QueryBoolResult()				{ ec = new ErrorConsole("QueryExecutor"); }
QueryBoolResult::QueryBoolResult(bool v) 			{ value=v; ec = new ErrorConsole("QueryExecutor"); }
QueryReferenceResult::QueryReferenceResult()			{ ec = new ErrorConsole("QueryExecutor"); refed = false; }
QueryReferenceResult::QueryReferenceResult(LogicalID* v) 	{ value=v; m_key.setEmpty(); ec = new ErrorConsole("QueryExecutor"); refed = false; }
QueryReferenceResult::QueryReferenceResult(LogicalID* v, InterfaceKey k) 	{ value=v; m_key=k; ec = new ErrorConsole("QueryExecutor"); refed = false; }
QueryNothingResult::QueryNothingResult() 			{ ec = new ErrorConsole("QueryExecutor"); }
QueryVirtualResult::QueryVirtualResult() 			{ ec = new ErrorConsole("QueryExecutor"); refed = false;}
QueryVirtualResult::QueryVirtualResult(string _vo_name, vector<LogicalID *> _view_defs, vector<QueryResult *> _seeds, LogicalID *_view_parent) {
vo_name = _vo_name; view_defs = _view_defs; seeds = _seeds; ec = new ErrorConsole("QueryExecutor"); refed = false; view_parent = _view_parent;}

//function clone()
QueryResult* QuerySequenceResult::clone()	{ return new QuerySequenceResult(seq); }
QueryResult* QueryBagResult::clone()		{ return new QueryBagResult(bag); }
QueryResult* QueryStructResult::clone()		{ return new QueryStructResult(str); }
QueryResult* QueryBinderResult::clone()		{ return new QueryBinderResult(name, (QueryResult*)item->clone()); }
QueryResult* QueryStringResult::clone()		{ return new QueryStringResult(value); }
QueryResult* QueryIntResult::clone()		{ return new QueryIntResult(value); }
QueryResult* QueryDoubleResult::clone()		{ return new QueryDoubleResult(value); }
QueryResult* QueryBoolResult::clone()		{ return new QueryBoolResult(value); }
QueryResult* QueryReferenceResult::clone()	{ return new QueryReferenceResult(value, m_key); }
QueryResult* QueryNothingResult::clone()	{ return new QueryNothingResult(); }
QueryResult* QueryVirtualResult::clone()	{ return new QueryVirtualResult(vo_name, view_defs, seeds, view_parent); }

//function type() - returns type of a result
int QueryResult::type() const			{ return QueryResult::QRESULT; }
int QuerySequenceResult::type() const		{ return QueryResult::QSEQUENCE; }
int QueryBagResult::type() const		{ return QueryResult::QBAG; }
int QueryStructResult::type() const		{ return QueryResult::QSTRUCT; }
int QueryBinderResult::type() const		{ return QueryResult::QBINDER; }
int QueryStringResult::type() const		{ return QueryResult::QSTRING; }
int QueryIntResult::type() const		{ return QueryResult::QINT; }
int QueryDoubleResult::type() const		{ return QueryResult::QDOUBLE; }
int QueryBoolResult::type() const		{ return QueryResult::QBOOL; }
int QueryReferenceResult::type() const		{ return QueryResult::QREFERENCE; }
int QueryNothingResult::type() const		{ return QueryResult::QNOTHING; }
int QueryVirtualResult::type() const		{ return QueryResult::QVIRTUAL; }

//function collection() - returns true if result is a collection, false if not
bool QuerySequenceResult::collection()	{ return true; }
bool QueryBagResult::collection()	{ return true; }
bool QueryStructResult::collection()	{ return true; }
bool QueryBinderResult::collection()	{ return false; }
bool QueryStringResult::collection()	{ return false; }
bool QueryIntResult::collection()	{ return false; }
bool QueryDoubleResult::collection()	{ return false; }
bool QueryBoolResult::collection()	{ return false; }
bool QueryReferenceResult::collection()	{ return false; }
bool QueryNothingResult::collection()	{ return false; }
bool QueryVirtualResult::collection()	{ return false; }

//function isEmpty() - returns false if a result contains some other results, true if does not
bool QuerySequenceResult::isEmpty()	{ return seq.empty(); }
bool QueryBagResult::isEmpty()		{ return bag.empty(); }
bool QueryStructResult::isEmpty()	{ return str.empty(); }
bool QueryBinderResult::isEmpty()	{ if (item != NULL) {return false;} else {return true;}; }
bool QueryStringResult::isEmpty()	{ return true; }
bool QueryIntResult::isEmpty()		{ return true; }
bool QueryDoubleResult::isEmpty()	{ return true; }
bool QueryBoolResult::isEmpty()		{ return true; }
bool QueryReferenceResult::isEmpty()	{ return true; }
bool QueryNothingResult::isEmpty()	{ return true; }
bool QueryVirtualResult::isEmpty()	{ return true; }

//function size() - returns how many other results does a result contain
unsigned int QuerySequenceResult::size() const	{ return seq.size(); }
unsigned int QueryBagResult::size() const	{ return bag.size(); }
unsigned int QueryStructResult::size() const	{ return str.size(); }
unsigned int QueryBinderResult::size() const	{ if (item != NULL) {return 1;} else {return 0;}; }
unsigned int QueryStringResult::size() const	{ return 0; }
unsigned int QueryIntResult::size() const	{ return 0; }
unsigned int QueryDoubleResult::size() const	{ return 0; }
unsigned int QueryBoolResult::size() const	{ return 0; }
unsigned int QueryReferenceResult::size() const	{ return 0; }
unsigned int QueryNothingResult::size() const	{ return 0; }
unsigned int QueryVirtualResult::size()	const	{ return 0; }

//accesors for collection result classes - sequence, bag, struct
void QuerySequenceResult::addResult(QueryResult *r){ 
	if (((r->type()) == (QueryResult::QSEQUENCE)) || ((r->type()) == (QueryResult::QBAG))) {
		unsigned int seq_size = (r->size());
		for (unsigned int i = 0; i < seq_size; i++) {
			int errcode;
			QueryResult *tmp_res;
			errcode = (r->getResult(tmp_res));
			if (not (tmp_res->isNothing()))
				seq.push_back(tmp_res);
		}
	}
	else {
		if (not (r->isNothing()))
			seq.push_back(r);
	}
	//this->orderBy() 
}

int QuerySequenceResult::getResult(QueryResult *&r){ 
	if (seq.empty()) { 
		*ec << (ErrQExecutor | EQEmptySet);
		return (ErrQExecutor | EQEmptySet); 
	} 
	else { 
		r=(seq.front());
		it=(seq.begin()); 
		seq.erase(it);
	}; 
	return 0;
}

int QuerySequenceResult::at(unsigned int i, QueryResult *&r){ 
	if ( i < 0 || i >= seq.size() ) {
		*ec << (ErrQExecutor | EQEmptySet);
		return (ErrQExecutor | EQEmptySet); 
	};
	r=(seq.at(i));
	return 0;
}

int QuerySequenceResult::lastElem(QueryResult *&r){ 
	int s = seq.size();
	if ( s == 0 ) {
		*ec << (ErrQExecutor | EQEmptySet);
		return (ErrQExecutor | EQEmptySet); 
	}
	r=(seq.at(s - 1));
	return 0;
}

vector<QueryResult*> QuerySequenceResult::getVector() {
	return seq;
}

void QueryBagResult::addResult(QueryResult *r){
	if (((r->type()) == (QueryResult::QSEQUENCE)) || ((r->type()) == (QueryResult::QBAG))) {
		if (this->size() == 0) {
			if (r->type() == QueryResult::QBAG)
				bag = ((QueryBagResult *) r)->getVector();
			else
				bag = ((QuerySequenceResult *) r)->getVector();
		}
		else {
			unsigned int bag_size = (r->size());
			for (unsigned int i = 0; i < bag_size; i++) {
				int errcode;
				QueryResult *tmp_res;
				errcode = (r->getResult(tmp_res));
				if (not (tmp_res->isNothing()))
					bag.push_back(tmp_res);
			}
		}
	}
	else
		if (not (r->isNothing()))
			bag.push_back(r); 
}
int QueryBagResult::getResult(QueryResult *&r){
	if (bag.empty()) { 
		*ec << (ErrQExecutor | EQEmptySet);
		return (ErrQExecutor | EQEmptySet); 
	}
	else {
		r=(bag.front());
		it=(bag.begin());
		bag.erase(it);
	};
	return 0;
}

int QueryBagResult::at(unsigned int i, QueryResult *&r){ 
	if ( i < 0 || i >= bag.size() ) {
		*ec << (ErrQExecutor | EQEmptySet);
		return (ErrQExecutor | EQEmptySet); 
	};
	r=(bag.at(i));
	return 0;
}

int QueryBagResult::lastElem(QueryResult *&r){ 
	int s = bag.size();
	if ( s == 0 ) {
		*ec << (ErrQExecutor | EQEmptySet);
		return (ErrQExecutor | EQEmptySet); 
	}
	r=(bag.at(s - 1));
	return 0;
}

vector<QueryResult*> QueryBagResult::getVector() {
	return bag;
}

void QueryStructResult::addResult(QueryResult *r){ 
if ((r->type()) == (QueryResult::QSTRUCT)) {
		unsigned int str_size = (r->size());
		for (unsigned int i = 0; i < str_size; i++) {
			int errcode;
			QueryResult *tmp_res;
			errcode = (((QueryStructResult*)r)->at(i, tmp_res));
			if (not (tmp_res->isNothing()))
				str.push_back(tmp_res);
		}
	}
	else
		if (not (r->isNothing()))
			str.push_back(r); 
}

int QueryStructResult::getResult(QueryResult *&r){ 
	if (str.empty()) { 
		*ec << (ErrQExecutor | EQEmptySet);
		return (ErrQExecutor | EQEmptySet);  
	} 
	else { 
		r=(str.front()); 
		it=(str.begin()); 
		str.erase(it);
	}; 
	return 0;
}

int QueryStructResult::at(unsigned int i, QueryResult *&r){ 
	if ( i < 0 || i >= str.size() ) {
		*ec << (ErrQExecutor | EQEmptySet);
		return (ErrQExecutor | EQEmptySet); 
	};
	r=(str.at(i));
	return 0;
}

int QueryStructResult::lastElem(QueryResult *&r){ 
	int s = str.size();
	if ( s == 0 ) {
		*ec << (ErrQExecutor | EQEmptySet);
		return (ErrQExecutor | EQEmptySet); 
	}
	r=(str.at(s - 1));
	return 0;
}

//accesors for single value type result classes - binder, string, int, double, bool, reference
QueryResult* QueryBinderResult::getItem()		{ return item; }
void QueryBinderResult::setItem(QueryResult *r)		{ item = r; }
string QueryBinderResult::getName()			{ return name; }
void QueryBinderResult::setName(string n)		{ name = n; }

string QueryStringResult::getValue()			{ return value; }
void QueryStringResult::setValue(string v)		{ value = v; }

int QueryIntResult::getValue()				{ return value; }
void QueryIntResult::setValue(int v)			{ value = v; }

double QueryDoubleResult::getValue()			{ return value; }
void QueryDoubleResult::setValue(double v)		{ value = v; }

bool QueryBoolResult::getValue()			{ return value;}
void QueryBoolResult::setValue(bool v)			{ value = v; }

LogicalID* QueryReferenceResult::getValue()		{ return value; }
void QueryReferenceResult::setValue(LogicalID* v)	{ value = v; }
bool QueryReferenceResult::wasRefed()			{ return refed; }
void QueryReferenceResult::setRef()			{ refed = true; }
InterfaceKey QueryReferenceResult::getInterfaceKey() const {return m_key;}

//algebraic operations plus, minus, times, divide_by, and, or.

//function plus()
int QueryIntResult::plus(QueryResult *r, QueryResult *&res){ 
	if ((r->type() != QueryResult::QINT) && (r->type() != QueryResult::QDOUBLE)) {
		*ec << "[QE] ERROR! + arguments must be INT or DOUBLE";
		*ec << (ErrQExecutor | ENumberExpected);
		return (ErrQExecutor | ENumberExpected);
	}
	if (r->type() == QueryResult::QINT) {
		int tmp_value = value + ((QueryIntResult*) r)->getValue();
		res = new QueryIntResult(tmp_value);
	}
	else { //QDOUBLE
		double tmp_value = ((double)value) + ((QueryDoubleResult*) r)->getValue();
		res = new QueryDoubleResult(tmp_value);
	}
	return 0; 
}

int QueryDoubleResult::plus(QueryResult *r, QueryResult *&res){
	if ((r->type() != QueryResult::QINT) && (r->type() != QueryResult::QDOUBLE)) {
		*ec << "[QE] ERROR! + arguments must be INT or DOUBLE";
		*ec << (ErrQExecutor | ENumberExpected);
		return (ErrQExecutor | ENumberExpected); 
	}
	if (r->type() == QueryResult::QINT) {
		double tmp_value = value + (double)(((QueryIntResult*) r)->getValue());
		res = new QueryDoubleResult(tmp_value);
	}
	else { //QDOUBLE
		double tmp_value = value + ((QueryDoubleResult*) r)->getValue();
		res = new QueryDoubleResult(tmp_value);
	}
	return 0; 
}

//function minus()
int QueryIntResult::minus(QueryResult *r, QueryResult *&res){
	if ((r->type() != QueryResult::QINT) && (r->type() != QueryResult::QDOUBLE)) {
		*ec << "[QE] ERROR! - arguments must be INT or DOUBLE";
		*ec << (ErrQExecutor | ENumberExpected);
		return (ErrQExecutor | ENumberExpected); 
	} 
	if (r->type() == QueryResult::QINT) {
		int tmp_value = value - ((QueryIntResult*) r)->getValue();
		res = new QueryIntResult(tmp_value);
	}
	else { //QDOUBLE
		double tmp_value = ((double)value) - ((QueryDoubleResult*) r)->getValue();
		res = new QueryDoubleResult(tmp_value);
	}
	return 0; 
}

int QueryDoubleResult::minus(QueryResult *r, QueryResult *&res){ 
	if ((r->type() != QueryResult::QINT) && (r->type() != QueryResult::QDOUBLE)) {
		*ec << "[QE] ERROR! - arguments must be INT or DOUBLE";
		*ec << (ErrQExecutor | ENumberExpected);
		return (ErrQExecutor | ENumberExpected);
	}
	if (r->type() == QueryResult::QINT) {
		double tmp_value = value - (double)(((QueryIntResult*) r)->getValue());
		res = new QueryDoubleResult(tmp_value);
	}
	else { //QDOUBLE
		double tmp_value = value - ((QueryDoubleResult*) r)->getValue();
		res = new QueryDoubleResult(tmp_value);
	}
	return 0; 
}

int QueryIntResult::minus(QueryResult *&res){ 
	int tmp_value = (- value);
	res = new QueryIntResult(tmp_value);
	return 0; 
}

int QueryDoubleResult::minus(QueryResult *&res){ 
	double tmp_value = (- value);
	res = new QueryDoubleResult(tmp_value);
	return 0; 
}

//function times()
int QueryIntResult::times(QueryResult *r, QueryResult *&res){ 
	if ((r->type() != QueryResult::QINT) && (r->type() != QueryResult::QDOUBLE)) {
		*ec << "[QE] ERROR! * arguments must be INT or DOUBLE";
		*ec << (ErrQExecutor | ENumberExpected);
		return (ErrQExecutor | ENumberExpected);
	}
	if (r->type() == QueryResult::QINT) {
		int tmp_value = value * ((QueryIntResult*) r)->getValue();
		res = new QueryIntResult(tmp_value);
	}
	else { //QDOUBLE
		double tmp_value = ((double)value) * ((QueryDoubleResult*) r)->getValue();
		res = new QueryDoubleResult(tmp_value);
	}
	return 0;
}

int QueryDoubleResult::times(QueryResult *r, QueryResult *&res){ 
	if ((r->type() != QueryResult::QINT) && (r->type() != QueryResult::QDOUBLE)) {
		*ec << "[QE] ERROR! * arguments must be INT or DOUBLE";
		*ec << (ErrQExecutor | ENumberExpected);
		return (ErrQExecutor | ENumberExpected);
	}
	if (r->type() == QueryResult::QINT) {
		double tmp_value = value * (double)(((QueryIntResult*) r)->getValue());
		res = new QueryDoubleResult(tmp_value);
	}
	else { //QDOUBLE
		double tmp_value = value * ((QueryDoubleResult*) r)->getValue();
		res = new QueryDoubleResult(tmp_value);
	}
	return 0; 
}

//function divide_by()
int QueryIntResult::divide_by(QueryResult *r, QueryResult *&res){
	if ((r->type() != QueryResult::QINT) && (r->type() != QueryResult::QDOUBLE)) {
		*ec << "[QE] ERROR! / arguments must be INT or DOUBLE";
		*ec << (ErrQExecutor | ENumberExpected);
		return (ErrQExecutor | ENumberExpected);
	}
	if (r->type() == QueryResult::QINT) {
		if (((QueryIntResult*) r)->getValue() == 0) {
			*ec << "[QE] Division by 0 error!";
			*ec << (ErrQExecutor | EDivBy0);
			return (ErrQExecutor | EDivBy0);
		}
		double tmp_value = (double)value / (double)(((QueryIntResult*) r)->getValue());
		res = new QueryDoubleResult(tmp_value);
	}
	else { //QDOUBLE
		if (((QueryDoubleResult*) r)->getValue() == 0) {
			*ec << "[QE] Division by 0 error!";
			*ec << (ErrQExecutor | EDivBy0);
			return (ErrQExecutor | EDivBy0);
		}
		double tmp_value = (double)value / ((QueryDoubleResult*) r)->getValue();
		res = new QueryDoubleResult(tmp_value);
	}
	return 0; 
}

int QueryDoubleResult::divide_by(QueryResult *r, QueryResult *&res){ 
	if ((r->type() != QueryResult::QINT) && (r->type() != QueryResult::QDOUBLE)) {
		*ec << "[QE] ERROR! / arguments must be INT or DOUBLE";
		*ec << (ErrQExecutor | ENumberExpected);
		return (ErrQExecutor | ENumberExpected);
	}
	if (r->type() == QueryResult::QINT) {
		if (((QueryIntResult*) r)->getValue() == 0) {
			*ec << "[QE] Division by 0 error!";
			*ec << (ErrQExecutor | EDivBy0);
			return (ErrQExecutor | EDivBy0);
		}
		double tmp_value = value / (double)(((QueryIntResult*) r)->getValue());
		res = new QueryDoubleResult(tmp_value);
	}
	else { //QDOUBLE
		if (((QueryDoubleResult*) r)->getValue() == 0) {
			*ec << "[QE] Division by 0 error!";
			*ec << (ErrQExecutor | EDivBy0);
			return (ErrQExecutor | EDivBy0);
		}
		double tmp_value = value / ((QueryDoubleResult*) r)->getValue();
		res = new QueryDoubleResult(tmp_value);
	}
	return 0; 
}

// boolean functions
int QueryBoolResult::bool_and(QueryResult *r, QueryResult *&res){ 
	if (r->type() != QueryResult::QBOOL ) {
		*ec << "[QE] ERROR! AND arguments must be BOOLEAN";
		*ec << (ErrQExecutor | EBoolExpected);
		return (ErrQExecutor | EBoolExpected);
	}
	bool tmp_value = value && ((QueryBoolResult*) r)->getValue();
	res = new QueryBoolResult(tmp_value);
	return 0; 
}

int QueryBoolResult::bool_or(QueryResult *r, QueryResult *&res){ 
	if (r->type() != QueryResult::QBOOL ) {
		*ec << "[QE] ERROR! OR arguments must be BOOLEAN";
		*ec << (ErrQExecutor | EBoolExpected);
		return (ErrQExecutor | EBoolExpected);
	}
	bool tmp_value = value || ((QueryBoolResult*) r)->getValue();
	res = new QueryBoolResult(tmp_value);
	return 0; 
}

int QueryBoolResult::bool_not(QueryResult *&res){ 
	bool tmp_value = not value;
	res = new QueryBoolResult(tmp_value);
	return 0; 
}

// functions equal, not_equal etc.

bool QuerySequenceResult::equal(QueryResult *r){
	if (r->type() != QueryResult::QSEQUENCE) return false; 
	if (r->size() != seq.size()) return false;
	this->sortSequence();
	((QuerySequenceResult *) r)->sortSequence();
	for (unsigned int i = 0; i < (seq.size()); i++ ) {
		QueryResult* tmp_res;
		if (((QuerySequenceResult*) r)->at(i, tmp_res) != 0) return false;
		if (not ((seq.at(i))->equal(tmp_res))) return false;
	};
	return true;
}

bool QueryBagResult::equal(QueryResult *r){
	if (r->type() != QueryResult::QBAG) return false; 
	if (r->size() != bag.size()) return false; 
	this->sortBag();
	((QueryBagResult *) r)->sortBag();
	for (unsigned int i = 0; i < (bag.size()); i++ ) { 
		QueryResult* tmp_res;
		if (((QueryBagResult*) r)->at(i, tmp_res) != 0) return false;
		if (not ((bag.at(i))->equal(tmp_res))) return false;
	};
	return true;
}

bool QueryStructResult::equal(QueryResult *r){
	if (r->type() != QueryResult::QSTRUCT) return false; 
	if (r->size() != str.size()) return false; 
	this->sortStruct();
	((QueryStructResult *) r)->sortStruct();
	for (unsigned int i = 0; i < (str.size()); i++ ) { 
		QueryResult* tmp_res;
		if (((QueryStructResult*) r)->at(i, tmp_res) != 0) return false;
		if (not ((str.at(i))->equal(tmp_res))) return false;
	};
	return true;
}

bool QueryBinderResult::equal(QueryResult *r){
	if (r->type() != QueryResult::QBINDER ) return false; 
	if (name != ((QueryBinderResult*) r)->getName()) return false;
	return (item->equal(((QueryBinderResult*) r)->getItem()));
}

bool QueryStringResult::equal(QueryResult *r){
	if (r->type() != QueryResult::QSTRING ) return false; 
	return (value == ((QueryStringResult*) r)->getValue());
}

bool QueryIntResult::equal(QueryResult *r){
	if ((r->type() != QueryResult::QINT) && (r->type() != QueryResult::QDOUBLE)) return false; 
	if (r->type() == QueryResult::QINT) return (value == ((QueryIntResult*) r)->getValue());
	else return ((double)value == ((QueryDoubleResult*) r)->getValue());
}

bool QueryDoubleResult::equal(QueryResult *r){
	if ((r->type() != QueryResult::QINT) && (r->type() != QueryResult::QDOUBLE)) return false; 
	if (r->type() == QueryResult::QINT) return (value == (double)(((QueryIntResult*) r)->getValue()));
	else return (value == ((QueryDoubleResult*) r)->getValue());
}

bool QueryBoolResult::equal(QueryResult *r){
	if (r->type() != QueryResult::QBOOL ) return false; 
	return (value == ((QueryBoolResult*) r)->getValue());
}

bool QueryReferenceResult::equal(QueryResult *r){
	if (r->type() != QueryResult::QREFERENCE ) return false; 
	return ((value->toInteger()) == ((((QueryReferenceResult*) r)->getValue())->toInteger()));
}

bool QueryNothingResult::equal(QueryResult *r){ 
	if (r->type() != QueryResult::QNOTHING ) return false; 
	return true; 
}

bool QueryVirtualResult::equal(QueryResult *r){ 
	if (r->type() != QueryResult::QVIRTUAL) return false; 
	return ((this->toString(0,true)) == (r->toString(0,true)));
}

bool QuerySequenceResult::less_than(QueryResult *r){
	if (r->type() != QueryResult::QSEQUENCE) return ((this->type()) < (r->type())); 
	if (r->size() != seq.size()) return (seq.size() < r->size());
	this->sortSequence();
	((QuerySequenceResult *) r)->sortSequence();
	for (unsigned int i = 0; i < (seq.size()); i++ ) {
		QueryResult* tmp_res;
		if (((QuerySequenceResult*) r)->at(i, tmp_res) != 0) return false;
		if ((seq.at(i))->less_than(tmp_res)) return true;
		else if ((seq.at(i))->greater_than(tmp_res)) return false;
	};
	return false;
}

bool QueryBagResult::less_than(QueryResult *r){
	if (r->type() != QueryResult::QBAG) return ((this->type()) < (r->type())); 
	if (r->size() != bag.size()) return (bag.size() < r->size()); 
	this->sortBag();
	((QueryBagResult *) r)->sortBag();
	for (unsigned int i = 0; i < (bag.size()); i++ ) { 
		QueryResult* tmp_res;
		if (((QueryBagResult*) r)->at(i, tmp_res) != 0) return false;
		if ((bag.at(i))->less_than(tmp_res)) return true;
		else if ((bag.at(i))->greater_than(tmp_res)) return false;
	};
	return false;
}

bool QueryStructResult::less_than(QueryResult *r){
	if (r->type() != QueryResult::QSTRUCT) return ((this->type()) < (r->type())); 
	if (r->size() != str.size()) return (str.size() < r->size()); 
	this->sortStruct();
	((QueryStructResult *) r)->sortStruct();
	for (unsigned int i = 0; i < (str.size()); i++ ) { 
		QueryResult* tmp_res;
		if (((QueryStructResult*) r)->at(i, tmp_res) != 0) return false;
		if ((str.at(i))->less_than(tmp_res)) return true;
		else if ((str.at(i))->greater_than(tmp_res)) return false;
	};
	return false;
}

bool QueryBinderResult::less_than(QueryResult *r){
	if (r->type() != QueryResult::QBINDER ) return ((this->type()) < (r->type())); 
	if (name != ((QueryBinderResult*) r)->getName()) 
		return (name < ((QueryBinderResult*) r)->getName());
	return (item->less_than(((QueryBinderResult*) r)->getItem()));
}

bool QueryStringResult::less_than(QueryResult *r){
	if (r->type() != QueryResult::QSTRING ) return ((this->type()) < (r->type())); 
	return (value < ((QueryStringResult*) r)->getValue());
}

bool QueryIntResult::less_than(QueryResult *r){
	if ((r->type() != QueryResult::QINT) && (r->type() != QueryResult::QDOUBLE)) 
		return ((this->type()) < (r->type())); 
	if (r->type() == QueryResult::QINT) return (value < ((QueryIntResult*) r)->getValue());
	else return ((double)value < ((QueryDoubleResult*) r)->getValue());
}

bool QueryDoubleResult::less_than(QueryResult *r){
	if ((r->type() != QueryResult::QINT) && (r->type() != QueryResult::QDOUBLE)) 
		return ((this->type()) < (r->type())); 
	if (r->type() == QueryResult::QINT) return (value < (double)(((QueryIntResult*) r)->getValue()));
	else return (value < ((QueryDoubleResult*) r)->getValue());
}

bool QueryBoolResult::less_than(QueryResult *r){
	if (r->type() != QueryResult::QBOOL ) return ((this->type()) < (r->type())); 
	return (value < ((QueryBoolResult*) r)->getValue());
}

bool QueryReferenceResult::less_than(QueryResult *r){
	if (r->type() != QueryResult::QREFERENCE ) return ((this->type()) < (r->type())); 
	return ((value->toInteger()) < ((((QueryReferenceResult*) r)->getValue())->toInteger()));
}

bool QueryNothingResult::less_than(QueryResult *r){ 
	if (r->type() != QueryResult::QNOTHING ) return ((this->type()) < (r->type())); 
	return false; 
}

bool QueryVirtualResult::less_than(QueryResult *r){ 
	if (r->type() != QueryResult::QVIRTUAL) return ((this->type()) < (r->type())); 
	return ((this->toString(0,true)) < (r->toString(0,true)));
}

bool QueryResult::not_equal(QueryResult *r){
	bool tmp_value = not (this->equal(r)); 
	return tmp_value;
}

bool QueryResult::greater_than(QueryResult *r) {
	bool tmp_value = not ((this->equal(r)) || (this->less_than(r)));
	return tmp_value;
}

bool QueryResult::greater_eq(QueryResult *r) {
	bool tmp_value = not (this->less_than(r));
	return tmp_value;
}

bool QueryResult::less_eq(QueryResult *r) {
	bool tmp_value = ((this->equal(r)) || (this->less_than(r)));
	return tmp_value;
}

//function isBool() - returns true if result is a boolean (also if it is table 1x1 containing one bollean), false if not
bool QuerySequenceResult::isBool() { 
	if (seq.size() == 1)
		return ((seq.at(0))->isBool());
	else
		return false; 
}
bool QueryBagResult::isBool() { 
	if (bag.size() == 1)
		return ((bag.at(0))->isBool());
	else
		return false; 
}
bool QueryStructResult::isBool() { 
	if (str.size() == 1)
		return ((str.at(0))->isBool());
	else
		return false; 
}
bool QueryBinderResult::isBool() {
if (item != NULL)
		return (item->isBool());
	else
		return false; 
}
bool QueryStringResult::isBool()	{ return false; }
bool QueryIntResult::isBool()		{ return false; }
bool QueryDoubleResult::isBool()	{ return false; }
bool QueryBoolResult::isBool()		{ return true; }
bool QueryReferenceResult::isBool()	{ return false; }
bool QueryNothingResult::isBool()	{ return false; }
bool QueryVirtualResult::isBool()	{ return false; }


//function isReferenceValue() - returns true if result is a reference, false if not
bool QuerySequenceResult::isReferenceValue() { 
	if (seq.size() == 1)
		return ((seq.at(0))->isReferenceValue());
	else
		return false; 
}
bool QueryBagResult::isReferenceValue() { 
	if (bag.size() == 1)
		return ((bag.at(0))->isReferenceValue());
	else
		return false; 
}
bool QueryStructResult::isReferenceValue() { 
	if (str.size() == 1)
		return ((str.at(0))->isReferenceValue());
	else
		return false; 
}
bool QueryBinderResult::isReferenceValue() {
if (item != NULL)
		return (item->isReferenceValue());
	else
		return false; 
}
bool QueryStringResult::isReferenceValue()	{ return false; }
bool QueryIntResult::isReferenceValue()		{ return false; }
bool QueryDoubleResult::isReferenceValue()	{ return false; }
bool QueryBoolResult::isReferenceValue()	{ return false; }
bool QueryReferenceResult::isReferenceValue()	{ return true; }
bool QueryNothingResult::isReferenceValue()	{ return false; }
bool QueryVirtualResult::isReferenceValue()	{ return false; }

//function isNothing() - returns true if result is void (also if it is table 1x1 containing void), false if not
bool QuerySequenceResult::isNothing() { 
	if (seq.size() == 1)
		return ((seq.at(0))->isNothing());
	else if (seq.size() == 0)
		return true;
	else
		return false; 
}
bool QueryBagResult::isNothing() { 
	if (bag.size() == 1)
		return ((bag.at(0))->isNothing());
	else if (bag.size() == 0)
		return true;
	else
		return false; 
}
bool QueryStructResult::isNothing() { 
	if (str.size() == 1)
		return ((str.at(0))->isNothing());
	else if (str.size() == 0)
		return true;
	else
		return false; 
}
bool QueryBinderResult::isNothing()	{ return false; }
bool QueryStringResult::isNothing()	{ return false; }
bool QueryIntResult::isNothing()	{ return false; }
bool QueryDoubleResult::isNothing()	{ return false; }
bool QueryBoolResult::isNothing()	{ return false; }
bool QueryReferenceResult::isNothing()	{ return false; }
bool QueryNothingResult::isNothing()	{ return true; }
bool QueryVirtualResult::isNothing()	{ return false; }

// function getBoolValue returns a boolean value if the object is a bollean
int QuerySequenceResult::getBoolValue(bool &b) { 
	if (seq.size() == 1)
		return ((seq.at(0))->getBoolValue(b));
	else
		*ec << (ErrQExecutor | EBoolExpected);
		return (ErrQExecutor | EBoolExpected);
}
int QueryBagResult::getBoolValue(bool &b) {
	if (bag.size() == 1)
		return ((bag.at(0))->getBoolValue(b));
	else
		*ec << (ErrQExecutor | EBoolExpected);
		return (ErrQExecutor | EBoolExpected);
}
int QueryStructResult::getBoolValue(bool &b) { 
	if (str.size() == 1)
		return ((str.at(0))->getBoolValue(b));
	else
		*ec << (ErrQExecutor | EBoolExpected);
		return (ErrQExecutor | EBoolExpected);
}
int QueryBinderResult::getBoolValue(bool &b) {
	if (item != NULL)
		return (item->getBoolValue(b));
	else
		*ec << (ErrQExecutor | EBoolExpected);
		return (ErrQExecutor | EBoolExpected);
}
int QueryStringResult::getBoolValue(bool &b) { 
	*ec << (ErrQExecutor | EBoolExpected);
	return (ErrQExecutor | EBoolExpected);
}
int QueryIntResult::getBoolValue(bool &b) { 
	*ec << (ErrQExecutor | EBoolExpected);
	return (ErrQExecutor | EBoolExpected);
}
int QueryDoubleResult::getBoolValue(bool &b) { 
	*ec << (ErrQExecutor | EBoolExpected);
	return (ErrQExecutor | EBoolExpected);
}
int QueryBoolResult::getBoolValue(bool &b) { 
	b = value;
	return 0;
}
int QueryReferenceResult::getBoolValue(bool &b) { 
	*ec << (ErrQExecutor | EBoolExpected);
	return (ErrQExecutor | EBoolExpected);
}
int QueryNothingResult::getBoolValue(bool &b) { 
	*ec << (ErrQExecutor | EBoolExpected);
	return (ErrQExecutor | EBoolExpected);
}

int QueryVirtualResult::getBoolValue(bool &b) { 
	*ec << (ErrQExecutor | EBoolExpected);
	return (ErrQExecutor | EBoolExpected);
}

// function getReferenceValue returns reference value if the object is a reference
int QuerySequenceResult::getReferenceValue(QueryResult *&r) {
	if (seq.size() == 1)
		return ((seq.at(0))->getReferenceValue(r));
	else
		*ec << (ErrQExecutor | ERefExpected);
		return (ErrQExecutor | ERefExpected);
}
int QueryBagResult::getReferenceValue(QueryResult *&r) {
	if (bag.size() == 1)
		return ((bag.at(0))->getReferenceValue(r));
	else
		*ec << (ErrQExecutor | ERefExpected);
		return (ErrQExecutor | ERefExpected);
}
int QueryStructResult::getReferenceValue(QueryResult *&r) { 
	if (str.size() == 1)
		return ((str.at(0))->getReferenceValue(r));
	else
		*ec << (ErrQExecutor | ERefExpected);
		return (ErrQExecutor | ERefExpected);
}
int QueryBinderResult::getReferenceValue(QueryResult *&r) {
	if (item != NULL)
		return (item->getReferenceValue(r));
	else
		*ec << (ErrQExecutor | ERefExpected);
		return (ErrQExecutor | ERefExpected);
}
int QueryStringResult::getReferenceValue(QueryResult *&r) { 
	*ec << (ErrQExecutor | ERefExpected);
	return (ErrQExecutor | ERefExpected);
}
int QueryIntResult::getReferenceValue(QueryResult *&r) { 
	*ec << (ErrQExecutor | ERefExpected);
	return (ErrQExecutor | ERefExpected);
}
int QueryDoubleResult::getReferenceValue(QueryResult *&r) { 
	*ec << (ErrQExecutor | ERefExpected);
	return (ErrQExecutor | ERefExpected);
}
int QueryBoolResult::getReferenceValue(QueryResult *&r) { 
	*ec << (ErrQExecutor | ERefExpected);
	return (ErrQExecutor | ERefExpected);
}
int QueryReferenceResult::getReferenceValue(QueryResult *&r) { 
	r = new QueryReferenceResult(value);
	return 0;
}
int QueryNothingResult::getReferenceValue(QueryResult *&r) { 
	*ec << (ErrQExecutor | ERefExpected);
	return (ErrQExecutor | ERefExpected);
}

int QueryVirtualResult::getReferenceValue(QueryResult *&r) { 
	*ec << (ErrQExecutor | ERefExpected);
	return (ErrQExecutor | ERefExpected);
}



bool QueryResult::sorting_strict_less(QueryResult *arg) {
	
	QueryResult *tmp_left_order_seq;
	if ((((QueryStructResult *) this)->at(1, tmp_left_order_seq)) != 0) return false;
	QueryResult *left_order_seq = new QueryStructResult();
	((QueryStructResult *) left_order_seq)->addResult(tmp_left_order_seq);
	
	QueryResult *tmp_right_order_seq;
	if ((((QueryStructResult *) arg)->at(1, tmp_right_order_seq)) != 0) return false;
	QueryResult *right_order_seq = new QueryStructResult();
	((QueryStructResult *) right_order_seq)->addResult(tmp_right_order_seq);
	
	if (left_order_seq->size() < right_order_seq->size()) return true;
	if (left_order_seq->size() > right_order_seq->size()) return false;
	
	for (unsigned int i = 0; i < left_order_seq->size(); i++) {
		QueryResult *left_order_seq_elem;
		if ((((QueryStructResult *) left_order_seq)->at(i, left_order_seq_elem)) != 0) return false;
		QueryResult *right_order_seq_elem;
		if ((((QueryStructResult *) right_order_seq)->at(i, right_order_seq_elem)) != 0) return false;
		if (left_order_seq_elem->less_than(right_order_seq_elem)) return true;
		else if (left_order_seq_elem->greater_than(right_order_seq_elem)) return false;
	}
	
	return false;
}

void QuerySequenceResult::sortSequence() {
	sort(seq.begin(), seq.end(), ptr_less<QueryResult>());
}

void QueryBagResult::sortBag() {
	sort(bag.begin(), bag.end(), ptr_less<QueryResult>());
}

void QueryStructResult::sortStruct() {
	sort(str.begin(), str.end(), ptr_less<QueryResult>());
}

void QueryBagResult::sortBag_in_orderBy() {
	sort(bag.begin(), bag.end(), ptr_less_in_orderBy<QueryResult>());
}

int QueryBagResult::postSort(QueryResult *&f) {
	*ec << "[QE] postSort()";
	int errcode;
	f = new QuerySequenceResult();
	for (unsigned int i = 0; i < bag.size(); i++) {
		QueryResult *elem;
		errcode = ((QueryStructResult *) bag.at(i))->at(0, elem);
		if (errcode != 0) return errcode;
		f->addResult(elem);
	}
	return 0;
};

}
