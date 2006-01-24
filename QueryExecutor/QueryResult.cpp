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
QuerySequenceResult::QuerySequenceResult() 			{}
QuerySequenceResult::QuerySequenceResult(vector<QueryResult*> s){ seq = s; }
QueryBagResult::QueryBagResult() 				{}
QueryBagResult::QueryBagResult(vector<QueryResult*> b)		{ bag = b; }
QueryStructResult::QueryStructResult() 				{}
QueryStructResult::QueryStructResult(vector<QueryResult*> s)	{ str = s; }
QueryBinderResult::QueryBinderResult()				{}
QueryBinderResult::QueryBinderResult(string n, QueryResult* r) 	{ name=n; item=r; }
QueryStringResult::QueryStringResult()				{}
QueryStringResult::QueryStringResult(string v) 			{ value=v; }
QueryIntResult::QueryIntResult()				{}
QueryIntResult::QueryIntResult(int v) 				{ value=v; }
QueryDoubleResult::QueryDoubleResult()				{}
QueryDoubleResult::QueryDoubleResult(double v) 			{ value=v; }
QueryBoolResult::QueryBoolResult()				{}
QueryBoolResult::QueryBoolResult(bool v) 			{ value=v; }
QueryReferenceResult::QueryReferenceResult()			{}
QueryReferenceResult::QueryReferenceResult(LogicalID* v) 	{ value=v; }
QueryNothingResult::QueryNothingResult() 			{}

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
			seq.push_back(tmp_res);
		}
	}
	else {
		seq.push_back(r);
	}
	//this->orderBy() 
}

int QuerySequenceResult::getResult(QueryResult *&r){ 
	if (seq.empty()) { 
		return EEmptySet | ErrQExecutor; 
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
		return -1;
	};
	r=(seq.at(i));
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
				bag.push_back(tmp_res);
			}
		}
	}
	else
		bag.push_back(r); 
}
int QueryBagResult::getResult(QueryResult *&r){
	if (bag.empty()) { 
		return EEmptySet | ErrQExecutor;
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
		return -1;
	};
	r=(bag.at(i));
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
			str.push_back(tmp_res);
		}
	}
	else
		str.push_back(r); 
}

