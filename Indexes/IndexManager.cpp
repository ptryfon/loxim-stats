#include "IndexManager.h"

//#include <stdio.h>

#define indexListParamName	"index_file_list"
#define indexDataParamName	"index_file_data"
#define indexLockParamName	"index_lock_file"

#define descInitValue		-1

namespace Indexes
{
	
	IndexManager* IndexManager::handle = NULL;
	ErrorConsole* IndexManager::ec = NULL; 
	SBQLConfig IndexManager::config("IndexManager");
	string IndexManager::indexLockFile = "";
	string IndexManager::listFileName = "";
	
	
	int IndexManager::lockDesc = descInitValue;
	int IndexManager::listDesc = descInitValue;
	int IndexManager::dataDesc = descInitValue;

	int IndexManager::init(bool cleanShutdown) {
		ec = new ErrorConsole("IndexManager");
		
		handle = new IndexManager();
		int err = handle->instanceInit(cleanShutdown);
		if (err != 0) {
			shutdown();
			return err;
		}
		*ec << "started";		
		return 0;
	}
	
	int IndexManager::shutdown() {
		int err;
		if (handle == NULL) {
			return -1; //TODO IndexManager is already closed
		}
		
		*ec << "shutting down...";
		
		//delete lock
		
		err = handle->finalize();
		
		delete handle;
		handle = NULL;
		
		*ec << "closed";
		delete ec;
		return err;	
	}
	
	int IndexManager::fileOpen(string paramName, int &descriptor) {
		
		string value;
		
		int err;
		
		if ((err = getFileName(paramName, value))) {
			return err;
		}
  		
  		if( ( descriptor = ::open( value.c_str(), O_RDWR | O_CREAT, S_IWUSR | S_IRUSR ) ) < 0 ) {
			ec->printf("Cannot open file %s\n", value.c_str());
			//TODO dorzucic kod bledu indexManageru
			return errno;
		}
		return 0;
	}
	
	int IndexManager::getFileName(string paramName, string &value) {
		
		int errCode;
		
		if( ( errCode = config.getString( paramName, value ) ) ) {
  			ec->printf("Cannot read %s from configuration file\n", paramName.c_str());
  			return errCode;
  		}
  		
  		return 0;
	}
	
	int IndexManager::closeFiles(int errCode) {
		int err;
		if (lockDesc != descInitValue) {
			if ((err = close(lockDesc))) {
				ec->printf("Error closing index lock file. Errorcode: %d\n", err);
			}
		}
		if (dataDesc != descInitValue) {
			if ((err = close(dataDesc))) {
				ec->printf("Error closing index data file. Errorcode: %d\n", err);
			}
		}
		if (listDesc != descInitValue) {
			if ((err = close(listDesc))) {
				ec->printf("Error closing index list file. Errorcode: %d\n", err);
			}
		}
		
		return errCode;
	}
	
	int IndexManager::instanceInit(bool cleanShutdown) {
		int errCode = 0;

  		SBQLConfig* config = new SBQLConfig( "IndexManager" );
  		
  		if( ( errCode = config->getString( indexLockParamName, indexLockFile ) ) ) {
  			ec->printf("Cannot read %s from configuration file\n", indexLockParamName);
  			return closeFiles(errCode);
  		}
  		
  		if ((errCode = getFileName(indexListParamName, listFileName))) {
  			return closeFiles(errCode);
  		}
  		
		if ((errCode = fileOpen(indexDataParamName, dataDesc))) {
			return closeFiles(errCode);
		}
		
		loadIndexList(listFileName);
		
		if (!cleanShutdown) {
			ec->printf("Shutdown wasn't clean. Rebuilding indexes...\n");
			
			ec->printf("Indexes rebuilding done\n");
		}
		
		//jesli jest lock to nie bylo czyste zamkniecie -> skasuj zawartosc indeksu, utworz nowa na podstawie listy indeksow, wyswietl komunikat
		
		//zaloz lock
		return 0;
	}
	
	int IndexManager::loadIndexList(string &fileName) {
		ifstream file(fileName.c_str(), ios::in);
		if (!file.is_open()) {
			ec->printf("Cannot open or create file %s", fileName.c_str());
			//TODO dorzucic blad indexow
			return ENoFile;
		}
		
		string indexName, rootName, fieldName; 
		
		ec->printf("wczytuje indeksy\n");
		while (file >> indexName >> rootName >> fieldName) {
			cout << "istniejace indexy: " << indexName << " ON " << rootName << "." << fieldName << endl;
		}
		ec->printf("zakonczono wczytywanie indeksow\n");
		
		//destruktor file zamyka plik
		return 0;
	}
	
	int IndexManager::finalize() {
		return 0;
	}
	
	IndexManager::IndexManager() {}
	
	IndexManager* IndexManager::getHandle() {
		return handle;
	}
	
	IndexManager::~IndexManager() {}
	
	/*int IndexManager::execute(IndexNode* node) {
		node->execute();
		return 0;
	}*/
	
	int IndexManager::createIndex(string indexName, string indexedRootName, string indexedFieldName, QueryResult **result) 
	{
		int err = 0;
		*ec << "creating index";
		
		ofstream fileList(listFileName.c_str(), ios::app);
		if (!fileList.is_open()) {
			ec->printf("Cannot open file: %s\n", listFileName.c_str());
			return ENoFile; //TODO dodac IM
		}
		
		fileList << indexName << " " << indexedRootName << " " << indexedFieldName << endl;
		
		if (!fileList) {
			ec->printf("Cannot write to a file: %s. File can be corrupted\n", listFileName.c_str());
			err = ENoFile; //TODO dodac
		}
	
		fileList.close();
		
		*result = new QueryNothingResult();
		return err; //destruktor fileList zamknie plik
	}

	int IndexManager::listIndex(QueryResult **result)
	{
		*ec << "listing indexes";
		*result = new QueryNothingResult();
		return 0;	
	}

	int IndexManager::dropIndex(string indexName, QueryResult **result) {
		*ec << "dropping index";
		*result = new QueryNothingResult();
		return 0;
	}
}
