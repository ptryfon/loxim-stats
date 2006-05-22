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
#include "Errors/Errors.h"
#include "Errors/ErrorConsole.h"

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
QueryReferenceResult::QueryReferenceResult(LogicalID* v) 	{ value=v; ec = new ErrorConsole("QueryExecutor"); refed = false; }
QueryNothingResult::QueryNothingResult() 			{ ec = new ErrorConsole("QueryExecutor"); }

//function clone()
QueryResult* QuerySequenceResult::clone()	{ return new QuerySequenceResult(seq); }
QueryResult* QueryBagResult::clone()		{ return new QueryBagResult(bag); }
QueryResult* QueryStructResult::clone()		{ return new QueryStructResult(str); }
QueryResult* QueryBinderResult::clone()		{ return new QueryBinderResult(name, (QueryResult*)item->clone()); }
QueryResult* QueryStringResult::clone()		{ return new QueryStringResult(value); }
QueryResult* QueryIntResult::clone()		{ return new QueryIntResult(value); }
QueryResult* QueryDoubleResult::clone()		{ return new QueryDoubleResult(value); }
QueryResult* QueryBoolResult::clone()		{ return new QueryBoolResult(value); }
QueryResult* QueryReferenceResult::clone()	{ return new QueryReferenceResult(value); }
QueryResult* QueryNothingResult::clone()	{ return new QueryNothingResult(); }

//function type() - returns type of a result
int QueryResult::type()			{ return QueryResult::QRESULT; }
int QuerySequenceResult::type()		{ return QueryResult::QSEQUENCE; }
int QueryBagResult::type()		{ return QueryResult::QBAG; }
int QueryStructResult::type()		{ return QueryResult::QSTRUCT; }
int QueryBinderResult::type()		{ return QueryResult::QBINDER; }
int QueryStringResult::type()		{ return QueryResult::QSTRING; }
int QueryIntResult::type()		{ return QueryResult::QINT; }
int QueryDoubleResult::type()		{ return QueryResult::QDOUBLE; }
int QueryBoolResult::type()		{ return QueryResult::QBOOL; }
int QueryReferenceResult::type()	{ return QueryResult::QREFERENCE; }
int QueryNothingResult::type()		{ return QueryResult::QNOTHING; }

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

//function size() - returns how many other results does a result contain
unsigned int QuerySequenceResult::size()	{ return seq.size(); }
unsigned int QueryBagResult::size()		{ return bag.size(); }
unsigned int QueryStructResult::size()		{ return str.size(); }
unsigned int QueryBinderResult::size()		{ if (item != NULL) {return 1;} else {return 0;}; }
unsigned int QueryStringResult::size()		{ return 0; }
unsigned int QueryIntResult::size()		{ return 0; }
unsigned int QueryDoubleResult::size()		{ return 0; }
unsigned int QueryBoolResult::size()		{ return 0; }
unsigned int QueryReferenceResult::size()	{ return 0; }
unsigned int QueryNothingResult::size()		{ return 0; }

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
		return ErrQExecutor | EQEmptySet; 
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
		return ErrQExecutor | EQEmptySet; 
	};
	r=(seq.at(i));
	return 0;
}

int QuerySequenceResult::lastElem(QueryResult *&r){ 
	int s = seq.size();
	if ( s == 0 ) {
		*ec << (ErrQExecutor | EQEmptySet);
		return ErrQExecutor | EQEmptySet; 
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
		return ErrQExecutor | EQEmptySet; 
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
		return ErrQExecutor | EQEmptySet; 
	};
	r=(bag.at(i));
	return 0;
}

int QueryBagResult::lastElem(QueryResult *&r){ 
	int s = bag.size();
	if ( s == 0 ) {
		*ec << (ErrQExecutor | EQEmptySet);
		return ErrQExecutor | EQEmptySet; 
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
			errcode = (r->getResult(tmp_res));
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
		return ErrQExecutor | EQEmptySet;  
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
		return ErrQExecutor | EQEmptySet; 
	};
	r=(str.at(i));
	return 0;
}