int QueryStructResult::getResult(QueryResult *&r){ 
	if (str.empty()) { 
		return EEmptySet | ErrQExecutor; 
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
		return -1;
	};
	r=(str.at(i));
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


//algebraic operations plus, minus, times, divide_by, and, or.

//function plus()
int QueryIntResult::plus(QueryResult *r, QueryResult *&res){ 
	if ((r->type() != QueryResult::QINT) && (r->type() != QueryResult::QDOUBLE)) {
		res = new QueryNothingResult();
		fprintf (stderr, "[QE] ERROR! + arguments must be INT or DOUBLE\n");
		return -1; 
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
		res = new QueryNothingResult();
		fprintf (stderr, "[QE] ERROR! + arguments must be INT or DOUBLE\n");
		return -1; 
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
		res = new QueryNothingResult();
		fprintf (stderr, "[QE] ERROR! - arguments must be INT or DOUBLE\n");
		return -1; 
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
		res = new QueryNothingResult();
		fprintf (stderr, "[QE] ERROR! - arguments must be INT or DOUBLE\n");
		return -1; 
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
	int tmp_value = - value;
	res = new QueryIntResult(tmp_value);
	return 0; 
}

int QueryDoubleResult::minus(QueryResult *&res){ 
	double tmp_value = - value;
	res = new QueryDoubleResult(tmp_value);
	return 0; 
}

//function times()
int QueryIntResult::times(QueryResult *r, QueryResult *&res){ 
	if ((r->type() != QueryResult::QINT) && (r->type() != QueryResult::QDOUBLE)) {
		res = new QueryNothingResult();
		fprintf (stderr, "[QE] ERROR! * arguments must be INT or DOUBLE\n");
		return -1; 
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
		res = new QueryNothingResult();
		fprintf (stderr, "[QE] ERROR! * arguments must be INT or DOUBLE\n");
		return -1; 
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
		res = new QueryNothingResult();
		fprintf (stderr, "[QE] ERROR! / arguments must be INT or DOUBLE\n");
		return -1; 
	}
	if (r->type() == QueryResult::QINT) {
		if (((QueryIntResult*) r)->getValue() == 0) {
			res = new QueryNothingResult();
			fprintf (stderr, "[QE] Division by 0 error!\n");
			return -2; //devide by 0 error 
		}
		int tmp_value = value / ((QueryIntResult*) r)->getValue();
		res = new QueryIntResult(tmp_value);
	}
	else { //QDOUBLE
		if (((QueryDoubleResult*) r)->getValue() == 0) {
			res = new QueryNothingResult();
			fprintf (stderr, "[QE] Division by 0 error!\n");
			return -2; //devide by 0 error 
		}
		double tmp_value = (double)value / ((QueryDoubleResult*) r)->getValue();
		res = new QueryDoubleResult(tmp_value);
	}
	return 0; 
}

int QueryDoubleResult::divide_by(QueryResult *r, QueryResult *&res){ 
	if ((r->type() != QueryResult::QINT) && (r->type() != QueryResult::QDOUBLE)) {
		res = new QueryNothingResult();
		fprintf (stderr, "[QE] ERROR! / arguments must be INT or DOUBLE\n");
		return -1; 
	}
	if (r->type() == QueryResult::QINT) {
		if (((QueryIntResult*) r)->getValue() == 0) {
			res = new QueryNothingResult();
			fprintf (stderr, "[QE] Division by 0 error!\n");
			return -2; //devide by 0 error 
		}
		double tmp_value = value / (double)(((QueryIntResult*) r)->getValue());
		res = new QueryDoubleResult(tmp_value);
	}
	else { //QDOUBLE
		if (((QueryDoubleResult*) r)->getValue() == 0) {
			res = new QueryNothingResult();
			fprintf (stderr, "[QE] Division by 0 error!\n");
			return -2; //devide by 0 error 
		}
		double tmp_value = value / ((QueryDoubleResult*) r)->getValue();
		res = new QueryDoubleResult(tmp_value);
	}
	return 0; 
}

// boolean functions
int QueryBoolResult::bool_and(QueryResult *r, QueryResult *&res){ 
	if (r->type() != QueryResult::QBOOL ) {
		res = new QueryNothingResult();
		fprintf (stderr, "[QE] ERROR! AND arguments must be BOOLEAN\n");
		return -1; 
	}
	bool tmp_value = value && ((QueryBoolResult*) r)->getValue();
	res = new QueryBoolResult(tmp_value);
	return 0; 
}

int QueryBoolResult::bool_or(QueryResult *r, QueryResult *&res){ 
	if (r->type() != QueryResult::QBOOL ) {
		res = new QueryNothingResult();
		fprintf (stderr, "[QE] ERROR! OR arguments must be BOOLEAN\n");
		return -1; 
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
	if (r->type() != QueryResult::QINT ) {
		return false; 
	}
	bool tmp_value = (value == ((QueryIntResult*) r)->getValue());
	return tmp_value;
}

bool QueryDoubleResult::equal(QueryResult *r){
	if (r->type() != QueryResult::QDOUBLE ) {
		return false; 
	}
	bool tmp_value = (value == ((QueryDoubleResult*) r)->getValue());
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
	bool tmp_value = ((int)value == (int)(((QueryReferenceResult*) r)->getValue()));
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
	if (r->type() != QueryResult::QINT ) {
		return false; 
	}
	bool tmp_value = (value > ((QueryIntResult*) r)->getValue());
	return tmp_value;
}

bool QueryDoubleResult::greater_than(QueryResult *r){
	if (r->type() != QueryResult::QDOUBLE ) {
		return false; 
	}
	bool tmp_value = (value > ((QueryDoubleResult*) r)->getValue());
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
	bool tmp_value = ((int)value > (int)(((QueryReferenceResult*) r)->getValue()));
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
	if (r->type() != QueryResult::QINT ) {
		return false; 
	}
	bool tmp_value = (value < ((QueryIntResult*) r)->getValue());
	return tmp_value;
}

bool QueryDoubleResult::less_than(QueryResult *r){
	if (r->type() != QueryResult::QDOUBLE ) {
		return false; 
	}
	bool tmp_value = (value < ((QueryDoubleResult*) r)->getValue());
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
	bool tmp_value = ((int)value < (int)(((QueryReferenceResult*) r)->getValue()));
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
	if (r->type() != QueryResult::QINT ) {
		return false; 
	}
	bool tmp_value = (value >= ((QueryIntResult*) r)->getValue());
	return tmp_value;
}

bool QueryDoubleResult::greater_eq(QueryResult *r){
	if (r->type() != QueryResult::QDOUBLE ) {
		return false; 
	}
	bool tmp_value = (value >= ((QueryDoubleResult*) r)->getValue());
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
	bool tmp_value = ((int)value >= (int)(((QueryReferenceResult*) r)->getValue()));
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
	if (r->type() != QueryResult::QINT ) {
		return false; 
	}
	bool tmp_value = (value <= ((QueryIntResult*) r)->getValue());
	return tmp_value;
}

bool QueryDoubleResult::less_eq(QueryResult *r){
	if (r->type() != QueryResult::QDOUBLE ) {
		return false; 
	}
	bool tmp_value = (value <= ((QueryDoubleResult*) r)->getValue());
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
	bool tmp_value = ((int)value <= (int)(((QueryReferenceResult*) r)->getValue()));
	return tmp_value;
}


// nested function returns bag of binders, which will be pushed on the environment stack

int QuerySequenceResult::nested(Transaction *tr, QueryResult *&r) {
	fprintf(stderr, "[QE] nested(): ERROR! QuerySequenceResult shouldn't be nested\n");
	return -1; // nested () function is applied to rows of a QueryResult and so, it shouldn't be applied to sequences and bags
}

int QueryBagResult::nested(Transaction *tr, QueryResult *&r) {
	fprintf(stderr, "[QE] nested(): ERROR! QueryBagResult shouldn't be nested\n");
	return -1; // nested () function is applied to rows of a QueryResult and so, it shouldn't be applied to sequences and bags
}

int QueryStructResult::nested(Transaction *tr, QueryResult *&r) {
	fprintf(stderr, "[QE] nested(): QueryStructResult\n");
	int errcode;
	for (unsigned int i = 0; i < str.size(); i++) {
		if ((str.at(i))->type() == QueryResult::QSTRUCT)
			return -1; // one row shouldn't contain another row;
		else {
			errcode = ((str.at(i))->nested(tr, r));
			if (errcode != 0) return errcode;
		}
	}
	return 0;
}

int QueryStringResult::nested(Transaction *tr, QueryResult *&r) {
	fprintf(stderr, "[QE] nested(): QueryStringResult can't be nested\n");
	return 0;
}

int QueryIntResult::nested(Transaction *tr, QueryResult *&r) {
	fprintf(stderr, "[QE] nested(): QueryIntResult can't be nested\n");
	return 0;
}

int QueryDoubleResult::nested(Transaction *tr, QueryResult *&r) {
	fprintf(stderr, "[QE] nested(): QueryDoubleResult can't be nested\n");
	return 0;
}

int QueryBoolResult::nested(Transaction *tr, QueryResult *&r) {
	fprintf(stderr, "[QE] nested(): QueryBoolResult can't be nested\n");
	return 0;
}

int QueryNothingResult::nested(Transaction *tr, QueryResult *&r) {
	fprintf(stderr, "[QE] nested(): QueryNothingResult can't be nested\n");
	return 0;
}

int QueryBinderResult::nested(Transaction *tr, QueryResult *&r) {
	if (item != NULL) {
		QueryBinderResult *tmp_value = new QueryBinderResult(name,item);
		fprintf(stderr, "[QE] nested(): QueryBinderResult copy returned name: ");
		cout << name << endl;
		r->addResult(tmp_value);
	}
	return 0;
}

int QueryReferenceResult::nested(Transaction *tr, QueryResult *&r) {
	DataValue* tmp_data_value;
	int errcode;
	ObjectPointer *optr;
	if (value != NULL) {
		if ((errcode = tr->getObjectPointer(value, Store::Read, optr)) != 0) {
			fprintf(stderr, "[QE] Error in getObjectPointer\n");
			return errcode;
		}
		tmp_data_value = optr->getValue();
		int vType = tmp_data_value->getType();
		switch (vType) {
			case Store::Integer: {
				fprintf(stderr, "[QE] nested(): QueryReferenceResult pointing integer value - can't be nested\n");
				break;
			}
			case Store::Double: {
				fprintf(stderr, "[QE] nested(): QueryReferenceResult pointing double value - can't be nested\n");
				break;
			}
			case Store::String: {
				fprintf(stderr, "[QE] nested(): QueryReferenceResult pointing string value - can't be nested\n");
				break;
			}
			case Store::Pointer: {
				LogicalID *tmp_logID = (tmp_data_value->getPointer());
				if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, optr)) != 0) {
				fprintf(stderr, "[QE] Error in getObjectPointer\n");
					return errcode;
				}
				string tmp_name = optr->getName();
				QueryReferenceResult *final_ref = new QueryReferenceResult(tmp_logID);
				QueryBinderResult *final_binder = new QueryBinderResult(tmp_name, final_ref);
				fprintf(stderr, "[QE] nested(): QueryReferenceResult pointing reference value\n");
				r->addResult(final_binder);
				fprintf(stderr, "[QE] nested(): new QueryBinderResult returned name: ");
				cout << tmp_name << endl;
				break;
			}
			case Store::Vector: {
				vector<LogicalID*>* tmp_vec = (tmp_data_value->getVector());
				fprintf(stderr, "[QE] nested(): QueryReferenceResult pointing vector value\n");
				int vec_size = tmp_vec->size();
				for (int i = 0; i < vec_size; i++ ) {
					LogicalID *tmp_logID = tmp_vec->at(i);
					if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, optr)) != 0) {
					fprintf(stderr, "[QE] Error in getObjectPointer\n");
						return errcode;
					}
					string tmp_name = optr->getName();
					QueryReferenceResult *final_ref = new QueryReferenceResult(tmp_logID);
					QueryBinderResult *final_binder = new QueryBinderResult(tmp_name, final_ref);
					fprintf(stderr, "[QE] nested(): vector element number %d\n", i);
					r->addResult(final_binder);
					fprintf(stderr, "[QE] nested(): new QueryBinderResult returned name: ");
					cout << tmp_name << endl;
				}
				break;
			}
			default : {
				fprintf(stderr, "[QE] nested(): ERROR! QueryReferenceResult pointing unknown format value\n");
				return -1;
				break;
			}
		}
	}
	return 0;
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
bool QueryBinderResult::isBool()	{ return false; }
bool QueryStringResult::isBool()	{ return false; }
bool QueryIntResult::isBool()		{ return false; }
bool QueryDoubleResult::isBool()	{ return false; }
bool QueryBoolResult::isBool()		{ return true; }
bool QueryReferenceResult::isBool()	{ return false; }
bool QueryNothingResult::isBool()	{ return false; }

