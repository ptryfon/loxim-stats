#include "LogRecord.h"
#include "LogIO.h"
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "Store/DBStoreManager.h"


int BeginTransactionRecord::modifySetsBackDir(CrashRecovery* cr)
    {
      if(cr->tidsToCommit.find(tid) == cr->tidsToCommit.end())
      {
        (cr->tidsToRollback).erase(tid);//bylo insert chyba bez sensu
        (cr->rollbackedTids).insert(tid);
      }
      return 0;
    } 

int ShutdownRecord::shutDown(CrashRecovery* cr) { cr->shutDown(); return 0;}

WriteRecord::~WriteRecord() { delete lid; if( freeOldValFlag ) delete oldVal; if( freeNewValFlag ) delete newVal; }

int WriteRecord::modifySetsBackDir(CrashRecovery* cr)
    {
      //wrzucamy do wycofania tylko to czego nie ma w commit
      if(cr->tidsToCommit.find(tid) == cr->tidsToCommit.end())
      {
        cr->tidsToRollback.insert(tid);
      }
      return 0;
    }

int WriteRecord::commit(SetOfTransactionIDS* setOfTIDs, StoreManager* sm)
    {
      //sprawdzamy czy zapis dotyczy jednej z tranzakcji do commitowania.
      if(setOfTIDs->find(tid) != setOfTIDs->end())
      {
        //zła postać rekordu w logach
        if(oldVal == NULL && newVal == NULL )
          return LOG_INCORRECT_RECORD_FORMAT;
  
        //Commitowana transakcja stworzyla obiekt, nalezy go odtworzyć.
        if(oldVal == NULL)
          return addToStore(sm, newVal);
  
        //Commitowana transakcja usunela obiekt nalezy go usunac.
        if(newVal == NULL)
          return deleteFromStore( sm, oldVal);
        
        //Commitowana tranzakcja zmienila wartosc na nową - ustawiamy nową wartość  
	//sytuacja nie zachodzi tak nam obiecal store
        return changeValue( sm, newVal);//mozliwe ze mozna add tu store
  
        //return 0;
      }
      return 0;
    }
    
    
   int AddRootRecord::modifySetsBackDir(CrashRecovery* cr)
    {
      //wrzucamy do wycofania tylko to czego nie ma w commit
      if(cr->tidsToCommit.find(tid) == cr->tidsToCommit.end())
      {
        cr->tidsToRollback.insert(tid);
      }
      return 0;
    }
    
int AddRootRecord::commit(SetOfTransactionIDS* setOfTIDs, StoreManager* sm)
    {
      //sprawdzamy czy zapis dotyczy jednej z tranzakcji do zatwierdzenia.
      if(setOfTIDs->find(tid) != setOfTIDs->end())
        //Zatwierdzana tranzakcja dodała roota, więc go dodajemy.
        return addRootToStore( sm );
      return 0;
    }
    
    
int RemoveRootRecord::modifySetsBackDir(CrashRecovery* cr)
    {
      //wrzucamy do wycofania tylko to czego nie ma w commit
      if(cr->tidsToCommit.find(tid) == cr->tidsToCommit.end())
      {
        cr->tidsToRollback.insert(tid);
      }
      return 0;
    }
  
int RemoveRootRecord::commit(SetOfTransactionIDS* setOfTIDs, StoreManager* sm)
    {  
      //sprawdzamy czy zapis dotyczy jednej z tranzakcji do zatwierdzenia.
      if(setOfTIDs->find(tid) != setOfTIDs->end())
        //Zatwierdzana tranzakcja usunęła roota więc go usuwamy.
        return removeRootFromStore( sm );
      return 0;
    }


int CommitRecord::modifySetsBackDir(CrashRecovery* cr)
  {
    cr->tidsToCommit.insert(tid);
    return 0;
  }

int CommitRecord::commit(SetOfTransactionIDS* setOfTIDs, StoreManager* sm)
  {
    //usuwamy tranzakcje ze zbioru tranzakcji do zatwierdzenia
    setOfTIDs->erase(tid);
    return 0;
  }
  
  

