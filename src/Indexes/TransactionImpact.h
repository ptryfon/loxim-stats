#ifndef TRANSACTIONIMPACT_H_
#define TRANSACTIONIMPACT_H_

#include <Indexes/const.h>
#include <set>
#include <map>

using namespace std;
namespace Indexes {

	class TransactionImpact {
		public:
			
			typedef set<tid_t> finishedBefore_t;
			
			TransactionImpact();
			virtual ~TransactionImpact();
			TransactionImpact* getNext();
			TransactionImpact* getPrev();
			
			/** dodaje kolejna transakcje zakonczona tuz przed aktualna */
			void addFinished(tid_t id);
			
			/** wstawia nowa transakcje tuz za aktualna */
			void insertAfter(TransactionImpact* tr);
			
			/** zwraca horyzont dla tej transakcji - ostatnia transakcje ktora zakonczyla sie przed rozpoczeciem tej */
			tid_t getHorizon();
			
			/** zwraca zbior wszystkich transakcji ktore zakonczyly sie przed rozpoczeciem tej ale po rozpoczeciu poprzedniej */
			finishedBefore_t* getFinishedBefore();
			
			/** wiedza o zakonczonych transakcjach nie jest juz potrzebna */
			void forgetFinishedBefore();
			
			/** usuwa te transakcje z listy */
			void remove();
			
			/** ile wpisow dana transakcja ma we wszystkich drzewach */
			typedef map<lid_t, unsigned long> impact_t; 
			impact_t entryCounter; 
			
		private:
			/** nastepna i poprzednia transakcja na liscie */
			TransactionImpact *next, *prev;
			
			/** zbior transakcji zakonczonych przed rozpoczeciem tej ale po rozpoczeciu poprzedniej */
			finishedBefore_t* finishedBefore;
			tid_t horizon;
			
			
	};

}

#endif /*TRANSACTIONIMPACT_H_*/
