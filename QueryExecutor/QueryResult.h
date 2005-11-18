#ifndef _QUERYRESULT_H_
#define _QUERYRESULT_H_

#include <string>
#include "Store/Store.h"

//using namespace std;

namespace QExecutor {

enum QueryResultType {QRESULT, QSEQUENCE, QBAG, QSTRUCT, QBINDER, QSTRING, QINT, QDOUBLE, QBOOL, QREFERENCE, QNOTHING}


class QueryResult {
 public:
  virtual bool operator==(QueryResult& r)=0;
  virtual int type()=0;
  virtual QueryResult* clone()=0;
  virtual ~QueryResult() {}
};

class QuerySequenceResult : public QueryResult {
 public:
  virtual bool operator==(QueryResult& r){ return false; }
  virtual QueryResult* clone(){ return NULL; }
  virtual void operator+=(QueryResult& r){}
  virtual QueryResult& operator[](int p){ return *(this->clone());}
  virtual int type() { return QueryResultType::QSEQUENCE; }
};

class QueryBagResult : public QueryResult {
 public:
  virtual bool operator==(QueryResult& r){ return false; }
  virtual QueryResult* clone(){ return NULL; }
  virtual void operator+=(QueryResult& r){}
  virtual int type() { return QueryResultType::QBAG; }      
};

class QueryStructResult : public QueryResult {
 public:
  virtual bool operator==(QueryResult& r){ return false; }
  virtual QueryResult* clone(){ return NULL; }
  virtual void operator+=(QueryResult& r){ }
  virtual int type() { return QueryResultType::QSTRUCT; }     
};

class QueryBinderResult : public QueryResult {
 protected:
  string name;
  QueryResult* item;
 public:
  virtual bool operator==(QueryResult& r){ return false; }
  virtual QueryResult* clone(){ return NULL;}
  QueryBinderResult(string _name, QueryResult* _r) : name(_name), item(_r) {}
  string getName() { return name; }
  QueryResult* getItem() { return item; }
  virtual int type() { return QueryResultType::QBINDER; }      
  virtual ~QueryBinderResult() { if (item != NULL) delete item; }  
};

class QueryStringResult : public QueryResult {
 protected:
  string value;
 public:
  virtual bool operator==(QueryResult& r){ return false; }
  virtual QueryResult* clone(){ return NULL; }
  QueryStringResult(string  _value) : value(_value) {}
  string getValue() { return value; }
  virtual int type() { return QueryResultType::QSTRING; }
};

class QueryIntResult : public QueryResult {
 protected:
  int value;
 public:
  virtual bool operator==(QueryResult& r){ return false; }
  virtual QueryResult* clone(){ return NULL; }
  QueryIntResult(int  _value) : value(_value) {}
  int getValue() { return value; }
  virtual int type() { return QueryResultType::QINT; }
};

class QueryDoubleResult : public QueryResult {
 protected:
  double value;
 public:
  virtual bool operator==(QueryResult& r){ return false; }
  virtual QueryResult* clone(){ return NULL; }
  QueryDoubleResult(double _value) : value(_value) {}
  double getValue() { return value; }
  virtual int type() { return QueryResultType::QDOUBLE; }
};

class QueryBoolResult : public QueryResult {
 protected:
  bool value;
 public:
  virtual bool operator==(QueryResult& r){ return false; }
  virtual QueryResult* clone(){ return NULL; }
  QueryBoolResult(bool _value) : value(_value) {}
  bool getValue() { return value; }
  virtual int type() { return QueryResultType::QBOOL; }
};

class QueryReferenceResult : public QueryResult {
 protected:
  LogicalID *value;
 public:
  virtual bool operator==(QueryResult& r){ return false; }
  virtual QueryResult* clone(){ return NULL; }
  QueryReferenceResult(LogicalID* _value) : value(_value) {}
  LogicalID* getValue() { return value; }
  virtual int type() { return QueryResultType::QREFERENCE; }     
  virtual ~QueryReferenceResult() { if (value != NULL) delete value; }
};

class QueryNothing {
 public:
  virtual bool operator==(QueryResult& r) { return false; };
  virtual int type() { return QueryResultType::QNOTHING; };
  virtual QueryResult* clone() {return NULL; };
  virtual ~QueryNothing() {}
};

}
#endif
