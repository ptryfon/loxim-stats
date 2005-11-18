#ifndef _QUERYRESULT_H_
#define _QUERYRESULT_H_

#include <string>
#include "Store/Store.h"

//using namespace std;
using namespace QParser;
using namespace TManager;

namespace QExecutor {

class QueryResult {
 public:
  virtual bool operator==(QueryResult& r){}
  virtual string type(){}
  virtual QueryResult* clone(){}
  virtual ~Result() {}
};

class QuerySequenceResult : public QueryResult {
 public:
  virtual bool operator==(QueryResult& r){}
  virtual QueryResult* clone(){}
  virtual QueryResultIterator& getIterator(){}
  virtual void operator+=(Result& r){}
  virtual QueryResult& operator[](int p){}
  virtual string type() { return "sequence"; }
};

class QueryBagResult : public QueryResult {
 public:
  virtual bool operator==(QueryResult& r){}
  virtual QueryResult* clone(){}
  virtual QueryResultIterator& getIterator(){}
  virtual void operator+=(QueryResult& r){}
  virtual string type() { return "bag"; }      
};

class QueryStructResult : public QueryResult {
 public:
  virtual bool operator==(Result& r){}
  virtual QueryResult* clone(){}
  virtual QueryResultIterator& getIterator(){}
  virtual void operator+=(QueryResult& r){}
  virtual string type() { return "struct"; }     
};

class QueryBinderResult : public QueryResult {
 protected:
  string name;
  Result* item;
 public:
  virtual bool operator==(QueryResult& r){}
  virtual QueryResult* clone(){}
  BinderResult(string _name, QueryResult* _r) : name(_name), item(_r) {}
  string getName() { return name; }
  QueryResult* getItem() { return item; }
  virtual string type() { return "binder"; }      
  virtual ~QueryBinderResult() { if (item != NULL) delete item; }  
};

class QueryStringResult : public QueryResult {
 protected:
  string value;
 public:
  virtual bool operator==(QueryResult& r){}
  virtual QueryResult* clone(){}
  StringResult(string  _value) : value(_value) {}
  string getValue() { return value; }
  virtual string type() { return "string"; }
};

class QueryIntResult : public QueryResult {
 protected:
  int value;
 public:
  virtual bool operator==(QueryResult& r){}
  virtual QueryResult* clone(){}
  QueryIntResult(int  _value) : value(_value) {}
  int getValue() { return value; }
  virtual string type() { return "int"; }  
};

class QueryDoubleResult : public QueryResult {
 protected:
  double value;
 public:
  virtual bool operator==(QueryResult& r){}
  virtual QueryResult* clone(){}
  QueryDoubleResult(double _value) : value(_value) {}
  double getValue() { return value; }
  virtual string type() { return "double"; }
};

class QueryBoolResult : public QueryResult {
 protected:
  bool value;
 public:
  virtual bool operator==(Result& r){}
  virtual Result* clone(){}
  QueryBoolResult(bool _value) : value(_value) {}
  bool getValue() { return value; }
  virtual string type() { return "bool"; }      
};

class QueryReferenceResult : public QueryResult {
 protected:
  LogicalID *value;
 public:
  virtual bool operator==(Result& r){}
  virtual Result* clone(){}
  QueryReferenceResult(LogicalID* _value) : value(_value) {}
  LogicalID* getValue() { return value; }
  virtual string type() { return "reference"; }     
  virtual ~ReferenceResult() { if (value != NULL) delete value; }
};
}
#endif