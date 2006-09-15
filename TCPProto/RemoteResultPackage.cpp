#include <errno.h>
#include <netinet/in.h> 
#include "Package.h"
#include "TCPParam.h"

namespace TCPProto {
	
	RemoteResultPackage::RemoteResultPackage(TCPParam* param) {
		server = param->getServer();
		port = param->getPort();
	}
	
	int RemoteResultPackage::deserialize(char* buffer, int size) {
		cerr << "zaczynam remote result";
	 	if (buffer[0] != getType()) return -1; //TODO
		bufferBegin = buffer+1;
		bufferEnd = buffer+size;
		int error;
		QueryResult* r;
		error = dataDeserialize(&r);
		if (error != 0) {
			if (r != NULL) delete r; //a part of tree was created
			return error;
		}
		tmpRes = r;
		free (buffer);
		return 0;
	 }
	
	QueryResult* RemoteResultPackage::getQueryResult() {
		return tmpRes;
	}
	
	int RemoteResultPackage::toInt(int* result, string from) {
		errno = 0;
		char *last;
		*result = strtol(from.c_str(), &last, 10);
		if (*last != 0 || errno != 0) {
			*ec << "konwersja string -> int nie powiodla sie";
			return 1; //TODO
		}
		return 0;
	}
	
	int RemoteResultPackage::stringCopy(char* &newBuffer) { // od bufferBegin
		int len = strlen(bufferBegin)+1; //including NULL
		
		if (bufferBegin + len > bufferEnd) {
			return -2; //TODO error czytanie poza buforem
		}
		
		newBuffer = (char*)malloc (len);
		
		if (NULL == newBuffer) {
			return (errno | ErrDriver);
		}
		strcpy (newBuffer, bufferBegin); // always succeeds
		bufferBegin += len;
		return 0;
}
	
	int RemoteResultPackage::getULong(unsigned long &val) {
		
		char* tmpPtr = bufferBegin + sizeof(long);
		
		if (tmpPtr > bufferEnd) {
			return -2; //TODO proba czytania poza buforem
		}
		
			val = ntohl(*((unsigned long*) bufferBegin));
			bufferBegin = tmpPtr; //skip the number of elements (long)
			return 0;
	}
	
	int RemoteResultPackage::grabElements(QueryResult* col) {
		
		unsigned long number;
		int error;
		cerr << "zaczynam remote grab elements";
		if (0 != (error = getULong(number))) return error;
		cerr << "numer wczytany";
		QueryResult* r;
//		col->setSize(getULong(number)); //by reference
		for (unsigned long i = 1; i <= number; i++) {
			cerr << "zaczynam kolejny element kolekcji";
			if (0 != (error = dataDeserialize(&r))) return error;
			cerr << "kolejny element kolekcji zostal wczytany";
			
			switch (col->type()) {
			case QueryResult::QBAG:
				((QueryBagResult*) col)->addResult(r);
				break;
			case QueryResult::QSTRUCT:
				((QueryStructResult*) col)->addResult(r);
				break;
			case QueryResult::QSEQUENCE:
				((QuerySequenceResult*) col)->addResult(r);
				break;
			default:
				return -1;//TODO
			}
			
		}
		
		return 0;
}
	
	int RemoteResultPackage::dataDeserialize(QueryResult** r ) {
//		int error;
//		ResultBag *brs;
//		Result* result;	
		char* id;
		int error;
		unsigned long number;
//		unsigned long i;
		unsigned long* lptr;
		double db;
		char df;
		LogicalID *lid;
		//printf("dataDeserialize: buffer[0], buffer[1], buffer[2], buffer[3], buffer[4] = |%d|%d|%d|%d|%d|\n",(int)bufferBegin[0], (int)bufferBegin[1], (int)bufferBegin[2], (int)bufferBegin[3], (int)bufferBegin[4]);	
		
		switch (*(bufferBegin++)) {
		
			case Result::BAG:
				cerr << "<Connection::deserialize> tworze obiekt BAG\n";
				*r = new QueryBagResult();
				return grabElements(*r);
				
			case Result::SEQUENCE:
				cerr << "<Connection::deserialize> tworze obiekt SEQUENCE\n";
				*r = new QuerySequenceResult();
				return grabElements(*r);
				
			case Result::STRUCT:
				cerr << "<Connection::deserialize> tworze obiekt STRUCT\n";
				*r = new QueryStructResult();
				return grabElements(*r);
					
			case Result::REFERENCE:
				cerr << "<Connection::deserialize> tworze obiekt REFERENCE w remote response\n";
				error = stringCopy(id); //by reference
				cerr << "wyciagnalem stringa\n";
				if (error != 0) return error;
				int value;
				error = toInt(&value, string(id));
				cerr << "RemoteResultPackage :: odebralem zdalna referencje: " << value << endl; 
				if (error != 0) return error;
				lid = new DBLogicalID();
				lid->setRemoteID(new DBLogicalID(value));
				lid->setServer(server);
				lid->setPort(port);
				*r = new QueryReferenceResult(lid);
				return 0;
			
			case Result::VOID:
				cerr << "<Connection::deserialize> tworze obiekt VOID\n";
				*r = new QueryNothingResult();
				return 0;
			
		    case Result::STRING:
		    	cerr << "<Connection::deserialize> tworze obiekt STRING\n " ;
				error = stringCopy(id); // by reference
				if (error != 0) return error;
				*r = new QueryStringResult(string (id));
				return 0;
			
			case Result::ERROR:
				cerr << "<Connection::deserialize> tworze obiekt ERROR\n";
				error = getULong(number); //by reference
				if (error != 0) return error;
				return number;
				
			case Result::INT:
				cerr << "<Connection::deserialize> tworze obiekt INT\n";
				error = getULong(number); //by reference
				if (error != 0) return error;
				*r = new QueryIntResult((int) number);
				return 0;
			
			case Result::BOOLTRUE:
				cerr << "<Connection::deserialize> tworze obiekt BOOL (true)\n";
				*r = new QueryBoolResult(true);
				return 0;
			
			case Result::BOOLFALSE:
				cerr << "<Connection::deserialize> tworze obiekt BOOL (false)\n";
				*r = new QueryBoolResult(false);
				return 0;
			
			case Result::DOUBLE:
				cerr << "<Connection::deserialize> tworze obiekt DOUBLE\n";
				lptr = (unsigned long*) &db;
				error = getULong(*lptr); //higher word
				if (error != 0) return error;
				error = getULong(*(lptr+1)); //lower word
				if (error != 0) return error;
				*r = new QueryDoubleResult(db);
				return 0;
			
			case Result::BINDER:
				cerr << "<Connection::deserialize> tworze obiekt BINDER\n";
				error = stringCopy(id); // by reference
				if (error != 0) return error;
				QueryResult* subRes;
				error = dataDeserialize(&subRes);
				if (error != 0) return error;
				*r = new QueryBinderResult(string(id), subRes);
				return 0;
				
			default:
				df = *(bufferBegin-1);
				cerr << "<Connection::deserialize> obiekt nieznany, nr: " << (int) df << endl;
				return -1; //TODO
		} // switch
	} // deserialize
}