int RollbackRecord::modifySetsBackDir(CrashRecovery* cr)
    {
      (cr->tidsToRollback).insert(tid);
      return 0;
    } 

int CkptRecord::ckptStart(CrashRecovery* cr) 
{
  for(unsigned i = 0; i < tidVec->size() ; i++)
  {
    if( (cr->tidsToCommit).find( (*tidVec)[i] ) == (cr->tidsToCommit).end() )
    {
      (cr->tidsToRollback).insert( (*tidVec)[i] );
    }
  }
  cr->CkptStart();
  return 0;
}

int EndCkptRecord::ckptEnd(CrashRecovery* cr) {cr->CkptEnd(); return 0;}



/* LogRecord class */

int LogRecord::initialize( int fd )
{
  idSeq = 0;
  new BeginTransactionRecord();
  new WriteRecord();
  new CkptRecord();
  new EndCkptRecord();
  new CommitRecord();
  new RollbackRecord();
  new ShutdownRecord();
  new AddRootRecord();
  new RemoveRootRecord();

  if( fd )
  {
    LogRecord* lr=NULL;
    int err = 0;

    if( ::lseek( fd,0,SEEK_END ) < 0 ) return errno;
    if( (err = LogRecord::readLogRecordBackward(lr, fd, NULL)) == 0)
    {
      lr->getId( idSeq );
      idSeq++;
    }
    delete lr;
    if( ::lseek( fd,0,SEEK_END ) < 0 ) return errno;
    printf( "IDSeq: %d\n", idSeq );
  }

  return 0;
}

int LogRecord::destroy()
{
  for( map< int, LogRecord* >::iterator it = dictionary.begin(); it != dictionary.end(); it++ )
  {
    delete (*it).second;
  }

  return 0;
}

int LogRecord::readLogRecordForward( LogRecord *&result, int fileDes, StoreManager* sm )
{
  int recordType;
  int errCode;
  off_t filePosBegin, filePosEnd;
  int recordLen;
  int recordId;

  if( ( errCode = LogIO::getFilePos( fileDes, filePosBegin ) ) ) return errCode;
  if( ( errCode = LogIO::readInt( fileDes, recordType ) ) ) return errCode;

  if( !dictionary.count( recordType ) ) return UNKNOWN_LOG_RECORD_TYPE_ERROR;

  switch( recordType ) {
    case BEGIN_LOG_REC_TYPE:
      result = new BeginTransactionRecord();
      break;
    case COMMIT_LOG_REC_TYPE:
      result = new CommitRecord();
      break;
    case ROLLBACK_LOG_REC_TYPE:
      result = new RollbackRecord();
      break;
    case CKPT_LOG_REC_TYPE:
      result = new CkptRecord();
      break;
    case END_CKPT_LOG_REC_TYPE:
      result = new EndCkptRecord();
      break;
    case WRITE_LOG_REC_TYPE:
      result = new WriteRecord();
      break;
    case SHUTDOWN_LOG_REC_TYPE:
      result = new ShutdownRecord();
      break;
    case ADD_ROOT_LOG_REC_TYPE:
      result = new AddRootRecord();
      break;
    case REMOVE_ROOT_LOG_REC_TYPE:
      result = new RemoveRootRecord();
      break;
  }
  //if( ( errCode = dictionary[recordType]->instance( result ) ) ) return errCode;
  if( ( errCode = LogIO::readInt( fileDes, recordId ) ) ) return errCode;
  if( ( errCode = result->read( fileDes, sm ) ) ) return errCode;
  result->setId( recordId );

  if( ( errCode = LogIO::readInt( fileDes, recordLen ) ) ) return errCode;
  if( ( errCode = LogIO::getFilePos( fileDes, filePosEnd ) ) ) return errCode;

  printf("\n[type=%d, begin=%ld,%ld,%d]\n", recordType ,filePosBegin,(filePosEnd - filePosBegin), recordLen );
  if( filePosEnd - filePosBegin != recordLen ) return LOG_RECORD_LENGTH_MISMATCH_ERROR;

  return 0;
}