int QueryStructResult::lastElem(QueryResult *&r){ 
	int s = str.size();
	if ( s == 0 ) {
		*ec << (ErrQExecutor | EQEmptySet);
		return ErrQExecutor | EQEmptySet; 
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

//algebraic operations plus, minus, times, divide_by, and, or.

//function plus()
int QueryIntResult::plus(QueryResult *r, QueryResult *&res){ 
	if ((r->type() != QueryResult::QINT) && (r->type() != QueryResult::QDOUBLE)) {
		*ec << "[QE] ERROR! + arguments must be INT or DOUBLE";
		*ec << (ErrQExecutor | ENumberExpected);
		return ErrQExecutor | ENumberExpected;
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
		return ErrQExecutor | ENumberExpected; 
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
		return ErrQExecutor | ENumberExpected; 
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
		return ErrQExecutor | ENumberExpected;
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
		return ErrQExecutor | ENumberExpected;
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
		return ErrQExecutor | ENumberExpected;
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
		return ErrQExecutor | ENumberExpected;
	}
	if (r->type() == QueryResult::QINT) {
		if (((QueryIntResult*) r)->getValue() == 0) {
			*ec << "[QE] Division by 0 error!";
			*ec << (ErrQExecutor | EDivBy0);
			return ErrQExecutor | EDivBy0;
		}
		double tmp_value = (double)value / (double)(((QueryIntResult*) r)->getValue());
		res = new QueryDoubleResult(tmp_value);
	}
	else { //QDOUBLE
		if (((QueryDoubleResult*) r)->getValue() == 0) {
			*ec << "[QE] Division by 0 error!";
			*ec << (ErrQExecutor | EDivBy0);
			return ErrQExecutor | EDivBy0;
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
		return ErrQExecutor | ENumberExpected;
	}
	if (r->type() == QueryResult::QINT) {
		if (((QueryIntResult*) r)->getValue() == 0) {
			*ec << "[QE] Division by 0 error!";
			*ec << (ErrQExecutor | EDivBy0);
			return ErrQExecutor | EDivBy0;
		}
		double tmp_value = value / (double)(((QueryIntResult*) r)->getValue());
		res = new QueryDoubleResult(tmp_value);
	}
	else { //QDOUBLE
		if (((QueryDoubleResult*) r)->getValue() == 0) {
			*ec << "[QE] Division by 0 error!";
			*ec << (ErrQExecutor | EDivBy0);
			return ErrQExecutor | EDivBy0;
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
		return ErrQExecutor | EBoolExpected;
	}
	bool tmp_value = value && ((QueryBoolResult*) r)->getValue();
	res = new QueryBoolResult(tmp_value);
	return 0; 
}

int QueryBoolResult::bool_or(QueryResult *r, QueryResult *&res){ 
	if (r->type() != QueryResult::QBOOL ) {
		*ec << "[QE] ERROR! OR arguments must be BOOLEAN";
		*ec << (ErrQExecutor | EBoolExpected);
		return ErrQExecutor | EBoolExpected;
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
	if (r->type() != QueryResult::QSEQUENCE ) {
		return false; 
	}
	if (r->size() != seq.size() ) {
		return false; 
	}
	bool tmp_value = true;
	for (unsigned int i = 0; i < (seq.size()); i++ ) {
		QueryResult* tmp_res;
		if ( ((QuerySequenceResult*) r)->at(i, tmp_res) != 0 ) {
			return false;
		};
		tmp_value = tmp_value && ( (seq.at(i))->equal(tmp_res) );
	};
	return tmp_value;
}

bool QueryBagResult::equal(QueryResult *r){
	if (r->type() != QueryResult::QBAG ) {
		return false; 
	}
	if (r->size() != bag.size() ) {
		return false; 
	}
	bool tmp_value = true;
	for (unsigned int i = 0; i < (bag.size()); i++ ) { 
		QueryResult* tmp_res;
		if ( ((QueryBagResult*) r)->at(i, tmp_res) != 0 ) {
			return false;
		};
		tmp_value = tmp_value && ( (bag.at(i))->equal(tmp_res) );
	};
	return tmp_value;
}

bool QueryStructResult::equal(QueryResult *r){
	if (r->type() != QueryResult::QSTRUCT ) {
		return false; 
	}
	if (r->size() != str.size() ) {
		return false; 
	}
	bool tmp_value = true;
	for (unsigned int i = 0; i < (str.size()); i++ ) { 
		QueryResult* tmp_res;
		if ( ((QueryStructResult*) r)->at(i, tmp_res) != 0 ) {
			return false;
		};
		tmp_value = tmp_value && ( (str.at(i))->equal(tmp_res) );
	};
	return tmp_value;
}

bool QueryBinderResult::equal(QueryResult *r){
	if (r->type() != QueryResult::QBINDER ) {
		return false; 
	}
	bool tmp_value1 = (name == ((QueryBinderResult*) r)->getName());
	bool tmp_value2 = (item->equal(((QueryBinderResult*) r)->getItem()));
	return (tmp_value1 && tmp_value2);
}

bool QueryStringResult::equal(QueryResult *r){
	if (r->type() != QueryResult::QSTRING ) {
		return false; 
	}
	bool tmp_value = (value == ((QueryStringResult*) r)->getValue());
	return tmp_value;
}

bool QueryIntResult::equal(QueryResult *r){
	if ((r->type() != QueryResult::QINT) && (r->type() != QueryResult::QDOUBLE)) {
		return false; 
	}
	bool tmp_value;
	if (r->type() == QueryResult::QINT)
		tmp_value = (value == ((QueryIntResult*) r)->getValue());
	else
		tmp_value = ((double)value == ((QueryDoubleResult*) r)->getValue());
	return tmp_value;
}

bool QueryDoubleResult::equal(QueryResult *r){
	if ((r->type() != QueryResult::QINT) && (r->type() != QueryResult::QDOUBLE)) {
		return false; 
	}
	bool tmp_value;
	if (r->type() == QueryResult::QINT)
		tmp_value = (value == (double)(((QueryIntResult*) r)->getValue()));
	else
		tmp_value = (value == ((QueryDoubleResult*) r)->getValue());
	return tmp_value;
}

bool QueryBoolResult::equal(QueryResult *r){
	if (r->type() != QueryResult::QBOOL ) {
		return false; 
	}
	bool tmp_value = (value == ((QueryBoolResult*) r)->getValue());
	return tmp_value;
}

bool QueryReferenceResult::equal(QueryResult *r){
	if (r->type() != QueryResult::QREFERENCE ) {
		return false; 
	}
	unsigned int first = value->toInteger();
	unsigned int second = (((QueryReferenceResult*) r)->getValue())->toInteger();
	bool tmp_value = (first == second);
	return tmp_value;
}

bool QueryNothingResult::equal(QueryResult *r){ 
	if (r->type() != QueryResult::QNOTHING ) {
		return false; 
	}
	return true; 
}


bool QuerySequenceResult::not_equal(QueryResult *r){
	bool tmp_value = not ( this->equal(r) ); 
	return tmp_value;
}

bool QueryBagResult::not_equal(QueryResult *r){
	bool tmp_value = not ( this->equal(r) );
	return tmp_value;
}

bool QueryStructResult::not_equal(QueryResult *r){
	bool tmp_value = not ( this->equal(r) );
	return tmp_value;
}

bool QueryBinderResult::not_equal(QueryResult *r){
	bool tmp_value = not ( this->equal(r) );
	return tmp_value;
}

bool QueryStringResult::not_equal(QueryResult *r){
	bool tmp_value = not ( this->equal(r) );
	return tmp_value;
}

bool QueryIntResult::not_equal(QueryResult *r){
	bool tmp_value = not ( this->equal(r) );
	return tmp_value;
}

bool QueryDoubleResult::not_equal(QueryResult *r){
	bool tmp_value = not ( this->equal(r) );
	return tmp_value;
}

bool QueryBoolResult::not_equal(QueryResult *r){
	bool tmp_value = not ( this->equal(r) );
	return tmp_value;
}

bool QueryReferenceResult::not_equal(QueryResult *r){
	bool tmp_value = not ( this->equal(r) );
	return tmp_value;
}

bool QueryNothingResult::not_equal(QueryResult *r){
	bool tmp_value = not ( this->equal(r) );
	return tmp_value;
}



bool QueryStringResult::greater_than(QueryResult *r){
	if (r->type() != QueryResult::QSTRING ) {
		return false; 
	}
	bool tmp_value = (value > ((QueryStringResult*) r)->getValue());
	return tmp_value;
}

bool QueryIntResult::greater_than(QueryResult *r){
	if ((r->type() != QueryResult::QINT) && (r->type() != QueryResult::QDOUBLE)) {
		return false; 
	}
	bool tmp_value;
	if (r->type() == QueryResult::QINT)
		tmp_value = (value > ((QueryIntResult*) r)->getValue());
	else
		tmp_value = ((double)value > ((QueryDoubleResult*) r)->getValue());
	return tmp_value;
}

bool QueryDoubleResult::greater_than(QueryResult *r){
	if ((r->type() != QueryResult::QINT) && (r->type() != QueryResult::QDOUBLE)) {
		return false; 
	}
	bool tmp_value;
	if (r->type() == QueryResult::QINT)
		tmp_value = (value > (double)(((QueryIntResult*) r)->getValue()));
	else
		tmp_value = (value > ((QueryDoubleResult*) r)->getValue());
	return tmp_value;
}

bool QueryBoolResult::greater_than(QueryResult *r){
	if (r->type() != QueryResult::QBOOL ) {
		return false; 
	}
	bool tmp_value = (value > ((QueryBoolResult*) r)->getValue());
	return tmp_value;
}

bool QueryReferenceResult::greater_than(QueryResult *r){
	if (r->type() != QueryResult::QREFERENCE ) {
		return false; 
	}
	unsigned int first = value->toInteger();
	unsigned int second = (((QueryReferenceResult*) r)->getValue())->toInteger();
	bool tmp_value = (first > second);
	return tmp_value;
}

bool QueryStringResult::less_than(QueryResult *r){
	if (r->type() != QueryResult::QSTRING ) {
		return false; 
	}
	bool tmp_value = (value < ((QueryStringResult*) r)->getValue());
	return tmp_value;
}

bool QueryIntResult::less_than(QueryResult *r){
	if ((r->type() != QueryResult::QINT) && (r->type() != QueryResult::QDOUBLE)) {
		return false; 
	}
	bool tmp_value;
	if (r->type() == QueryResult::QINT)
		tmp_value = (value < ((QueryIntResult*) r)->getValue());
	else
		tmp_value = ((double)value < ((QueryDoubleResult*) r)->getValue());
	return tmp_value;
}

bool QueryDoubleResult::less_than(QueryResult *r){
	if ((r->type() != QueryResult::QINT) && (r->type() != QueryResult::QDOUBLE)) {
		return false; 
	}
	bool tmp_value;
	if (r->type() == QueryResult::QINT)
		tmp_value = (value < (double)(((QueryIntResult*) r)->getValue()));
	else
		tmp_value = (value < ((QueryDoubleResult*) r)->getValue());
	return tmp_value;
}

bool QueryBoolResult::less_than(QueryResult *r){
	if (r->type() != QueryResult::QBOOL ) {
		return false; 
	}
	bool tmp_value = (value < ((QueryBoolResult*) r)->getValue());
	return tmp_value;
}

bool QueryReferenceResult::less_than(QueryResult *r){
	if (r->type() != QueryResult::QREFERENCE ) {
		return false; 
	}
	unsigned int first = value->toInteger();
	unsigned int second = (((QueryReferenceResult*) r)->getValue())->toInteger();
	bool tmp_value = (first < second);
	return tmp_value;
}

bool QueryStringResult::greater_eq(QueryResult *r){
	if (r->type() != QueryResult::QSTRING ) {
		return false; 
	}
	bool tmp_value = (value >= ((QueryStringResult*) r)->getValue());
	return tmp_value;
}

bool QueryIntResult::greater_eq(QueryResult *r){
	if ((r->type() != QueryResult::QINT) && (r->type() != QueryResult::QDOUBLE)) {
		return false; 
	}
	bool tmp_value;
	if (r->type() == QueryResult::QINT)
		tmp_value = (value >= ((QueryIntResult*) r)->getValue());
	else
		tmp_value = ((double)value >= ((QueryDoubleResult*) r)->getValue());
	return tmp_value;
}

bool QueryDoubleResult::greater_eq(QueryResult *r){
	if ((r->type() != QueryResult::QINT) && (r->type() != QueryResult::QDOUBLE)) {
		return false; 
	}
	bool tmp_value;
	if (r->type() == QueryResult::QINT)
		tmp_value = (value >= (double)(((QueryIntResult*) r)->getValue()));
	else
		tmp_value = (value >= ((QueryDoubleResult*) r)->getValue());
	return tmp_value;
}

bool QueryBoolResult::greater_eq(QueryResult *r){
	if (r->type() != QueryResult::QBOOL ) {
		return false; 
	}
	bool tmp_value = (value >= ((QueryBoolResult*) r)->getValue());
	return tmp_value;
}

bool QueryReferenceResult::greater_eq(QueryResult *r){
	if (r->type() != QueryResult::QREFERENCE ) {
		return false; 
	}
	unsigned int first = value->toInteger();
	unsigned int second = (((QueryReferenceResult*) r)->getValue())->toInteger();
	bool tmp_value = (first >= second);
	return tmp_value;
}

bool QueryStringResult::less_eq(QueryResult *r){
	if (r->type() != QueryResult::QSTRING ) {
		return false; 
	}
	bool tmp_value = (value <= ((QueryStringResult*) r)->getValue());
	return tmp_value;
}

bool QueryIntResult::less_eq(QueryResult *r){
	if ((r->type() != QueryResult::QINT) && (r->type() != QueryResult::QDOUBLE)) {
		return false; 
	}
	bool tmp_value;
	if (r->type() == QueryResult::QINT)
		tmp_value = (value <= ((QueryIntResult*) r)->getValue());
	else
		tmp_value = ((double)value <= ((QueryDoubleResult*) r)->getValue());
	return tmp_value;
}

bool QueryDoubleResult::less_eq(QueryResult *r){
	if ((r->type() != QueryResult::QINT) && (r->type() != QueryResult::QDOUBLE)) {
		return false; 
	}
	bool tmp_value;
	if (r->type() == QueryResult::QINT)
		tmp_value = (value <= (double)(((QueryIntResult*) r)->getValue()));
	else
		tmp_value = (value <= ((QueryDoubleResult*) r)->getValue());
	return tmp_value;
}

bool QueryBoolResult::less_eq(QueryResult *r){
	if (r->type() != QueryResult::QBOOL ) {
		return false; 
	}
	bool tmp_value = (value <= ((QueryBoolResult*) r)->getValue());
	return tmp_value;
}

bool QueryReferenceResult::less_eq(QueryResult *r){
	if (r->type() != QueryResult::QREFERENCE ) {
		return false; 
	}
	unsigned int first = value->toInteger();
	unsigned int second = (((QueryReferenceResult*) r)->getValue())->toInteger();
	bool tmp_value = (first <= second);
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
bool QueryBinderResult::isNothing() {
if (item != NULL)
		return (item->isNothing());
	else
		return false; 
}
bool QueryStringResult::isNothing()	{ return false; }
bool QueryIntResult::isNothing()	{ return false; }
bool QueryDoubleResult::isNothing()	{ return false; }
bool QueryBoolResult::isNothing()	{ return false; }
bool QueryReferenceResult::isNothing()	{ return false; }
bool QueryNothingResult::isNothing()	{ return true; }

// function getBoolValue returns a boolean value if the object is a bollean
int QuerySequenceResult::getBoolValue(bool &b) { 
	if (seq.size() == 1)
		return ((seq.at(0))->getBoolValue(b));
	else
		*ec << (ErrQExecutor | EBoolExpected);
		return ErrQExecutor | EBoolExpected;
}
int QueryBagResult::getBoolValue(bool &b) {
	if (bag.size() == 1)
		return ((bag.at(0))->getBoolValue(b));
	else
		*ec << (ErrQExecutor | EBoolExpected);
		return ErrQExecutor | EBoolExpected;
}
int QueryStructResult::getBoolValue(bool &b) { 
	if (str.size() == 1)
		return ((str.at(0))->getBoolValue(b));
	else
		*ec << (ErrQExecutor | EBoolExpected);
		return ErrQExecutor | EBoolExpected;
}
int QueryBinderResult::getBoolValue(bool &b) {
	if (item != NULL)
		return (item->getBoolValue(b));
	else
		*ec << (ErrQExecutor | EBoolExpected);
		return ErrQExecutor | EBoolExpected;
}
int QueryStringResult::getBoolValue(bool &b) { 
	*ec << (ErrQExecutor | EBoolExpected);
	return ErrQExecutor | EBoolExpected;
}
int QueryIntResult::getBoolValue(bool &b) { 
	*ec << (ErrQExecutor | EBoolExpected);
	return ErrQExecutor | EBoolExpected;
}
int QueryDoubleResult::getBoolValue(bool &b) { 
	*ec << (ErrQExecutor | EBoolExpected);
	return ErrQExecutor | EBoolExpected;
}
int QueryBoolResult::getBoolValue(bool &b) { 
	b = value;
	return 0;
}
int QueryReferenceResult::getBoolValue(bool &b) { 
	*ec << (ErrQExecutor | EBoolExpected);
	return ErrQExecutor | EBoolExpected;
}
int QueryNothingResult::getBoolValue(bool &b) { 
	*ec << (ErrQExecutor | EBoolExpected);
	return ErrQExecutor | EBoolExpected;
}

// function getReferenceValue returns reference value if the object is a reference
int QuerySequenceResult::getReferenceValue(QueryResult *&r) {
	if (seq.size() == 1)
		return ((seq.at(0))->getReferenceValue(r));
	else
		*ec << (ErrQExecutor | ERefExpected);
		return ErrQExecutor | ERefExpected;
}
int QueryBagResult::getReferenceValue(QueryResult *&r) {
	if (bag.size() == 1)
		return ((bag.at(0))->getReferenceValue(r));
	else
		*ec << (ErrQExecutor | ERefExpected);
		return ErrQExecutor | ERefExpected;
}
int QueryStructResult::getReferenceValue(QueryResult *&r) { 
	if (str.size() == 1)
		return ((str.at(0))->getReferenceValue(r));
	else
		*ec << (ErrQExecutor | ERefExpected);
		return ErrQExecutor | ERefExpected;
}
int QueryBinderResult::getReferenceValue(QueryResult *&r) {
	if (item != NULL)
		return (item->getReferenceValue(r));
	else
		*ec << (ErrQExecutor | ERefExpected);
		return ErrQExecutor | ERefExpected;
}
int QueryStringResult::getReferenceValue(QueryResult *&r) { 
	*ec << (ErrQExecutor | ERefExpected);
	return ErrQExecutor | ERefExpected;
}
int QueryIntResult::getReferenceValue(QueryResult *&r) { 
	*ec << (ErrQExecutor | ERefExpected);
	return ErrQExecutor | ERefExpected;
}
int QueryDoubleResult::getReferenceValue(QueryResult *&r) { 
	*ec << (ErrQExecutor | ERefExpected);
	return ErrQExecutor | ERefExpected;
}
int QueryBoolResult::getReferenceValue(QueryResult *&r) { 
	*ec << (ErrQExecutor | ERefExpected);
	return ErrQExecutor | ERefExpected;
}
int QueryReferenceResult::getReferenceValue(QueryResult *&r) { 
	r = new QueryReferenceResult(value);
	return 0;
}
int QueryNothingResult::getReferenceValue(QueryResult *&r) { 
	*ec << (ErrQExecutor | ERefExpected);
	return ErrQExecutor | ERefExpected;
}

int QuerySequenceResult::comma(QueryResult *arg, QueryResult *&score) {
	int errcode;
	for (unsigned int i = 0; i < seq.size(); i++) {
		QueryResult *tmp_res = seq.at(i);
		errcode = tmp_res->comma(arg, score);
		if (errcode != 0) return errcode;
	} 
	return 0;
}
int QueryBagResult::comma(QueryResult *arg, QueryResult *&score) {
	int errcode;
	for (unsigned int i = 0; i < bag.size(); i++) {
		QueryResult *tmp_res = bag.at(i);
		errcode = tmp_res->comma(arg, score);
		if (errcode != 0) return errcode;
	}
	return 0;
}
int QueryStructResult::comma(QueryResult *arg, QueryResult *&score) {
	int errcode;
	switch (arg->type()) {
		case QueryResult::QBAG: {
			for (unsigned int i = 0; i < arg->size(); i++) {
				QueryResult *tmp;
				errcode = ((QueryBagResult *) arg)->at(i, tmp);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res = new QueryStructResult(str);
				((QueryStructResult *) tmp_res)->addResult(tmp);
				((QueryBagResult *) score)->addResult(tmp_res);
			}
			break;
		} 
		case QueryResult::QSEQUENCE: {
			for (unsigned int i = 0; i < arg->size(); i++) {
				QueryResult *tmp;
				errcode = ((QuerySequenceResult *) arg)->at(i, tmp);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res = new QueryStructResult(str);
				((QueryStructResult *) tmp_res)->addResult(tmp);
				((QueryBagResult *) score)->addResult(tmp_res);
			}
			break;
		}
		default: {
			QueryResult *tmp_res = new QueryStructResult(str);
			((QueryStructResult *) tmp_res)->addResult(arg);
			((QueryBagResult *) score)->addResult(tmp_res);
			break;
		}
	}
	return 0;
}
int QueryBinderResult::comma(QueryResult *arg, QueryResult *&score) {
	int errcode;
	switch (arg->type()) {
		case QueryResult::QBAG: {
			for (unsigned int i = 0; i < arg->size(); i++) {
				QueryResult *tmp;
				errcode = ((QueryBagResult *) arg)->at(i, tmp);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res = new QueryStructResult();
				QueryResult *current = new QueryBinderResult(name, item);
				((QueryStructResult *) tmp_res)->addResult(current);
				((QueryStructResult *) tmp_res)->addResult(tmp);
				((QueryBagResult *) score)->addResult(tmp_res);
			}
			break;
		} 
		case QueryResult::QSEQUENCE: {
			for (unsigned int i = 0; i < arg->size(); i++) {
				QueryResult *tmp;
				errcode = ((QuerySequenceResult *) arg)->at(i, tmp);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res = new QueryStructResult();
				QueryResult *current = new QueryBinderResult(name, item);
				((QueryStructResult *) tmp_res)->addResult(current);
				((QueryStructResult *) tmp_res)->addResult(tmp);
				((QueryBagResult *) score)->addResult(tmp_res);
			}
			break;
		}
		default: {
			QueryResult *tmp_res = new QueryStructResult();
			QueryResult *current = new QueryBinderResult(name, item);
			((QueryStructResult *) tmp_res)->addResult(current);
			((QueryStructResult *) tmp_res)->addResult(arg);
			((QueryBagResult *) score)->addResult(tmp_res);
			break;
		}
	}
	return 0;
}
int QueryStringResult::comma(QueryResult *arg, QueryResult *&score) {
	int errcode;
	switch (arg->type()) {
		case QueryResult::QBAG: {
			for (unsigned int i = 0; i < arg->size(); i++) {
				QueryResult *tmp;
				errcode = ((QueryBagResult *) arg)->at(i, tmp);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res = new QueryStructResult();
				QueryResult *current = new QueryStringResult(value);
				((QueryStructResult *) tmp_res)->addResult(current);
				((QueryStructResult *) tmp_res)->addResult(tmp);
				((QueryBagResult *) score)->addResult(tmp_res);
			}
			break;
		} 
		case QueryResult::QSEQUENCE: {
			for (unsigned int i = 0; i < arg->size(); i++) {
				QueryResult *tmp;
				errcode = ((QuerySequenceResult *) arg)->at(i, tmp);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res = new QueryStructResult();
				QueryResult *current = new QueryStringResult(value);
				((QueryStructResult *) tmp_res)->addResult(current);
				((QueryStructResult *) tmp_res)->addResult(tmp);
				((QueryBagResult *) score)->addResult(tmp_res);
			}
			break;
		}
		default: {
			QueryResult *tmp_res = new QueryStructResult();
			QueryResult *current = new QueryStringResult(value);
			((QueryStructResult *) tmp_res)->addResult(current);
			((QueryStructResult *) tmp_res)->addResult(arg);
			((QueryBagResult *) score)->addResult(tmp_res);
			break;
		}
	}
	return 0;
}
int QueryIntResult::comma(QueryResult *arg, QueryResult *&score) {
	int errcode;
	switch (arg->type()) {
		case QueryResult::QBAG: {
			for (unsigned int i = 0; i < arg->size(); i++) {
				QueryResult *tmp;
				errcode = ((QueryBagResult *) arg)->at(i, tmp);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res = new QueryStructResult();
				QueryResult *current = new QueryIntResult(value);
				((QueryStructResult *) tmp_res)->addResult(current);
				((QueryStructResult *) tmp_res)->addResult(tmp);
				((QueryBagResult *) score)->addResult(tmp_res);
			}
			break;
		} 
		case QueryResult::QSEQUENCE: {
			for (unsigned int i = 0; i < arg->size(); i++) {
				QueryResult *tmp;
				errcode = ((QuerySequenceResult *) arg)->at(i, tmp);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res = new QueryStructResult();
				QueryResult *current = new QueryIntResult(value);
				((QueryStructResult *) tmp_res)->addResult(current);
				((QueryStructResult *) tmp_res)->addResult(tmp);
				((QueryBagResult *) score)->addResult(tmp_res);
			}
			break;
		}
		default: {
			QueryResult *tmp_res = new QueryStructResult();
			QueryResult *current = new QueryIntResult(value);
			((QueryStructResult *) tmp_res)->addResult(current);
			((QueryStructResult *) tmp_res)->addResult(arg);
			((QueryBagResult *) score)->addResult(tmp_res);
			break;
		}
	}
	return 0;
}
int QueryDoubleResult::comma(QueryResult *arg, QueryResult *&score) {
	int errcode;
	switch (arg->type()) {
		case QueryResult::QBAG: {
			for (unsigned int i = 0; i < arg->size(); i++) {
				QueryResult *tmp;
				errcode = ((QueryBagResult *) arg)->at(i, tmp);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res = new QueryStructResult();
				QueryResult *current = new QueryDoubleResult(value);
				((QueryStructResult *) tmp_res)->addResult(current);
				((QueryStructResult *) tmp_res)->addResult(tmp);
				((QueryBagResult *) score)->addResult(tmp_res);
			}
			break;
		} 
		case QueryResult::QSEQUENCE: {
			for (unsigned int i = 0; i < arg->size(); i++) {
				QueryResult *tmp;
				errcode = ((QuerySequenceResult *) arg)->at(i, tmp);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res = new QueryStructResult();
				QueryResult *current = new QueryDoubleResult(value);
				((QueryStructResult *) tmp_res)->addResult(current);
				((QueryStructResult *) tmp_res)->addResult(tmp);
				((QueryBagResult *) score)->addResult(tmp_res);
			}
			break;
		}
		default: {
			QueryResult *tmp_res = new QueryStructResult();
			QueryResult *current = new QueryDoubleResult(value);
			((QueryStructResult *) tmp_res)->addResult(current);
			((QueryStructResult *) tmp_res)->addResult(arg);
			((QueryBagResult *) score)->addResult(tmp_res);
			break;
		}
	}
	return 0;
}
int QueryReferenceResult::comma(QueryResult *arg, QueryResult *&score) {
	int errcode;
	switch (arg->type()) {
		case QueryResult::QBAG: {
			for (unsigned int i = 0; i < arg->size(); i++) {
				QueryResult *tmp;
				errcode = ((QueryBagResult *) arg)->at(i, tmp);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res = new QueryStructResult();
				QueryResult *current = new QueryReferenceResult(value);
				((QueryStructResult *) tmp_res)->addResult(current);
				((QueryStructResult *) tmp_res)->addResult(tmp);
				((QueryBagResult *) score)->addResult(tmp_res);
			}
			break;
		} 
		case QueryResult::QSEQUENCE: {
			for (unsigned int i = 0; i < arg->size(); i++) {
				QueryResult *tmp;
				errcode = ((QuerySequenceResult *) arg)->at(i, tmp);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res = new QueryStructResult();
				QueryResult *current = new QueryReferenceResult(value);
				((QueryStructResult *) tmp_res)->addResult(current);
				((QueryStructResult *) tmp_res)->addResult(tmp);
				((QueryBagResult *) score)->addResult(tmp_res);
			}
			break;
		}
		default: {
			QueryResult *tmp_res = new QueryStructResult();
			QueryResult *current = new QueryReferenceResult(value);
			((QueryStructResult *) tmp_res)->addResult(current);
			((QueryStructResult *) tmp_res)->addResult(arg);
			((QueryBagResult *) score)->addResult(tmp_res);
			break;
		}
	}
	return 0;
}
int QueryBoolResult::comma(QueryResult *arg, QueryResult *&score) {
	int errcode;
	switch (arg->type()) {
		case QueryResult::QBAG: {
			for (unsigned int i = 0; i < arg->size(); i++) {
				QueryResult *tmp;
				errcode = ((QueryBagResult *) arg)->at(i, tmp);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res = new QueryStructResult();
				QueryResult *current = new QueryBoolResult(value);
				((QueryStructResult *) tmp_res)->addResult(current);
				((QueryStructResult *) tmp_res)->addResult(tmp);
				((QueryBagResult *) score)->addResult(tmp_res);
			}
			break;
		} 
		case QueryResult::QSEQUENCE: {
			for (unsigned int i = 0; i < arg->size(); i++) {
				QueryResult *tmp;
				errcode = ((QuerySequenceResult *) arg)->at(i, tmp);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res = new QueryStructResult();
				QueryResult *current = new QueryBoolResult(value);
				((QueryStructResult *) tmp_res)->addResult(current);
				((QueryStructResult *) tmp_res)->addResult(tmp);
				((QueryBagResult *) score)->addResult(tmp_res);
			}
			break;
		}
		default: {
			QueryResult *tmp_res = new QueryStructResult();
			QueryResult *current = new QueryBoolResult(value);
			((QueryStructResult *) tmp_res)->addResult(current);
			((QueryStructResult *) tmp_res)->addResult(arg);
			((QueryBagResult *) score)->addResult(tmp_res);
			break;
		}
	}
	return 0;
}
int QueryNothingResult::comma(QueryResult *arg, QueryResult *&score) {
	int errcode;
	switch (arg->type()) {
		case QueryResult::QBAG: {
			for (unsigned int i = 0; i < arg->size(); i++) {
				QueryResult *tmp;
				errcode = ((QueryBagResult *) arg)->at(i, tmp);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res = new QueryStructResult();
				QueryResult *current = new QueryNothingResult();
				((QueryStructResult *) tmp_res)->addResult(current);
				((QueryStructResult *) tmp_res)->addResult(tmp);
				((QueryBagResult *) score)->addResult(tmp_res);
			}
			break;
		} 
		case QueryResult::QSEQUENCE: {
			for (unsigned int i = 0; i < arg->size(); i++) {
				QueryResult *tmp;
				errcode = ((QuerySequenceResult *) arg)->at(i, tmp);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res = new QueryStructResult();
				QueryResult *current = new QueryNothingResult();
				((QueryStructResult *) tmp_res)->addResult(current);
				((QueryStructResult *) tmp_res)->addResult(tmp);
				((QueryBagResult *) score)->addResult(tmp_res);
			}
			break;
		}
		default: {
			QueryResult *tmp_res = new QueryStructResult();
			QueryResult *current = new QueryNothingResult();
			((QueryStructResult *) tmp_res)->addResult(current);
			((QueryStructResult *) tmp_res)->addResult(arg);
			((QueryBagResult *) score)->addResult(tmp_res);
			break;
		}
	}
	return 0;
}

bool QueryResult::sorting_less_eq(QueryResult *arg) {
	if (((this->type()) == (arg->type())) || 
	(((this->type()) == (QueryResult::QINT)) && ((arg->type()) == (QueryResult::QDOUBLE))) || 
	(((arg->type()) == (QueryResult::QINT)) && ((this->type()) == (QueryResult::QDOUBLE)))) {
		if (this->less_eq(arg))
			return true;
		else 
			return false;
	}
	else if ((this->type()) < (arg->type())) {
			return true;
	}
	return false;
}

int QueryBagResult::divideBag(QueryResult *&left, QueryResult *&right) {
	*ec << "[QE] divideBag()";
	unsigned int bagSize = bag.size();
	div_t temp;
	temp = div (bagSize,2);
	unsigned int bagHalf = temp.quot;
	left = new QueryBagResult();
	right = new QueryBagResult();
	for (unsigned int i = 0; i < bagHalf; i++) {
		left->addResult(bag.at(i));
	}
	for (unsigned int i = bagHalf; i < bagSize; i++) {
		right->addResult(bag.at(i));
	}
	return 0;
}

int QuerySequenceResult::sortCollection(QueryResult *r) {
	*ec << "[QE] sortCollection()";
	int errcode;
	unsigned int bagSize = r->size();
	switch (bagSize) {
		case 0: { 
			break; 
		}
		case 1: {
			QueryResult *current;
			errcode = r->getResult(current);
			if (errcode != 0) return errcode;
			seq.push_back(current);
			break;
		}
		default: {
			QueryResult *first_bag;
			QueryResult *second_bag;
			errcode = ((QueryBagResult *) r)->divideBag(first_bag, second_bag);
			if (errcode != 0) return errcode;
			QueryResult *first_seq = new QuerySequenceResult();
			errcode = ((QuerySequenceResult *) first_seq)->sortCollection(first_bag);
			delete first_bag;
			if (errcode != 0) return errcode;
			QueryResult *second_seq = new QuerySequenceResult();
			errcode = ((QuerySequenceResult *) second_seq)->sortCollection(second_bag);
			delete second_bag;
			if (errcode != 0) return errcode;
			unsigned int seq_size = (first_seq->size()) + (second_seq->size());
			if (seq_size != bagSize) {
				*ec << "[QE] sortCollection() ERROR! - i lost some elements somewhere...";
				*ec << (ErrQExecutor | EQEUnexpectedErr);
				return ErrQExecutor | EQEUnexpectedErr;
			}
			for (unsigned int i = 0; i < seq_size; i++) {
				QueryResult *first_elem;
				QueryResult *second_elem;
				if ((first_seq->size()) == 0) { // first sequence ended
					errcode = second_seq->getResult(second_elem);
					if (errcode != 0) return errcode;
					seq.push_back(second_elem);
				} 
				else if ((second_seq->size()) == 0) { // second sequence ended
					errcode = first_seq->getResult(first_elem);
					if (errcode != 0) return errcode;
					seq.push_back(first_elem);
				} 
				else {
					errcode = ((QuerySequenceResult *) first_seq)->at(0,first_elem);
					if (errcode != 0) return errcode;
					errcode = ((QuerySequenceResult *) second_seq)->at(0,second_elem);
					if (errcode != 0) return errcode;
					if ((first_elem->type() != QueryResult::QSTRUCT) || (second_elem->type() != QueryResult::QSTRUCT)) {
						*ec << "[QE] sortCollection() ERROR - sorted elements must be struct type";
						*ec << (ErrQExecutor | EQEUnexpectedErr);
						return ErrQExecutor | EQEUnexpectedErr;
					}
					QueryResult *first_sorting;
					QueryResult *second_sorting;
					errcode = ((QueryStructResult *) first_elem)->at(1, first_sorting);
					if (errcode != 0) return errcode;
					errcode = ((QueryStructResult *) second_elem)->at(1, second_sorting);
					if (errcode != 0) return errcode;
					QueryResult *first_sStr;
					if ((first_sorting->type()) == QueryResult::QSTRUCT) {
						first_sStr = first_sorting;
					}
					else {
						first_sStr = new QueryStructResult();
						first_sStr->addResult(first_sorting);
					}
					QueryResult *second_sStr;
					if ((second_sorting->type()) == QueryResult::QSTRUCT) {
						second_sStr = second_sorting;
					}
					else {
						second_sStr = new QueryStructResult();
						second_sStr->addResult(second_sorting);
					}
					if ((first_sStr->size()) != (second_sStr->size())) {
						*ec << "[QE] sortCollection() ERROR - sorted elements must have equal number of elements";
						*ec << (ErrQExecutor | EQEUnexpectedErr);
						return ErrQExecutor | EQEUnexpectedErr;
					}
					QueryResult *first_sElem;
					QueryResult *second_sElem;
					bool first_lessEQ = false;
					bool second_lessEQ = false;
					unsigned int jj = 0;
					while ((jj < first_sStr->size()) && (first_lessEQ == second_lessEQ)) {
						errcode = ((QueryStructResult*)first_sStr)->at(jj, first_sElem);
						if (errcode != 0) return errcode;
						errcode = ((QueryStructResult*)second_sStr)->at(jj, second_sElem);
						if (errcode != 0) return errcode;
						first_lessEQ = first_sElem->sorting_less_eq(second_sElem);
						second_lessEQ = second_sElem->sorting_less_eq(first_sElem);
						if ((first_lessEQ) && (not(second_lessEQ))) {
							errcode = first_seq->getResult(first_elem);
							if (errcode != 0) return errcode;
							seq.push_back(first_elem);
						}
						else if ((second_lessEQ) && (not(first_lessEQ))) {
							errcode = second_seq->getResult(second_elem);
							if (errcode != 0) return errcode;
							seq.push_back(second_elem);
						}
						jj++;
					}
					if ((first_lessEQ) == (second_lessEQ)) {
						errcode = first_seq->getResult(first_elem);
						if (errcode != 0) return errcode;
						seq.push_back(first_elem);
					}
					if ((first_sorting->type()) != QueryResult::QSTRUCT) delete first_sStr;
					if ((second_sorting->type()) != QueryResult::QSTRUCT) delete second_sStr;
				}
			}
			delete first_seq;
			delete second_seq;
			break;
		}
	}
	return 0;
}

int QuerySequenceResult::postSort(QueryResult *&f) {
	*ec << "[QE] postSort()";
	int errcode;
	f = new QuerySequenceResult();
	for (unsigned int i = 0; i < seq.size(); i++) {
		QueryResult *tmp_res = seq.at(i);
		QueryResult *elem;
		errcode = ((QueryStructResult *)tmp_res)->at(0,elem);
		if (errcode != 0) return errcode;
		f->addResult(elem);
	}
	return 0;
};


}
