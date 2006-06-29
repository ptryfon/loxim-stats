#include <netinet/in.h> 
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
			length += sizeof(unsigned long); //stringLength
			length += sizeof(char) * (paramName.length() + 1);
			//TODO: serializing paramResult
			Result* paramResult = (*it).second;
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
				
		for (unsigned long i=0; i < paramCount; i++) {
			string paramName;
			getString(&paramName);
			QueryResult* paramResult;
			getQueryResult(&paramResult);
			queryParams[paramName] = paramResult;
		}
		
		free(buffer);

		//cerr << *this ;
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
	
	int ParamStatementPackage::getQueryResult(QueryResult** result) {	
		//TODO:
		bufferBegin += sizeof(unsigned long);
		
		*result = new QueryBagResult();
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
	
	
	int ParamStatementPackage::setResult(Result* result) {
		char* tmpPtr = bufferBegin + sizeof(unsigned long);
		if (tmpPtr > bufferEnd) {
			return -2; //TODO proba czytania poza buforem
		}
		
		setULong(Result::VOID);
		bufferBegin = tmpPtr;
		
		return 0;
	}
	
	ostream& operator<<(ostream& os, ParamStatementPackage& psp) {
		os << "ParamStatementPackage" << endl;
		map<string, Result*>::iterator it;
		it = psp.params.begin();	
		while (it != psp.params.end()) {
			string paramName = (*it).first;
			os << "param: " << paramName << " ";
			
			Result* paramResult = (*it).second;
			os << *paramResult << endl;
			it++;
		}
		
		map<string, QueryResult*>::iterator qit;
		qit = psp.queryParams.begin();	
		while (qit != psp.queryParams.end()) {
			string paramName = (*qit).first;
			os << "param: " << paramName << " ";
			
			// QueryResult* paramResult = (*qit).second;
			// os << *paramResult;
			os << endl;
			qit++;
		}
		return os;
	}
}