// function getBoolValue returns a boolean value if the object is a bollean
int QuerySequenceResult::getBoolValue(bool &b) { 
	if (seq.size() == 1)
		return ((seq.at(0))->getBoolValue(b));
	else
		return -1; 
}
int QueryBagResult::getBoolValue(bool &b) {
	if (bag.size() == 1)
		return ((bag.at(0))->getBoolValue(b));
	else
		return -1;
}
int QueryStructResult::getBoolValue(bool &b) { 
	if (str.size() == 1)
		return ((str.at(0))->getBoolValue(b));
	else
		return -1;
}
int QueryBinderResult::getBoolValue(bool &b)	{ return -1; } //error not a boolean type
int QueryStringResult::getBoolValue(bool &b)	{ return -1; } //error not a boolean type
int QueryIntResult::getBoolValue(bool &b)	{ return -1; } //error not a boolean type
int QueryDoubleResult::getBoolValue(bool &b)	{ return -1; } //error not a boolean type
int QueryBoolResult::getBoolValue(bool &b) { 
	b = value;
	return 0;
}
int QueryReferenceResult::getBoolValue(bool &b)	{ return -1; } //error not a boolean type
int QueryNothingResult::getBoolValue(bool &b)	{ return -1; } //error not a boolean type

//function isSingleValue() - returns true if result is a boolean,int,double,string,reference or nothing 
//(also if it is table 1x1 containing one of them), false if not
bool QuerySequenceResult::isSingleValue() { 
	if (seq.size() == 1)
		return ((seq.at(0))->isSingleValue());
	else
		return false; 
}
bool QueryBagResult::isSingleValue() { 
	if (bag.size() == 1)
		return ((bag.at(0))->isSingleValue());
	else
		return false; 
}
bool QueryStructResult::isSingleValue() { 
	if (str.size() == 1)
		return ((str.at(0))->isSingleValue());
	else
		return false; 
}
bool QueryBinderResult::isSingleValue() {
if (item != NULL)
		return (item->isSingleValue());
	else
		return false; 
}
bool QueryStringResult::isSingleValue()		{ return true; }
bool QueryIntResult::isSingleValue()		{ return true; }
bool QueryDoubleResult::isSingleValue()		{ return true; }
bool QueryBoolResult::isSingleValue()		{ return true; }
bool QueryReferenceResult::isSingleValue()	{ return true; }
bool QueryNothingResult::isSingleValue()	{ return true; }

