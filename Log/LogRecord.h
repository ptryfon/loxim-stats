#ifndef __LOG_RECORD_H__
#define __LOG_RECORD_H__

namespace Logs
{
  class LogRecord;
  class BeginTransactionRecord;
  class WriteRecord;
  class CkptRecord;
  class ShutdownRecord;
  class EndCkptRecord;
  class CommitRecord;
  class RollbackRecord;
  class AddRootRecord;
  class RemoveRootRecord;
}

#include <string>
#include <map>
#include <set>
#include <stdio.h>
#include "../Store/Store.h"

using namespace Store;
using namespace std;


namespace Logs
{

#define UNKNOWN_LOG_RECORD_TYPE_ERROR    200002
#define LOG_RECORD_LENGTH_MISMATCH_ERROR 200003
#define LOG_INCORRECT_RECORD_FORMAT      200004

/// Mozliwe typy rekordu logowania
#define BEGIN_LOG_REC_TYPE        1
#define COMMIT_LOG_REC_TYPE       2
#define ROLLBACK_LOG_REC_TYPE     3
#define CKPT_LOG_REC_TYPE         4
#define END_CKPT_LOG_REC_TYPE     5
#define WRITE_LOG_REC_TYPE        6
#define SHUTDOWN_LOG_REC_TYPE     7
#define ADD_ROOT_LOG_REC_TYPE     8
#define REMOVE_ROOT_LOG_REC_TYPE  9

  typedef set<int> SetOfTransactionIDS;

  class LogRecord
  {
    protected:
    unsigned int id;           // unikalny identyfikator rekordu
    int type;                  // stala XXXX_LOG_REC_TYPE
    static map< int, LogRecord * > dictionary; // prototypy obiektow podklas
    static unsigned int idSeq; // sekwencja do generowania kolejnych wartosci pola `id`

    LogRecord() : id( ++idSeq ) {}
    LogRecord( int _type ) : id( ++idSeq ), type( _type ) { dictionary[type] = this; }

    virtual int read( int fileDes, StoreManager* sm ) = 0;
    virtual int write( int fileDes ) = 0;
    virtual int instance( LogRecord *&result ) = 0;

    public:
    static int initialize();
    static int readLogRecordForward( LogRecord *&result, int fileDes, StoreManager* sm );
    static int readLogRecordBackward( LogRecord *&result, int fileDes, StoreManager* sm );
    static int writeLogRecord( LogRecord *recordPtr, int fileDes ); // zakladamy ze wskaznik biezacej pozycji w pliku jest na koncu pliku
    static int destroy();

    virtual ~LogRecord() {}
    int getId( unsigned int &result ) { result = id; return 0; }
    int getType( int &result ) { result = type; return 0; }

    /*
    setOfTIDs - wszystkie tranzakcje które chcemy wycofać.
    Metoda bierze zbiór tranzakcji do wycofania i jeśli rekord dotyczy tej tranzakcji to dokonuje wycofania w sm
    lub usuwa transakcję ze zbioru (begin) lub nie robi nic.
    */
    virtual int rollBack(SetOfTransactionIDS* setOfTIDs, StoreManager* sm) {return 0;}
  };


  class ShutdownRecord : public LogRecord
  {
    friend class LogRecord;

    protected:

    virtual int read( int fileDes, StoreManager* sm ) { return 0; }
    virtual int write( int fileDes ) { return 0; }

    virtual int instance( LogRecord *&result ) { result = new ShutdownRecord(); return 0; }

    public:
    ShutdownRecord() : LogRecord( SHUTDOWN_LOG_REC_TYPE ) {dictionary[type] = this;}
    virtual ~ShutdownRecord() {}

  };


  class TransactionRecord : public LogRecord
  {
    friend class LogRecord;

    protected:
    int tid;

    TransactionRecord( int _type ) : LogRecord( _type ) {}

    virtual int read( int fileDes, StoreManager* sm );
    virtual int write( int fileDes );

    public:
    virtual ~TransactionRecord() { /* delete tid; */ } // TODO
    TransactionRecord( int _tid, int _type ) : LogRecord( _type ), tid( _tid ) {}
  };


  class BeginTransactionRecord : public TransactionRecord
  {
    friend class LogRecord;

    protected:
    BeginTransactionRecord() : TransactionRecord( BEGIN_LOG_REC_TYPE ) {dictionary[type] = this;}

    virtual int instance( LogRecord *&result ) { result = new BeginTransactionRecord(); return 0; }

    public:
    virtual int rollBack(SetOfTransactionIDS* setOfTIDs, StoreManager* sm);
    BeginTransactionRecord( int _tid ) : TransactionRecord( _tid, BEGIN_LOG_REC_TYPE ) {}
  };


  class WriteRecord : public TransactionRecord
  {
    friend class LogRecord;

    protected:

    string name;

    // do zapisu
    string oldValS;
    string newValS;

    public://do testow
    // do odczytu
    LogicalID *lid;
    DataValue *oldVal;
    DataValue *newVal;

