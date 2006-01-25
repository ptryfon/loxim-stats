#include "LogRecord.h"
#include "LogIO.h"
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>


/* LogRecord class */

int LogRecord::initialize()
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
  result->read( fileDes, sm );

  if( ( errCode = LogIO::readInt( fileDes, recordLen ) ) ) return errCode;
  if( ( errCode = LogIO::getFilePos( fileDes, filePosEnd ) ) ) return errCode;

  if( filePosEnd - filePosBegin != recordLen ) return LOG_RECORD_LENGTH_MISMATCH_ERROR;

  return 0;
}

int LogRecord::readLogRecordBackward( LogRecord *&result, int fileDes, StoreManager* sm )
{
  int errCode;
  int recordLen;
  off_t filePos;

  if( ( errCode = LogIO::getFilePos( fileDes, filePos ) ) ) return errCode;
  if( !filePos )
  {
    result = NULL;
    return 0;
  }

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

  if( ( errCode = recordPtr->getType( recordType ) ) ) return errCode;
  if( ( errCode = LogIO::getFilePos( fileDes, filePosBegin ) ) ) return errCode;

  // zapisujemy typ rekordu
  if( ( errCode = LogIO::writeInt( fileDes, recordType ) ) ) return errCode;

  // zapisujemy rekord
  if( ( errCode = recordPtr->write( fileDes ) ) ) return errCode;

  // zapisujemy dlugosc rekordu
  if( ( errCode = LogIO::getFilePos( fileDes, filePosEnd ) ) ) return errCode;
  if( ( errCode = LogIO::writeInt( fileDes, filePosEnd - filePosBegin + sizeof( int ) ) ) ) return errCode;

  return 0;
}


/* TransactionRecord class */

int TransactionRecord::read( int fileDes, StoreManager* sm ) { return LogIO::readTransactionID( tid, fileDes ); }

int TransactionRecord::write( int fileDes ) { return LogIO::writeTransactionID( tid, fileDes ); }

/* BeginTransactionRecord class */

int BeginTransactionRecord::rollBack(SetOfTransactionIDS* setOfTIDs, StoreManager* sm)
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

WriteRecord::WriteRecord( int _tid, LogicalID *_lid, string _name, DataValue *_oldVal, DataValue *_newVal )
: TransactionRecord( _tid, WRITE_LOG_REC_TYPE ), lid( _lid ), name(_name), oldVal( _oldVal ), newVal( _newVal )
{
  if( _oldVal == NULL ) oldValS = "";
  else {
    Serialized serial1 = _oldVal->serialize();
    oldValS = string( (const char*) serial1.bytes, serial1.size );
    printf("s1 = %d ",serial1.size);
  }

  if( _newVal == NULL ) newValS = "";
  else {
    Serialized serial2 = _newVal->serialize();
    newValS = string( (const char*) serial2.bytes, serial2.size );
    printf("s2 = %d\n",serial2.size);
  }
  
}

int WriteRecord::deleteFromStore(StoreManager* sm, DataValue *dv)
{
  ObjectPointer* op = sm->createObjectPointer( lid, name, dv);
  return sm->deleteObject( NULL, op );
}

//UWAGA to zle dziala  TODO createObject, trzeba zamienic na cos co robi undo delete
int WriteRecord::addToStore(StoreManager* sm, DataValue *dv)
{
  ObjectPointer* op = sm->createObjectPointer( lid, name, dv);
  return sm->createObject( NULL, name, dv, op );
}

int WriteRecord::rollBack(SetOfTransactionIDS* setOfTIDs, StoreManager* sm)
{
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

    return 0;
  }
  return 0;
}

int WriteRecord::read( int fileDes, StoreManager* sm )
{
  int errCode;

  if( ( errCode = TransactionRecord::read( fileDes, sm ) ) ) return errCode;
  if( ( errCode = LogIO::readLogicalID( lid, fileDes, sm ) ) ) return errCode;
  if( ( errCode = LogIO::readString( fileDes, name ) ) ) return errCode;
  if( ( errCode = LogIO::readDataValue( oldVal, fileDes, sm ) ) ) return errCode;
  if( ( errCode = LogIO::readDataValue( newVal, fileDes, sm ) ) ) return errCode;

  return errCode;
}

int WriteRecord::write( int fileDes )
{
  int errCode;

  if( ( errCode = TransactionRecord::write( fileDes ) ) ) return errCode;
  if( ( errCode = LogIO::writeLogicalID( lid, fileDes ) ) ) return errCode;
  if( ( errCode = LogIO::writeString( fileDes, (char*) name.data(), name.length() ) ) ) return errCode;
  if( ( errCode = LogIO::writeString( fileDes, oldValS.c_str(), oldValS.length() ) ) ) return errCode;
  if( ( errCode = LogIO::writeString( fileDes, newValS.c_str(), newValS.length() ) ) ) return errCode;

  return errCode;
}

/* RootRecord class */

//Uwaga, czy Store jest odporny na usuwanie roota ktorego nie ma??
//bo moze byc tak, ze najpierw zostanie usuniety obiekt z roota, a potem sam obiekt
//przy jechaniu w tył bedzie zle.
//Jesli Store nie jest na to odporny trzeba sie przejechac dwa razy po logu zeby cos wycofac.
int RootRecord::removeRootFromStore( StoreManager* sm )
{
  ObjectPointer* op = sm->createObjectPointer( lid );
  return sm->removeRoot( NULL, op );
}

//Uwaga może działać bez sensu, jeśli przy odtwarzaniu obiektu bedzie przydzielane nowe lid
//to wtedy jako root moze byc wstawiane cos czego juz nie ma
int RootRecord::addRootToStore( StoreManager* sm )
{
  ObjectPointer* op = sm->createObjectPointer( lid );
  return sm->addRoot( NULL, op );
}


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
int AddRootRecord::rollBack(SetOfTransactionIDS* setOfTIDs, StoreManager* sm)
{
  //sprawdzamy czy zapis dotyczy jednej z tranzakcji do wycofania.
  if(setOfTIDs->find(tid) != setOfTIDs->end())
    //Wycofywana tranzakcja dodała roota więc go usuwamy.
    return removeRootFromStore( sm );
  return 0;
}

/* RemoveRootRecord class */
int RemoveRootRecord::rollBack(SetOfTransactionIDS* setOfTIDs, StoreManager* sm)
{
  //sprawdzamy czy zapis dotyczy jednej z tranzakcji do wycofania.
  if(setOfTIDs->find(tid) != setOfTIDs->end())
    //Wycofywana tranzakcja usunęła roota więc go dodajemy.
    return addRootToStore( sm );
  return 0;
}