int LogRecord::readLogRecordBackward( LogRecord *&result, int fileDes, StoreManager* sm )
{
  int errCode;
  int recordLen;
  off_t filePos;

  if( ( errCode = LogIO::getFilePos( fileDes, filePos ) ) ) return errCode;

  // cofamy sie o rozmiar int'a do tylu
  if( ::lseek( fileDes, - sizeof( int ), SEEK_CUR ) < 0 ) return errno;
  if( ( errCode = LogIO::readInt( fileDes, recordLen ) ) ) return errCode;

  // cofamy sie o rozmiar rekordu do tylu
  if( ::lseek( fileDes, - recordLen, SEEK_CUR ) < 0 ) return errno;

  if( ( errCode = readLogRecordForward( result, fileDes, sm ) ) ) return errCode;

  // ustawiamy sie przed odczytanym wlasnie rekordem
  if( ::lseek( fileDes, - recordLen, SEEK_CUR ) < 0 ) return errno;

  return 0;
}

int LogRecord::writeLogRecord( LogRecord *recordPtr, int fileDes )
{
  int errCode;
  off_t filePosBegin, filePosEnd;
  int recordType;
  unsigned int recordId;

  if( ( errCode = recordPtr->getType( recordType ) ) ) return errCode;
  if( ( errCode = recordPtr->getId( recordId ) ) ) return errCode;
  if( ( errCode = LogIO::getFilePos( fileDes, filePosBegin ) ) ) return errCode;

  // zapisujemy typ rekordu
  if( ( errCode = LogIO::writeInt( fileDes, recordType ) ) ) return errCode;

  // zapisujemy id rekordu
  if( ( errCode = LogIO::writeInt( fileDes, (int) recordId ) ) ) return errCode;

  // zapisujemy rekord
  if( ( errCode = recordPtr->write( fileDes ) ) ) return errCode;

  // zapisujemy dlugosc rekordu
  if( ( errCode = LogIO::getFilePos( fileDes, filePosEnd ) ) ) return errCode;
  if( ( errCode = LogIO::writeInt( fileDes, filePosEnd - filePosBegin + sizeof( int ) ) ) ) return errCode;

  return 0;
}


/* TransactionRecord class */

int TransactionRecord::read( int fileDes, StoreManager* sm ) { int result = LogIO::readTransactionID( tid, fileDes ); return result;}

int TransactionRecord::write( int fileDes ) { return LogIO::writeTransactionID( tid, fileDes ); }

/* BeginTransactionRecord class */

int BeginTransactionRecord::rollBack(SetOfTransactionIDS* setOfTIDs, StoreManager* sm, CrashRecovery* cr )
{
  // Ponieważ się cofamy, to po napotkaniu poczatku tranzakcji 
  // mozemy ja usunac ze spisu transakcji do wycofania.
  setOfTIDs->erase(tid);
  return 0;
}



/* CkptRecord class */

int CkptRecord::read( int fileDes, StoreManager* sm ) { return LogIO::readTransactionIDVector( tidVec, fileDes ); }
int CkptRecord::write( int fileDes ) { return LogIO::writeTransactionIDVector( tidVec, fileDes ); }


/* WriteRecord class */

WriteRecord::WriteRecord( int _tid, LogicalID *_lid, string _name, DataValue *_oldVal, DataValue *_newVal, bool _newLID )
: TransactionRecord( _tid, WRITE_LOG_REC_TYPE )
{
  lid = _lid;
  newLID = _newLID;
  name = _name;
  oldVal = _oldVal;
  newVal = _newVal;
  if( _oldVal == NULL ) oldValS = "";
  else {
    Serialized serial1 = _oldVal->serialize();
    oldValS = string( (const char*) serial1.bytes, serial1.size );
  }

  if( _newVal == NULL ) newValS = "";
  else {
    Serialized serial2 = _newVal->serialize();
    newValS = string( (const char*) serial2.bytes, serial2.size );
  }

}

