#ifndef RESULT_H_
#define RESULT_H_

#include <iostream>
#include <iterator>
#include <vector>

using namespace std;

class Result
{
public:
	enum ResultType { RESULT=0,    SEQUENCE=1, BAG=2, 
			  STRUCT=3,    BINDER=4,   STRING=5, 
			  INT=6,       DOUBLE=7,   BOOL=8, 
			  REFERENCE=9, VOID=10,    ERROR=11,
			  BOOLTRUE=12, BOOLFALSE=13 }; //piotrek
	Result() {};
	virtual bool    operator==(Result& r)    =0;
  	virtual Result* clone()			 =0;
	virtual int     getType() { return RESULT; }
	virtual ~Result() { }
	
	//Maybe remove that:
	virtual void    toStream(ostream& os) const { os << "no type"; }
	friend ostream& operator<<(ostream&, Result&);	
};//class Result



class ResultBag : public Result
{
private:	
	vector<Result*> bag;

public:
	ResultBag(unsigned long size) : bag(0){} 

	bool    operator==(Result& r){ return false;       }
  	Result* clone()              { return NULL;        }
  	int 	getType()	     { return Result::BAG; }      
	int     size()		     { return bag.size();  }
	void    add(Result* val)     { bag.push_back(val); }
	Result* get(int i)	     { return bag.at(i);   }
	void   toStream(ostream& os) const ;
	
	virtual ~ResultBag() {} 
// don't know the class of bat.at(i), so can't delete! hmm...
//		{ for(unsigned int i=0; i<bag.size(); i++) delete bag.at(i); }
};//class ResultBag : ResultCollection


class ResultStruct : public Result
{
private:	
	vector<Result*> str;
	
public:
	ResultStruct(unsigned long size)  : str(0){      }

	bool    operator==(Result& r){ return false;          }
  	Result* clone()              { return NULL;           }
  	int 	getType()         { return Result::STRUCT;    }      
	int     size()            { return str.size();        }
	void    add(Result* val)  { str.push_back(val);       }
	Result* get(int i)	  { return str.at(i);	      }
	void   toStream(ostream& os) const ;
		
	virtual ~ResultStruct()   {}
// don't know the class of bat.at(i), so can't delete! hmm...	
//		{ for(unsigned int i=0; i<str.size(); i++) delete str.at(i); }
};//class ResultStruct


class ResultSequence : public Result
{
private:	
	vector<Result*> seq;
	
public:
	ResultSequence(unsigned long size)  : seq(0){ }

	bool    operator==(Result& r){ return false;          }
  	Result* clone()              { return NULL;           }
  	int 	getType()         { return Result::SEQUENCE;  }      
	int     size()            { return seq.size();        }
	void    add(Result* val)  { seq.push_back(val);       }
	Result* get(int i)	  { return seq.at(i);	      }
	void   toStream(ostream& os) const ;

	virtual ~ResultSequence() {}
// don't know the class of seq.at(i), so can't delete! hmm...	
//		{ for(unsigned int i=0; i<seq.size(); i++) delete seq.at(i); }
};//class ResultSequence


class ResultReference : public Result
{
private:
	int   value;
	char* ch_value;
	
public:
	ResultReference(int val)     { value = val;     }
	ResultReference(char* val)   { ch_value = val;  }
	
	bool   operator==(Result& r) { return false;    }
//  	ResultReference* clone()   { return (new ResultReference(value)); }
	ResultReference* clone()     { return NULL;     }
	int    getType() 	     { return Result::REFERENCE; }
	
	void   setValue(int val)     { value = val;     }
	int    getValue()	     { return value;    }
	void   setChValue(char* val) { ch_value = val;  }
	char*  getChValue()          { return ch_value; }
//	string toString();
	void   toStream(ostream& os) const {os << value;}		
	virtual ~ResultReference()   { if (ch_value!=NULL) delete[] ch_value; }
};//class ResultReference 


class ResultInt  : public Result
{
private:	
	int value;
public:
	ResultInt(int val) : value(val) { }
	bool   operator==(Result& r) { return false;         }
	ResultReference* clone()     { return NULL;          }
	int    getType() 	     { return Result::INT;   }
	void   setValue(int value)   { this->value = value;  }
	int    getValue()	     { return value;	     }
	void   toStream(ostream& os) const { os << value;    }		
	~ResultInt() {}
};//class ResultInt


class ResultString  : public Result
{
private:	
	string value;
public:
	ResultString(string val) : value(val) { }
	bool   operator==(Result& r) { return false;         }
	ResultReference* clone()     { return NULL;          }
	int    getType() 	     { return Result::STRING;}
	void   setValue(string value){ this->value = value;  }
	string getValue()	     { return value;         }
	void   toStream(ostream& os) const { os << value;    }	
	~ResultString() {}
};//class ResultString


class ResultBool  : public Result
{
private:	
	bool value;
public:
	ResultBool(bool val) : value(val) { }
	bool   operator==(Result& r) { return false;         }
	ResultReference* clone()     { return NULL;          }
	int    getType() 	     { return Result::BOOL;  }
	void   setValue(bool value)  { this->value = value;  }
	bool   getValue()	     { return value;	     }
	void   toStream(ostream& os) const { os << value;    }
	~ResultBool() {}
};//class ResultString



class ResultVoid : public Result
{
public:
	ResultVoid() {};
	bool   operator==(Result& r) { return false;         }
	ResultReference* clone()     { return NULL;          }
	int    getType() 	     { return Result::VOID;  }
	void     toStream(ostream& os) const { os << "void"; }
	virtual ~ResultVoid() {};
};//class ResultVoid 


class ResultError : public Result
{
private:
	int     code;
	string  message;
public:
	ResultError() : code(0), message("error"){}
	ResultError(int    err) : code(err)      {}
	ResultError(string err) : message(err)   {}

	bool   operator==(Result& r) { return false;         }
	ResultReference* clone()     { return NULL;          }
	int    getType()             { return Result::ERROR; }
	int    getCode()             { return code;          }
	string getMessage()          { return message;       }
	void   toStream(ostream& os) const { os << message;  }
	virtual ~ResultError() {}
};//class ResultError


#endif /*RESULT_H_*/
