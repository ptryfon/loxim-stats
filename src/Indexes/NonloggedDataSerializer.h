#ifndef LOXIMDATABRIDGE_H_
#define LOXIMDATABRIDGE_H_

#include "Store/Store.h"
#include "CachedNode.h"
#include "TransactionManager/Transaction.h"
#include "Errors/Errors.h"

namespace Indexes {

	class NonloggedDataSerializer {
		private:
			//LogicalID *rootLid;//, *indexLid, *rolledBackLid, *paramLid;
			//DataValue *rolledBackDv;
			
			/** deskryptor pliku z metadanymi */
			int meta_fd;
			//properties_t property
		public:
			
			typedef string key_t;
			typedef pair<DataValue*, LogicalID*> value_t;
			
			typedef map< key_t, value_t > properties_t;
			typedef pair< key_t, value_t > property_t;
			
			/** inicjalizacja wszystkich lidow */
			static void startup();
			static void shutdown();
			
			int writeBuf(char* buf, int fd, ssize_t toWrite);
			int readBuf(char* buf, int fd, ssize_t toRead);
			
			/** deserialuje kolejna wartosc typu T */
			template<class T>
			int deserializeValue(T &value);
			
			/** serialuje kolejna wartosc typu T */
			template<class T>
			int serializeValue(T value);
			
			/** serialuje wszystkie metadane */
			int serialize(string &indexMetadataFile);
			
			/** deserialuje wszystkie metadane. najpierw lista indeksow musi byc wczytana ze store'a */
			int deserialize(string &indexMetadataFile);
			
			NonloggedDataSerializer();
			virtual ~NonloggedDataSerializer();
	};
	
}

#endif /*LOXIMDATABRIDGE_H_*/