int WriteRecord::deleteFromStore(StoreManager* sm, DataValue *dv)
{
  int err=0;
  if( dv != NULL )
  {
    if( oldVal == dv ) freeOldValFlag = false;
    else if( newVal == dv ) freeNewValFlag = false;
  }
  ObjectPointer* op = sm->createObjectPointer( (lid ? lid->clone() : NULL), name, dv);
  err=sm->deleteObject( NULL, op );
  delete op;
  printf( "Delete... (err=%d)\n", err );
  return err;
}

//UWAGA to zle dziala  TODO createObject, trzeba zamienic na cos co robi undo delete
int WriteRecord::addToStore(StoreManager* sm, DataValue *dv)
{
  LogicalID *lidCopy = NULL;
  int err = 0;
  printf( "createObjectPointer( lid=%d, name='%s', dv='%s' )\n", lid->toInteger(), name.c_str(), dv->toString().c_str() );
  if( dv != NULL )
  {
    if( oldVal == dv ) freeOldValFlag = false;
    else if( newVal == dv ) freeNewValFlag = false;
  }
  ObjectPointer* op = sm->createObjectPointer( (lid ? lid->clone() : NULL), name, dv);

  if( ( sm->getLogManager()->getCrashRecoveryInProgress() ) && newLID )
    lidCopy = NULL;
  else
    lidCopy = (lid ? lid->clone() : NULL );

  lidCopy = (lid ? lid->clone() : NULL );
  err=sm->createObject( NULL, name, dv, op, lidCopy);
  delete op;
  return err;
}

int WriteRecord::changeValue(StoreManager* sm, DataValue* dv)
//moze ta metoda nie bedzie potrzebna to zalezy od tego jak bedzie dzialac to undoDelete
{
  int err = 0;
  ObjectPointer* op=NULL;
  //trzeba okreslic dokladnie jakie ma byc to err
  printf( "LID: %d\n", lid->toInteger() );
  if( (err = sm->getObject(NULL,(lid ? lid->clone() : NULL),Write,op)) == 0)
  {
    //Nie ma żadnej pewności, że to będzie działać z sensem
    //bo w virtual void setValue(DataValue* val) nie ma miejsca na tid
    op->setValue(dv);
  }
  else
  {
    //nie ma obiektu wiec nalezy go odtworzyc
    err = addToStore(sm, dv);
  }
  delete op;// choc nie wiem czy to dobry pomysl??
  return err;
}

int WriteRecord::rollBack(SetOfTransactionIDS* setOfTIDs, StoreManager* sm, CrashRecovery* cr )
{
  if(cr != NULL)
  {
    int err=0;
    bool possible;
    if( (err=cr->storeReadingPossible(&possible)) !=0 )
      return err;
    if( !possible )
      return 0;
  }
  //sprawdzamy czy zapis dotyczy jednej z tranzakcji do wycofania.
  if(setOfTIDs->find(tid) != setOfTIDs->end())
  {
    //zła postać rekordu w logach
    if(oldVal == NULL && newVal == NULL )
      return LOG_INCORRECT_RECORD_FORMAT;

    //Wycofywana transakcja stworzyla obiekt, nalezy go usunac.
    if(oldVal == NULL)
      return deleteFromStore(sm, newVal);

    //Wycofywana tranzakcja usunęła obiekt, należy go odtworzyć.
    if(newVal == NULL)
      return addToStore( sm, oldVal);

    //sa obie wartosci trzeba przywrocic stara
    return changeValue(sm, oldVal);//mozliwe ze mozna addToStore

    //return 0;
  }
  return 0;
}

int WriteRecord::read( int fileDes, StoreManager* sm )
{
  int errCode;
  int flag;

  if( ( errCode = TransactionRecord::read( fileDes, sm ) ) ) return errCode;
  if( ( errCode = LogIO::readLogicalID( lid, fileDes, sm ) ) ) return errCode;
  if( ( errCode = LogIO::readString( fileDes, name ) ) ) return errCode;
  if( ( errCode = LogIO::readDataValue( oldVal, fileDes, sm ) ) ) return errCode;
  if( ( errCode = LogIO::readDataValue( newVal, fileDes, sm ) ) ) return errCode;
  if( ( errCode = LogIO::readInt( fileDes, flag ) ) ) return errCode;

  newLID = ( flag > 0 ) ? true : false;

  return errCode;
}

