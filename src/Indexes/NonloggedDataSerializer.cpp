#include <cassert>
#include <Errors/Errors.h>
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>

#include <Indexes/NonloggedDataSerializer.h>
#include <Indexes/IndexHandler.h>
#include <Indexes/IndexManager.h>
#include <Indexes/VisibilityResolver.h>


namespace Indexes {
using namespace std;

	NonloggedDataSerializer::NonloggedDataSerializer() {
		meta_fd = -1;
	}
	
	NonloggedDataSerializer::~NonloggedDataSerializer() {
		if (meta_fd != -1) {
			::fsync(meta_fd);
			::close(meta_fd);
			meta_fd = -1;
		}
	}
	
	int NonloggedDataSerializer::writeBuf(char* buf, int /*fd*/, ssize_t toWrite) {
		ssize_t count;
		while (toWrite > 0) {
			count = write(meta_fd, buf, toWrite);
			//	jesli read > 0 lub errno == EINTR to wszystko ok. trzeba ponowic petle
			if ((count < 0) && (errno != EINTR)) {
				cout << "write error" << endl;
				return errno | ErrIndexes;
			}
			toWrite -= count;
			buf += count;
		}
		return 0;
	}
	
	int NonloggedDataSerializer::readBuf(char* buf, int /*fd*/, ssize_t toRead) {
		ssize_t count;
		while (toRead > 0) {
			count = read(meta_fd, buf, toRead);
			//	jesli count > 0 lub errno == EINTR to wszystko ok. trzeba ponowic petle
			if ((count <= 0) && (errno != EINTR)) {
				if (count < 0) {
					return errno | ErrIndexes;
				} else {
					//EOF? impossible. blad krytyczny
					//assert(false);
					return errno | ErrIndexes;
				}
			}
			toRead -= count;
			buf += count;
		}
		return 0;
	}
			
	template<class T>
	int NonloggedDataSerializer::serializeValue(T value) {
		int size = sizeof(T);
		char* buf = (char*)malloc(size);
		if (!buf) {
			return ENOMEM | ErrIndexes;
		}
		*((T*)(buf)) = value;
		int err = writeBuf(buf, meta_fd, size);
		free(buf);
		return err;
	}
	
	template<class T>
	int NonloggedDataSerializer::deserializeValue(T &value) {
		int size = sizeof(T);
		int err = 0;
		char* buf = (char*)malloc(size);
		if (!buf) {
			return ENOMEM | ErrIndexes;
		}
		err = readBuf(buf, meta_fd, size);
		value = *((T*)buf);
		free(buf);
		return err;
	}
	
	int NonloggedDataSerializer::serialize(string &indexMetadataFile) {
		int err = 0;
		if( ( meta_fd = ::open( indexMetadataFile.c_str(), O_RDWR | O_CREAT, S_IWUSR | S_IRUSR ) ) < 0 ) {
			return errno | ErrIndexes;
		}
		
		if ((err = ftruncate(meta_fd, 0))) {
			return err | ErrIndexes;
		}
		//parametry
		if ((err = serializeValue(IndexHandler::nextFreeNodeAddr))) {
			return err;
		}
		err = serializeValue(IndexHandler::droppedNode);
		err = serializeValue(IndexHandler::droppedLastNode);
		
		//dodatkowe parametry indeksow
		IndexManager::string2index::iterator it;
		IndexManager* im = IndexManager::getHandle();
		int i =0;
		for (it = im->indexNames.begin(); it != im->indexNames.end(); it++, i++) {
			err = serializeValue(it->second->lid->toInteger());
			err = serializeValue(it->second->tree->rootAddr);
			err = serializeValue(it->second->tree->rootLevel);
			err = serializeValue(it->second->firstNode);
			err = serializeValue(it->second->lastNode);
		}
		
		//mapa transakcji rollbackowanych
		VisibilityResolver* v = IndexManager::getHandle()->resolver.get();
		unsigned int size = v->rolledBack.size();
		err = serializeValue(size);
		VisibilityResolver::rolledBack_t::const_iterator it2;
		VisibilityResolver::rolledBackIndex_t::const_iterator rit;
		for(it2 = v->rolledBack.begin(); it2 != v->rolledBack.end(); it2++) {
			serializeValue(it2->first); //lid
			serializeValue(it2->second.size());
			for (rit = it2->second.begin(); rit != it2->second.end(); rit++) {
				serializeValue(rit->first); //tid
				serializeValue(rit->second); //count
			}
		}
		return 0;
	}
	
	int NonloggedDataSerializer::deserialize(string &indexMetadataFile) {
		int err = 0; 
		if( ( meta_fd = ::open( indexMetadataFile.c_str(), O_RDWR, S_IWUSR | S_IRUSR ) ) == -1 ) {
			if (errno == ENOENT) {
				//nie ma tego pliku - pierwsze uruchomienie, zostana ustawione wartosci domyslne
				return 0;
			}
			return errno | ErrIndexes;
		}
		
		struct stat state;
		if (fstat(meta_fd, &state)) {
			return errno | ErrIndexes;
		}
		if (state.st_size == 0) {
			return EBadFile | ErrIndexes;
		}
		
		//parametry
		err = deserializeValue(IndexHandler::nextFreeNodeAddr);
		err = deserializeValue(IndexHandler::droppedNode);
		err = deserializeValue(IndexHandler::droppedLastNode);
		
		//dodatkowe parametry indeksow
		//najpierw musi byc wczytana lista indksow ze store'a
		int id;
		IndexManager* im = IndexManager::getHandle();
		IndexHandler* ih;
		int rootAddr, rootLevel, firstNode;
		for (unsigned int i = 0; i < im->indexNames.size(); i++) {
			deserializeValue(id);
			
			ih = im->indexLid[id];
			if (!ih) {
				return EMetaIncorrect | ErrIndexes;
			}
			
			deserializeValue(rootAddr);
			deserializeValue(rootLevel);
			deserializeValue(firstNode);
			deserializeValue(ih->lastNode);
			
			ih->firstNode = firstNode;
			ih->tree->init(rootAddr, rootLevel);
		}
		im->indexLid.clear(); //juz nie bedzie potrzebne
		//mapa transakcji rollbackowanych
		unsigned int size, count, innerSize;
		tid_t tid;
		lid_t lid;
		VisibilityResolver::rolledBackIndex_t tmpMap;
		deserializeValue(size);
		for (unsigned int i = 0; i < size; i++) {
			err = deserializeValue(lid);
			err = deserializeValue(innerSize);
			tmpMap.clear();
			for (unsigned int i = 0; i < innerSize; i++) {
				err = deserializeValue(tid);
				err = deserializeValue(count);
				tmpMap[tid] = count;
			}
			im->resolver->rolledBack[lid].swap(tmpMap);
		}
		return 0;
	}
}
