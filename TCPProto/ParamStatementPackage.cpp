#include <netinet/in.h> 
#include <iostream>
#include <sstream>
#include "Package.h"

using namespace std;

namespace TCPProto {

	int ParamStatementPackage::serialize(char** buffer, int* size) {
		//StatementPackage::serialize(buffer, &tmpsize); 

		// cerr << *this;		
		map<string, Result*>::iterator it;
		int error = 0;
		
		/* Counting buffer length */
		int length = 1; //Package type
		length    += sizeof(unsigned long); //total length
		length    += sizeof(unsigned long); //stmtNr
		length    += sizeof(unsigned long); //paramCount
		it = params.begin();	
		while (it != params.end()) {
			string paramName = (*it).first;
			length += getStringLength(paramName);

			/* deprecated	length += sizeof(unsigned long); //stringLength
			length += sizeof(char) * (paramName.length() + 1);
			*/

			Result* paramResult = (*it).second;
			length += getResultLength(paramResult);
			length += sizeof(unsigned long);
			it++;
		}

		/* Malloc buffer */
		*buffer = (char*) malloc(length); // including type
		if (*buffer == NULL) return -2; //TODO
		bufferBegin = *buffer;
		bufferEnd   = *buffer + length;
		
		/* Serializing */		
		char type = (char) getType();
		memcpy(*buffer, &type, sizeof(char));
		bufferBegin += 1;
		
		error  = setULong(length);
		if (error != 0) return -2;
				
		error  = setULong(stmtNr);
		if (error != 0) return -2;

		error  = setULong(params.size());
		if (error != 0) return -2;

		it = params.begin();	
		while (it != params.end()) {
			string paramName = (*it).first;
			setString(paramName);

			Result* paramResult = (*it).second;
			setResult(paramResult);
						
			it++;
		}
		
		*size = length;
		return 0;
	}

	int ParamStatementPackage::deserialize(char* buffer, int size) {
		//return StatementPackage::deserialize(buffer, size);
		int error;
		
		if (buffer[0] != getType()) return -1; //TODO
		bufferBegin = buffer + 1;
		bufferEnd   = buffer + 1 + sizeof(unsigned long);
		
		unsigned long length;
		unsigned long paramCount;
		
		error  = getULong(&length);
		if (error != 0) return -2;
		bufferEnd = buffer + length;
		
		error  = getULong(&stmtNr);
		if (error != 0) return -2;
		
		error  = getULong(&paramCount);
		if (error != 0) return -2;

		// cerr << "length: " << length << endl;
		// cerr << "stmtNr: " << stmtNr << endl;
		// cerr << "paramCount: " << paramCount << endl;

				
		for (unsigned long i=0; i < paramCount; i++) {
			string paramName;
			getString(&paramName);
			QueryResult* paramResult;
			getQueryResult(&paramResult);
			queryParams[paramName] = paramResult;
		}
		
		free(buffer);

		return 0;
	}
	
	int ParamStatementPackage::getString(string* str) {
		unsigned long strLn = 0;
		char*         chr;
		int           error;
		
		char* tmpPtr = bufferBegin + sizeof(long);
		if (tmpPtr > bufferEnd) {
			return -2; //TODO proba czytania poza buforem
		}

		error = getULong(&strLn);
		if (error != 0) return -2;
			
		tmpPtr += strLn;
		if (tmpPtr > bufferEnd) {
			return -2; //TODO proba czytania poza buforem
		}
		
		chr = (char*) malloc(sizeof(char) * strLn);
		memcpy(chr, bufferBegin, strLn);

		if (str == NULL) 
			str = new string();
		else 
			str->clear();
		str->append(chr);
		
		free(chr);
		
		bufferBegin = tmpPtr; //skip the number of elements (long)

		return 0;
	}
	
	
	
	int ParamStatementPackage::setString(string str) {
		char* tmpPtr = bufferBegin + sizeof(unsigned long) + str.length() + 1;
		if (tmpPtr > bufferEnd) {
			return -2; //TODO proba czytania poza buforem
		}
		
		setULong(str.length()+1);
		memcpy(bufferBegin, str.c_str(), str.length()+1);
		bufferBegin = tmpPtr;
		
		return 0;
	}
	
	int ParamStatementPackage::getStringLength(string str) {
		return sizeof(unsigned long) + str.length() + 1;
		//string size + string ln + '\0'
	}
	
	
	int ParamStatementPackage::setDouble(double val) {
		unsigned long *in  = (unsigned long*) &val;
		int error; 
		
		error =	setULong(in[0]);
		if (error != 0) return -2;
	
		error = setULong(in[1]);
		if (error != 0) return -2;
	
		return 0;	
	}
	
	int ParamStatementPackage::getDouble(double* val) {
		unsigned long* out = (unsigned long*) val;
		int error; 
		
		error = getULong(&(out[0]));
		if (error != 0) return -2;
		
		error = getULong(&(out[1]));
		if (error != 0) return -2;
		
		return 0;	
	}
	