int QuerySequenceResult::getSingleValue(QueryResult *&r) { 
	if (seq.size() == 1)
		return ((seq.at(0))->getSingleValue(r));
	else
		return -1; 
}
int QueryBagResult::getSingleValue(QueryResult *&r) {
	if (bag.size() == 1)
		return ((bag.at(0))->getSingleValue(r));
	else
		return -1;
}
int QueryStructResult::getSingleValue(QueryResult *&r) { 
	if (str.size() == 1)
		return ((str.at(0))->getSingleValue(r));
	else
		return -1;
}
int QueryBinderResult::getSingleValue(QueryResult *&r) {
if (item != NULL)
		return (item->getSingleValue(r));
	else
		return false; 
}
int QueryStringResult::getSingleValue(QueryResult *&r) {
	r = new QueryStringResult(value);
	return 0;
}
int QueryIntResult::getSingleValue(QueryResult *&r) {
	r = new QueryIntResult(value);
	return 0;
}
int QueryDoubleResult::getSingleValue(QueryResult *&r) {
	r = new QueryDoubleResult(value);
	return 0;
}
int QueryBoolResult::getSingleValue(QueryResult *&r) { 
	r = new QueryBoolResult(value);
	return 0;
}
int QueryReferenceResult::getSingleValue(QueryResult *&r) {
	r = new QueryReferenceResult(value);
	return 0;
}
int QueryNothingResult::getSingleValue(QueryResult *&r) {
	r = new QueryNothingResult();
	return 0;
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

}