int WriteRecord::write( int fileDes )
{
  int errCode;

  if( ( errCode = TransactionRecord::write( fileDes ) ) ) return errCode;
  if( ( errCode = LogIO::writeLogicalID( lid, fileDes ) ) ) return errCode;
  if( ( errCode = LogIO::writeString( fileDes, (char*) name.data(), name.length() ) ) ) return errCode;
  if( ( errCode = LogIO::writeString( fileDes, oldValS.data(), oldValS.length() ) ) ) return errCode;
  if( ( errCode = LogIO::writeString( fileDes, newValS.data(), newValS.length() ) ) ) return errCode;
  if( ( errCode = LogIO::writeInt( fileDes, ( newLID ? 1 : 0 ) ) ) ) return errCode;

  return errCode;
}

/* RootRecord class */

//Uwaga, czy Store jest odporny na usuwanie roota ktorego nie ma??
//bo moze byc tak, ze najpierw zostanie usuniety obiekt z roota, a potem sam obiekt
//przy jechaniu w tył bedzie zle.
//Jesli Store nie jest na to odporny trzeba sie przejechac dwa razy po logu zeby cos wycofac.
int RootRecord::removeRootFromStore( StoreManager* sm )
{
  int err=0;
  ObjectPointer* op = sm->createObjectPointer( lid ? lid->clone() : NULL );
  err = sm->removeRoot( NULL, op );
  delete op;
  return err;
}

//Uwaga może działać bez sensu, jeśli przy odtwarzaniu obiektu bedzie przydzielane nowe lid
//to wtedy jako root moze byc wstawiane cos czego juz nie ma
int RootRecord::addRootToStore( StoreManager* sm )
{
  int err = 0;
  ObjectPointer* op = sm->createObjectPointer( lid ? lid->clone() : NULL );
  err = sm->addRoot( NULL, op );
  delete op;
  return err;
}

RootRecord::~RootRecord() {  delete lid; }

int RootRecord::write( int fileDes )
{
  int errCode;

  if( ( errCode = TransactionRecord::write( fileDes ) ) ) return errCode;
  if( ( errCode = LogIO::writeLogicalID( lid, fileDes ) ) ) return errCode;
  return errCode;
}

int RootRecord::read( int fileDes, StoreManager* sm )
{
  int errCode;

  if( ( errCode = TransactionRecord::read( fileDes, sm ) ) ) return errCode;
  if( ( errCode = LogIO::readLogicalID( lid, fileDes, sm ) ) ) return errCode;
  return errCode;
}

/* AddRootRecord class */
int AddRootRecord::rollBack(SetOfTransactionIDS* setOfTIDs, StoreManager* sm, CrashRecovery* cr )
{
  if(cr != NULL)
  {
    int err=0;
    bool possible;
    if( (err=cr->storeReadingPossible(&possible)) !=0 )
      return err;
    if( !possible )
      return 0;
  }
  //sprawdzamy czy zapis dotyczy jednej z tranzakcji do wycofania.
  if(setOfTIDs->find(tid) != setOfTIDs->end())
    //Wycofywana tranzakcja dodała roota więc go usuwamy.
    return removeRootFromStore( sm );
  return 0;
}

/* RemoveRootRecord class */
int RemoveRootRecord::rollBack(SetOfTransactionIDS* setOfTIDs, StoreManager* sm, CrashRecovery* cr )
{
  if(cr != NULL)
  {
    int err=0;
    bool possible;
    if( (err=cr->storeReadingPossible(&possible)) !=0 )
      return err;
    if( !possible )
      return 0;
  }
  //sprawdzamy czy zapis dotyczy jednej z tranzakcji do wycofania.
  if(setOfTIDs->find(tid) != setOfTIDs->end())
    //Wycofywana tranzakcja usunęła roota więc go dodajemy.
    return addRootToStore( sm );
  return 0;
}