	int ParamStatementPackage::getResultCollectionLength(ResultCollection* collection) {
		int length = 0; // why not 1? We don't need to add ResultCollection type
		length += sizeof(unsigned long); //collection size
		int i;
		for(i=0;i < collection->size(); i++) {
			length += getResultLength(collection->get(i));
		}
		return length;
	}
	
	int	ParamStatementPackage::getResultLength(Result* result) {
		int length = sizeof(unsigned long);	//result type
		ResultString* rstring; 	//it is used somewhere in switch
		ResultReference* rref;  //it is used somewhere in switch
		ResultBinder* rbinder;  //it is used somewhere in switch
		string str; 			//it is used somewhere in switch
		
		switch (result->getType()) {
	
		case Result::BAG:
			length += getResultCollectionLength((ResultCollection*) result);
			break;
						
		case Result::SEQUENCE:
			length += getResultCollectionLength((ResultCollection*) result);
			break;

		case Result::STRUCT:
			length += getResultCollectionLength((ResultCollection*) result);
			break;
			
		case Result::REFERENCE:
			rref = (ResultReference*) result;
			str  = rref->getValue();
			length += sizeof(unsigned long) + str.length() + 1;
			break;

		case Result::VOID:
			break;
		
		case Result::STRING:
			rstring = (ResultString*) result;
			str = rstring->getValue();
			length += sizeof(unsigned long) + str.length() + 1;
			break;
					
		case Result::ERROR:
			length += sizeof(unsigned long);
			break;

		case Result::INT:
			length += sizeof(unsigned long);
			break;
		
		case Result::BOOL:
			length += sizeof(unsigned long);
			break;		
		
		case Result::BOOLTRUE:
			length += sizeof(unsigned long);		
			break;
		
		case Result::BOOLFALSE:
			length += sizeof(unsigned long);		
			break;
		
		case Result::DOUBLE:
		        //length += sizeof(double);
		        length += 2*sizeof(unsigned long);
			break;
		
		case Result::BINDER:
			rbinder = (ResultBinder*) result;
			length += getStringLength(rbinder->getName());
			length += getResultLength(rbinder->getValue());
			break;
			
		default:
			cerr << "<ParamStatementPackage::getResultLength> obiekt nieznany, nr: " << result->getType() << endl;
			return 0; //TODO
			
		} // switch

		return length;
	}

	/** Serializeing result */
	int ParamStatementPackage::setResultOld(Result* result) {
		char* tmpPtr = bufferBegin + sizeof(unsigned long);
		if (tmpPtr > bufferEnd) {
			return -2; //TODO proba czytania poza buforem
		}

		// serializeng Result		
		setULong(Result::VOID);
	
		bufferBegin = tmpPtr;
		return 0;
	}

	
	int ParamStatementPackage::setResultCollection(ResultCollection* collection) {
		setULong(collection->size());
		int i;
		for(i=0;i < collection->size(); i++) {
			Result* result = collection->get(i);
			setResult(result);
		}
		return 0;	
	}
	
	/** Serializing result */
	int ParamStatementPackage::setResult(Result* result) {
	        //		ResultSequence* rsequence;
		//		ResultBag*		rbag;
		ResultStruct	rstruct;
		ResultBinder*   rbinder;
		ResultString*   rstring;
  		ResultInt*      rint;
  		ResultDouble*   rdouble;
		ResultBool*     rbool;
  		ResultReference* rref;
  		//ResultVoid*     rvoid;
  		ResultError*    rerror;
		
		char* tmpPtr = bufferBegin + getResultLength(result);
		if (tmpPtr > bufferEnd) {
			return -2; //TODO proba czytania poza buforem
		}

		setULong(result->getType());
		
		
		
		switch (result->getType()) {
		case Result::BAG:
			setResultCollection((ResultCollection*) result);
			break;
			
		case Result::SEQUENCE:
			setResultCollection((ResultCollection*) result);
			break;
			
		case Result::STRUCT:
			setResultCollection((ResultCollection*) result);
			break;
			
		case Result::REFERENCE:
			rref = (ResultReference*) result;
			setString(rref->getValue());
			break;
		
		case Result::VOID:
			break;
		
		case Result::STRING:
			rstring = (ResultString*) result;
			setString(rstring->getValue());
			break;
		
		case Result::ERROR:
			rerror = (ResultError*) result;
			setULong(rerror->getCode());
			break;
			
		case Result::INT:
			rint   = (ResultInt*) result;
			setULong(rint->getValue());
			break;
		
		case Result::BOOLTRUE:
		        setULong(0);
			break;
		
		case Result::BOOLFALSE:
		        setULong(1);
			break;
			
		case Result::BOOL:
		        rbool  = (ResultBool*) result;
			if (rbool->getValue() == true)
			  setULong(0);
			else
			  setULong(1);
			break;
		case Result::DOUBLE:
			rdouble = (ResultDouble*) result;
			setDouble(rdouble->getValue());
			break;
		
		case Result::BINDER:
			rbinder = (ResultBinder*) result;
			setString(rbinder->getName());
			setResult(rbinder->getValue());
			break;
			
		default:
			cerr << "<Connection::deserialize> obiekt nieznany, nr: " << result->getType() << endl;
			return -1;
		} // switch
		
	
		//bufferBegin = tmpPtr;
		return 0;
	}

