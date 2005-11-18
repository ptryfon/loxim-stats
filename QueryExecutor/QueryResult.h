#ifndef _QUERYRESULT_H_
#define _QUERYRESULT_H_

#include <string>
#include "Store/Store.h"

//using namespace std;

namespace QExecutor {

class QueryResult {
 public:
  virtual bool operator==(QueryResult& r)=0;
  virtual string type()=0;
  virtual QueryResult* clone()=0;
  virtual ~QueryResult() {}
};

class QuerySequenceResult : public QueryResult {
 public:
  virtual bool operator==(QueryResult& r){ return false; }
  virtual QueryResult* clone(){ return NULL; }
  virtual void operator+=(QueryResult& r){}
  virtual QueryResult& operator[](int p){ return *(this->clone());}
  virtual string type() { return "sequence"; }
};

class QueryBagResult : public QueryResult {
 public:
  virtual bool operator==(QueryResult& r){ return false; }
  virtual QueryResult* clone(){ return NULL; }
  virtual void operator+=(QueryResult& r){}
  virtual string type() { return "bag"; }      
};

class QueryStructResult : public QueryResult {
 public:
  virtual bool operator==(QueryResult& r){ return false; }
  virtual QueryResult* clone(){ return NULL; }
  virtual void operator+=(QueryResult& r){ }
  virtual string type() { return "struct"; }     
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
  virtual string type() { return "binder"; }      
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
  virtual string type() { return "string"; }
};

class QueryIntResult : public QueryResult {
 protected:
  int value;
 public:
  virtual bool operator==(QueryResult& r){ return false; }
  virtual QueryResult* clone(){ return NULL; }
  QueryIntResult(int  _value) : value(_value) {}
  int getValue() { return value; }
  virtual string type() { return "int"; }  
};

class QueryDoubleResult : public QueryResult {
 protected:
  double value;
 public:
  virtual bool operator==(QueryResult& r){ return false; }
  virtual QueryResult* clone(){ return NULL; }
  QueryDoubleResult(double _value) : value(_value) {}
  double getValue() { return value; }
  virtual string type() { return "double"; }
};

class QueryBoolResult : public QueryResult {
 protected:
  bool value;
 public:
  virtual bool operator==(QueryResult& r){ return false; }
  virtual QueryResult* clone(){ return NULL; }
  QueryBoolResult(bool _value) : value(_value) {}
  bool getValue() { return value; }
  virtual string type() { return "bool"; }      
};

class QueryReferenceResult : public QueryResult {
 protected:
  LogicalID *value;
 public:
  virtual bool operator==(QueryResult& r){ return false; }
  virtual QueryResult* clone(){ return NULL; }
  QueryReferenceResult(LogicalID* _value) : value(_value) {}
  LogicalID* getValue() { return value; }
  virtual string type() { return "reference"; }     
  virtual ~QueryReferenceResult() { if (value != NULL) delete value; }
};
}

#endif