    protected:
    WriteRecord() : TransactionRecord( WRITE_LOG_REC_TYPE ) {dictionary[type] = this;}

    virtual int read( int fileDes, StoreManager* sm );
    virtual int write( int fileDes );
    virtual int instance( LogRecord *&result ) { result = new WriteRecord(); return 0; }
    virtual int deleteFromStore(StoreManager* sm, DataValue *dv);
    virtual int addToStore(StoreManager* sm, DataValue *dv);
    virtual int changeValue(StoreManager* sm, DataValue* dv);//mozliwe ze niepotrzebne

    public:
    WriteRecord( int _tid, LogicalID *_lid, string _name, DataValue *_oldVal, DataValue *_newVal );
    virtual ~WriteRecord() {  /*delete lid; delete oldVal; delete newVal;*/ } //
    virtual int rollBack(SetOfTransactionIDS* setOfTIDs, StoreManager* sm);
  };

  class RootRecord : public TransactionRecord
  {
    friend class LogRecord;

    private:
    LogicalID *lid;// trzeba miec tu bytearray

    protected:
    RootRecord( int _type ) : TransactionRecord( _type ) {}

    virtual int read( int fileDes, StoreManager* sm );
    virtual int write( int fileDes );
    virtual int removeRootFromStore( StoreManager* sm );
    virtual int addRootToStore( StoreManager* sm );

    public:
    RootRecord( int _tid, LogicalID *_lid, int _type) : TransactionRecord( _tid, _type ), lid(_lid) {}
    virtual ~RootRecord() {  /*delete lid; */ } //
    //virtual int rollBack(SetOfTransactionIDS* setOfTIDs, StoreManager* sm);
  };

  class AddRootRecord : public RootRecord
  {
    friend class LogRecord;

    protected:
    AddRootRecord() : RootRecord(ADD_ROOT_LOG_REC_TYPE) {dictionary[type] = this;}
    virtual int instance( LogRecord *&result ) { result = new AddRootRecord(); return 0; }

    public:
    AddRootRecord( int _tid, LogicalID *_lid) : RootRecord( _tid, _lid, ADD_ROOT_LOG_REC_TYPE ) {}
    virtual int rollBack(SetOfTransactionIDS* setOfTIDs, StoreManager* sm);
    virtual ~AddRootRecord(){}
  };

  class RemoveRootRecord : public RootRecord
  {
    friend class LogRecord;

    protected:
    RemoveRootRecord() : RootRecord(REMOVE_ROOT_LOG_REC_TYPE) {dictionary[type] = this;}
    virtual int instance( LogRecord *&result ) { result = new RemoveRootRecord(); return 0; }

    public:
    RemoveRootRecord( int _tid, LogicalID *_lid) : RootRecord( _tid, _lid, REMOVE_ROOT_LOG_REC_TYPE ) {}
    virtual int rollBack(SetOfTransactionIDS* setOfTIDs, StoreManager* sm);
    virtual ~RemoveRootRecord(){}
  };

  class CommitRecord : public TransactionRecord
  {
    friend class LogRecord;

    protected:
    CommitRecord() : TransactionRecord( COMMIT_LOG_REC_TYPE ) {dictionary[type] = this;}

    virtual int instance( LogRecord *&result ) { result = new CommitRecord(); return 0; }

    public:
    CommitRecord( int _tid ) : TransactionRecord( _tid, COMMIT_LOG_REC_TYPE ) {}
  };


  class RollbackRecord : public TransactionRecord
  {
    friend class LogRecord;

    protected:
    RollbackRecord() : TransactionRecord( ROLLBACK_LOG_REC_TYPE ) {dictionary[type] = this;}

    virtual int instance( LogRecord *&result ) { result = new RollbackRecord(); return 0; }

    public:
    RollbackRecord( int _tid ) : TransactionRecord( _tid, ROLLBACK_LOG_REC_TYPE ) {}
  };

  class CkptRecord : public LogRecord
  {
    friend class LogRecord;

    protected:
    vector< int > *tidVec;

    CkptRecord() : LogRecord( CKPT_LOG_REC_TYPE ) {dictionary[type] = this;}

    virtual int read( int fileDes, StoreManager* sm );
    virtual int write( int fileDes );
    virtual int instance( LogRecord *&result ) { result = new CkptRecord(); return 0; }

    public:
    CkptRecord( vector< int > *_tidVec ) : LogRecord( CKPT_LOG_REC_TYPE ), tidVec( _tidVec ) {}

    virtual ~CkptRecord() { delete tidVec; }
  };


  class EndCkptRecord : public LogRecord
  {
    friend class LogRecord;

    virtual int read( int fileDes, StoreManager* sm ) { return 0; }
    virtual int write( int fileDes ) { return 0; }
    virtual int instance( LogRecord *&result ) { result = new EndCkptRecord(); return 0; }

    public:
    EndCkptRecord() : LogRecord( END_CKPT_LOG_REC_TYPE ) {dictionary[type] = this;}
  };

};

#endif