	int ParamStatementPackage::getQueryResultCollection(QueryResult* collection) {
		QueryResult*		 result;
		QuerySequenceResult*     qsequence;
		QueryBagResult*		 qbag;
		QueryStructResult*	 qstruct;
		
		unsigned long length, i;
		getULong(&length);

		// cerr << "col ln: " << length << endl;

		for(i=0; i < length; i++) {
			getQueryResult(&result);
			switch(collection->type()) {
			case Result::SEQUENCE:
				qsequence = (QuerySequenceResult*) collection;
				qsequence->addResult(result);
				break;
			case Result::BAG:
				qbag = (QueryBagResult*) collection;
				qbag->addResult(result);
				break;
			case Result::STRUCT:
				qstruct = (QueryStructResult*) collection;
				qstruct->addResult(result);
				break;
			default:
				return -2;
			}
		}
		return 0;
	}
	
	
	int ParamStatementPackage::getQueryResult(QueryResult** result) {
		// declarations
	    QueryStringResult *qstring;
	    //QueryBagResult *qbag;
	    QueryReferenceResult *qref;
	    QueryIntResult *qint;
	    QueryBoolResult *qbool;
   	    QueryDoubleResult *qdouble;
	    QueryBinderResult *qbinder;
		unsigned long type;
		string        str;
		unsigned long lng;
		double		  dbl;
		QueryResult*  tmpResult;
		
		// deserializing
		getULong(&type);
		// cerr << "[ParamStatementPackage::getQueryResult]" << " type: " << type << endl; 

		
		switch(type) {
		case Result::BAG:	
			*result = new QueryBagResult();
			getQueryResultCollection(*result);
			break;
			
		case Result::SEQUENCE:
			*result = new QuerySequenceResult();
			getQueryResultCollection(*result);
			break;
			
		case Result::STRUCT:
			*result = new QueryStructResult();
			getQueryResultCollection(*result);			
			break;
			
		case Result::REFERENCE:
			//	DBLogicalID
			*result = new QueryReferenceResult();
			qref	= (QueryReferenceResult*) *result;
			int id;
			str.clear();
			getString(&str);
			sscanf(str.c_str(), "%d", &id);
			qref->setValue(new DBLogicalID(id));
			break;
			
		case Result::STRING:
			*result = new QueryStringResult();
			qstring = (QueryStringResult*) *result;
			str.clear();
			getString(&str);
			qstring->setValue(str);
			break;
			
		case Result::VOID:
			*result = new QueryNothingResult();
			break;
			
		case Result::INT:
			*result = new QueryIntResult();
			qint    = (QueryIntResult*) *result;
			getULong(&lng);
			qint->setValue(lng);
			break;
			
		case Result::DOUBLE:
			*result = new QueryDoubleResult();
			qdouble = (QueryDoubleResult*) *result;
			getDouble(&dbl);
			qdouble->setValue(dbl);
			break;
		
		case Result::BOOLTRUE:
		case Result::BOOLFALSE:
		case Result::BOOL:
			*result = new QueryBoolResult();
			qbool = (QueryBoolResult*) *result;
			getULong(&lng);
			if (lng == 0) qbool->setValue(true);
			else qbool->setValue(false);
			break;
		
		case Result::BINDER:
			*result = new QueryBinderResult();
			qbinder = (QueryBinderResult*) *result;
			str.clear();
			getString(&str);
			getQueryResult(&tmpResult);
			qbinder->setName(str);
			qbinder->setItem(tmpResult);
			break;

		case Result::RESULT:
			return -2;		
		default:
			return -2;
		

			//Error
		}
		
		return 0;
	}
	
        string ParamStatementPackage::toString() {
	  string output;
	  stringstream oss;

	  oss << *this;
	  output = oss.str();

	  return output;
	}
	
	ostream& operator<<(ostream& os, ParamStatementPackage& psp) {
		
		map<string, Result*>::iterator it;
		it = psp.params.begin();	
		os << "Result params: " << endl;
		while (it != psp.params.end()) {
			string paramName = (*it).first;
			os << "name:" << paramName << endl;
			
			Result* paramResult = (*it).second;
			os << *paramResult;
			it++;
			if (it != psp.params.end()) 
				os << endl;
		}
		os << endl;

		map<string, QueryResult*>::iterator qit;
		qit = psp.queryParams.begin();	
		os << "QueryResult params: " << endl;
		while (qit != psp.queryParams.end()) {
			string paramName = (*qit).first;
			os << "name:" << paramName << endl;
			
			QueryResult* paramResult = (*qit).second;
			os << paramResult->toString(0,true,"");

			qit++;
			if (qit != psp.queryParams.end()) 
				os << endl;
		}
		os << endl;
		return os;
	}
}

