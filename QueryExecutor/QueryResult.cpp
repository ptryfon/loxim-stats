#include <vector>
#include "QueryResult.h"

#include <string>
#include <vector>
#include "Store/Store.h"
#include "Errors/Errors.h"

using namespace std;

namespace QExecutor {

//constructors
QuerySequenceResult::QuerySequenceResult() 			{}
QuerySequenceResult::QuerySequenceResult(unsigned int size)	{}
QuerySequenceResult::QuerySequenceResult(vector<QueryResult*> s){ seq = s; }
QueryBagResult::QueryBagResult() 				{}
QueryBagResult::QueryBagResult(unsigned int size)		{}
QueryBagResult::QueryBagResult(vector<QueryResult*> b)		{ bag = b; }
QueryStructResult::QueryStructResult() 				{}
QueryStructResult::QueryStructResult(unsigned int size)		{}
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
	seq.push_back(r); 
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

void QueryBagResult::addResult(QueryResult *r){
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

void QueryStructResult::addResult(QueryResult *r){ 
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
QueryResult* QueryIntResult::plus(QueryResult *r){ 
	if (r->type() != QueryResult::QINT ) {
		return new QueryNothingResult(); 
	}
	int tmp_value = value + ((QueryIntResult*) r)->getValue();
	return new QueryIntResult(tmp_value); 
}

QueryResult* QueryDoubleResult::plus(QueryResult *r){
	if (r->type() != QueryResult::QDOUBLE ) {
		return new QueryNothingResult(); 
	}
	double tmp_value = value + ((QueryDoubleResult*) r)->getValue(); 
	return new QueryDoubleResult(tmp_value); 
}

//function minus()
QueryResult* QueryIntResult::minus(QueryResult *r){
	if (r->type() != QueryResult::QINT ) {
		return new QueryNothingResult(); 
	} 
	int tmp_value = value - ((QueryIntResult*) r)->getValue();
	return new QueryIntResult(tmp_value); 
}

QueryResult* QueryDoubleResult::minus(QueryResult *r){ 
	if (r->type() != QueryResult::QDOUBLE ) {
		return new QueryNothingResult(); 
	}
	double tmp_value = value - ((QueryDoubleResult*) r)->getValue();
	return new QueryDoubleResult(tmp_value);
}

QueryResult* QueryIntResult::minus(){ 
	bool tmp_value = - value;
	return new QueryIntResult(tmp_value); 
}

QueryResult* QueryDoubleResult::minus(){ 
	bool tmp_value = - value;
	return new QueryDoubleResult(tmp_value); 
}

//function times()
QueryResult* QueryIntResult::times(QueryResult *r){ 
	if (r->type() != QueryResult::QINT ) {
		return new QueryNothingResult(); 
	}
	int tmp_value = value * ((QueryIntResult*) r)->getValue();
	return new QueryIntResult(tmp_value); 
}

QueryResult* QueryDoubleResult::times(QueryResult *r){ 
	if (r->type() != QueryResult::QDOUBLE ) {
		return new QueryNothingResult(); 
	}
	double tmp_value = value * ((QueryDoubleResult*) r)->getValue();
	return new QueryDoubleResult(tmp_value);
}

//function divide_by()
QueryResult* QueryIntResult::divide_by(QueryResult *r){
	if (r->type() != QueryResult::QINT ) {
		return new QueryNothingResult(); 
	}
	if (((QueryIntResult*) r)->getValue() == 0) {
		return new QueryNothingResult(); 
	}
	int tmp_value = value / ((QueryIntResult*) r)->getValue();
	return new QueryIntResult(tmp_value); 
}

QueryResult* QueryDoubleResult::divide_by(QueryResult *r){ 
	if (r->type() != QueryResult::QDOUBLE ) {
		return new QueryNothingResult(); 
	}
	if (((QueryDoubleResult*) r)->getValue() == 0) {
		return new QueryNothingResult(); 
	}
	double tmp_value = value / ((QueryIntResult*) r)->getValue();
	return new QueryDoubleResult(tmp_value);
}

// boolean functions
QueryResult* QueryBoolResult::bool_and(QueryResult *r){ 
	if (r->type() != QueryResult::QBOOL ) {
		return new QueryNothingResult(); 
	}
	bool tmp_value = value && ((QueryBoolResult*) r)->getValue();
	return new QueryBoolResult(tmp_value); 
}

QueryResult* QueryBoolResult::bool_or(QueryResult *r){ 
	if (r->type() != QueryResult::QBOOL ) {
		return new QueryNothingResult(); 
	}
	bool tmp_value = value || ((QueryBoolResult*) r)->getValue();
	return new QueryBoolResult(tmp_value); 
}

QueryResult* QueryBoolResult::bool_not(){ 
	bool tmp_value = not value;
	return new QueryBoolResult(tmp_value); 
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
		if ( ((QuerySequenceResult*) r)->at(i, tmp_res) ) {
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
		if ( ((QueryBagResult*) r)->at(i, tmp_res) ) {
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
		if ( ((QueryStructResult*) r)->at(i, tmp_res) ) {
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










}

/* TODO 
- pisac err na konsole bagginsa

- zastanowic sie jak naprawde powinny wygladac i zachowywac sie bag, sequence i struct
  i ewentulanie zmienic ich implementacje

- byc moze opracowac operatory dla stringow np. konkatenacje
- byc moze dodac operatory replace(i), remove(i) do kolekcji

- do rozwazenia, czy zrobimy osobna klase na stos czy wykorzystamy ktoryz z typow result,
- do rozwazenie czy operacje typu suma zbiorów, przeciecie, i inne podobne jakie bedziemy wykonywac na wynikach maja byc zdefiniowane
  w QueryExecutorze czy tez moga to byc wbudowane metody poszczególnych resultów */

