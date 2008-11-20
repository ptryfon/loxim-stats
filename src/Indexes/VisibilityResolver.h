#ifndef VISIBILITYRESOLVER_H_
#define VISIBILITYRESOLVER_H_

#include <map>
#include <Indexes/const.h>
#include <Indexes/TransactionImpact.h>
#include <TransactionManager/Mutex.h>

using namespace std;
namespace Indexes {

	enum TransactionEntryStatus {ACTIVE_ENTRY, ROLLED_BACK_ENTRY};

	class VisibilityResolver {
		public:
			VisibilityResolver();
			virtual ~VisibilityResolver();
			
			/** czy zmiany zrobione przez transakcje from sa widoczne w transakcji ktora sie rozpoczela w chwili begin i nadal jest aktywna
			 *  from nie moze byc transakcja rozpoczynajaca sie w chwili begin
			 *  */
			bool isVisible(tid_t from, ts_t begin, lid_t index);
			
			/** rozpoczecie transakcji */
			void begin(tid_t id);
			
			/** wycofanie transakcji */
			void abort(tid_t id);
			
			/** zacommitowanie transakcji */
			void commit(tid_t id, ts_t timestamp);
			
			/** czy transakcja jest wcofana */
			bool isRolledBack(tid_t tid, lid_t index);
			
			/** czy transakcja jest poza horyzontem
			 *  ale nie sprawdza czy zacommitowana albo rollbackowana
			 * */
			bool isBeyondHorizon(tid_t tid);
			
			/** po usunieciu indexu usuwa informacje o wycofanych wpisach dla tego indeksu */
			void indexDropped(lid_t index);
			
			/** z indeksu wyrzucono wpis transakcji tid, trzeba zmnieszyc licznik zmian */
			void entryDropped(tid_t tid, lid_t index, TransactionEntryStatus status);
			
			/** do indeksu dodano wpis transakcji tid, trzeba zwiekszyc licznik zmian */
			void entryAdded(tid_t tid, lid_t index); 
			
		private:
			//zmniejszane gdy wszystkie wpisy znikna z drzewa
			typedef map<tid_t, unsigned long> rolledBackIndex_t;
			typedef map<lid_t, rolledBackIndex_t > rolledBack_t; 
			rolledBack_t rolledBack;
			
			//zmniejszane gdy commit lub abort
			typedef map<tid_t, TransactionImpact*> active_t;
			active_t active;
			
			/** czasy zacommitowania transakcji. zmniejszane gdy transakcja wyjdzie poza horyzont */
			typedef map<tid_t, ts_t> commitTs_t;
			commitTs_t commitTs;
			
			/** synchronizuje dostep do map */
			SemaphoreLib::Mutex* access;
			
			/** transakcjie o id <= niz horizon nie maja wplywu na zadna obecna transakcje. zaostaly zakonczone przed jakakolwiek aktywna transakcja */
			tid_t horizon;
			
			/** wykonuje operacje po zakonczeniu transakcji. bez wzgledu na to czy to byl commit czy rollback
			 *  usuwa obiekt tr */
			void finish(TransactionImpact* tr);
			
			/** lista aktywnych transakcji i zaleznosci miedzy nimi */
			TransactionImpact* tail;
			
			/** przygotowuje transakcje id do zakonczenia
			 *  zwraca obiekt TransactionImpact dla transakcji id,*/
			TransactionImpact* prepare2finish(tid_t id);
			
			/** ilosc transakcji na liscie */
			//int listLength;
			
			friend class Tester;
			friend class NonloggedDataSerializer;
	};

}

#endif /*VISIBILITYRESOLVER_H_*/
