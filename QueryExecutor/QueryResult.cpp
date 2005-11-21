#include <vector>
#include "QueryResult.h"

#include <string>
#include <vector>
#include "Store/Store.h"

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
QueryBinderResult::QueryBinderResult(string n, QueryResult* r) 	{ name=n; item=r; }
QueryStringResult::QueryStringResult(string v) 			{ value=v; }
QueryIntResult::QueryIntResult(int v) 				{ value=v; }
QueryDoubleResult::QueryDoubleResult(double v) 			{ value=v; }
QueryBoolResult::QueryBoolResult(bool v) 			{ value=v; }
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
int QuerySequenceResult::size()		{ return seq.size(); }
int QueryBagResult::size()		{ return bag.size(); }
int QueryStructResult::size()		{ return str.size(); }
int QueryBinderResult::size()		{ if (item != NULL) {return 1;} else {return 0;}; }
int QueryStringResult::size()		{ return 0; }
int QueryIntResult::size()		{ return 0; }
int QueryDoubleResult::size()		{ return 0; }
int QueryBoolResult::size()		{ return 0; }
int QueryReferenceResult::size()	{ return 0; }
int QueryNothingResult::size()		{ return 0; }

//accesors for collection result classes - sequence, bag, sruct
void QuerySequenceResult::addResult(QueryResult *r)	{ seq.push_back(r); }
int QuerySequenceResult::getResult(QueryResult *r)	{ if (seq.empty()) { return -1; } 
							else { r=(seq.front()); it=(seq.begin()); seq.erase(it);}; return 0; }

void QueryBagResult::addResult(QueryResult *r)		{ bag.push_back(r); }
int QueryBagResult::getResult(QueryResult *r)		{ if (bag.empty()) { return -1; } 
							else { r=(bag.front()); it=(bag.begin()); bag.erase(it);}; return 0; }

void QueryStructResult::addResult(QueryResult *r)	{ str.push_back(r); }
int QueryStructResult::getResult(QueryResult *r)	{ if (str.empty()) { return -1; } 
							else { r=(str.front()); it=(str.begin()); str.erase(it);}; return 0; }

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

}

/* TODO 
- zglosic blad w funkcji getResult pojawiajacy sie gdy ktos sprubuje pobrac cos z pustej kolekcji
- po przydzieleniu errno poprawic return -1 na return errno w tej funkcji

- zastanowic sie jak naprawde powinny wygladac i zachowywac sie bag, sequence i struct
- zmienic ich implementacje

- opracowac operatory ==, >, < etc. +, -, etc...
- byc moze dodac operator at() do kolekcji
- moze takze dodac replace(i), remove(i)

- do rozwazenia, czy zrobimy osobna klase na stos czy wykorzystamy ktoryz z typow result,
- do rozwazenie czy operacje typu suma zbiorów, przeciecie, i inne podobne jakie bedziemy wykonywac na wynikach maja byc zdefiniowane
  w QueryExecutorze czy tez moga to byc wbudowane metody poszczególnych resultów */
   
