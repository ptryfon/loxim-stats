#include<stdio.h>

/**
 *	TransactionID - obiekt jednoznacznie identyfikujacy transakcje
 */
class TransactionID
{      
      public:
                TransactionID(int id)
                {
					printf("TransactionID");
                };
};

/**
 *   Transaction - obiekt w ramach ktorego dziala Executor
 */
class Transaction
{
	  private:
		TransactionID* tid;

      public:
                Transaction(TransactionID* tId);
                {
					printf("Transaction");
					this->tid = tId;
                };
                
                Proxy* getProxy(LogicalID lid, AccessMode mode)
				{
					printf("Transaction: getProxy");
					LockManager::lock(lid, *(tid), mode);
					return ObjectStore::getProxy( tid, lid, mode );
				};
}; 

/**
 *	TransactionManager - fabryka obiektow Transaction, kazdy z Executor'ow prosi go o nowa transakcje
 */
class TransactionManager
{
      public:
      static Transaction* createTransaction()
             {
				 printf("TransactionManager");
                 return new Transaction( new TransactionID(1) );            
             }              
};

/**
 *	LockManager - implementuje tablice zamkow, wywolywany przez dana Transakcje 
 */
class LockManager
{ 
      public:      
      static int lock(LogicalID lid, TransactionID tid, AccessMode mode)
      {
			 printf("LockManager");
             return 0;
      };
};
