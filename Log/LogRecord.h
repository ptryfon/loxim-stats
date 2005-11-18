#ifndef __LOG_RECORD_H__
#define __LOG_RECORD_H__

namespace Logs
{
  class LogRecord;
  class BeginTransactionRecord;
  class WriteRecord;
  class CkptRecord;
  class EndCkptRecord;
  class CommitRecord;
  class RollbackRecord;
}

#include <string>
#include <map>
#include <stdio.h>
#include "../Store/Store.h"
#include "../TransactionManager/Transaction.h"

using namespace Store;
using namespace TManager;


namespace Logs
{

#define UNKNOWN_LOG_RECORD_TYPE_ERROR    200002
#define LOG_RECORD_LENGTH_MISMATCH_ERROR 200003

/// Mozliwe typy rekordu logowania
#define BEGIN_LOG_REC_TYPE     1
#define COMMIT_LOG_REC_TYPE    2
#define ROLLBACK_LOG_REC_TYPE  3
#define CKPT_LOG_REC_TYPE      4
#define END_CKPT_LOG_REC_TYPE  5
#define WRITE_LOG_REC_TYPE     6


  class LogRecord
  {
    protected:
    unsigned int id;           // unikalny identyfikator rekordu
    int type;                  // stala XXXX_LOG_REC_TYPE
    static map< int, LogRecord * > dictionary; // prototypy obiektow podklas
    static unsigned int idSeq; // sekwencja do generowania kolejnych wartosci pola `id`

    LogRecord() : id( ++idSeq ) {}
    LogRecord( int _type ) : id( ++idSeq ), type( _type ) { dictionary[type] = this; }

    virtual int read( int fileDes ) = 0;
    virtual int write( int fileDes ) = 0;
    virtual int instance( LogRecord *&result ) = 0;

    public:
    static int initialize();
    static int readLogRecordForward( LogRecord *&result, int fileDes );
    static int readLogRecordBackward( LogRecord *&result, int fileDes );
    static int writeLogRecord( LogRecord *recordPtr, int fileDes ); // zakladamy ze wskaznik biezacej pozycji w pliku jest na koncu pliku
    static int destroy();

    virtual ~LogRecord() {}
    int getId( unsigned int &result ) { result = id; return 0; }
    int getType( int &result ) { result = type; return 0; }
  };


  class TransactionRecord : public LogRecord
  {
    friend class LogRecord;

    protected:
    TransactionID *tid;

    TransactionRecord( int _type ) : LogRecord( _type ) {}

    virtual int read( int fileDes );
    virtual int write( int fileDes );

    public:
    virtual ~TransactionRecord() { /* delete tid; */ } // TODO
    TransactionRecord( TransactionID *_tid ) : LogRecord(), tid( _tid ) {}
  };


  class BeginTransactionRecord : public TransactionRecord
  {
    friend class LogRecord;

    protected:
    BeginTransactionRecord() : TransactionRecord( BEGIN_LOG_REC_TYPE ) {}

    virtual int instance( LogRecord *&result ) { result = new BeginTransactionRecord(); return 0; }

    public:
    BeginTransactionRecord( TransactionID *_tid ) : TransactionRecord( _tid ) {}
  };


  class WriteRecord : public TransactionRecord
  {
    friend class LogRecord;

    protected:
    LogicalID *lid;
    DataValue *oldVal;
    DataValue *newVal;

    WriteRecord() : TransactionRecord( WRITE_LOG_REC_TYPE ) {}

    virtual int read( int fileDes );
    virtual int write( int fileDes );
    virtual int instance( LogRecord *&result ) { result = new WriteRecord(); return 0; }

    public:
    WriteRecord( TransactionID *_tid, LogicalID *_lid, DataValue *_oldVal, DataValue *_newVal )
    : TransactionRecord( _tid ), lid( _lid ), oldVal( _oldVal ), newVal( _newVal )
    {}
    virtual ~WriteRecord() { /* delete lid; delete oldVal; delete newVal; */} // TODO
  };


  class CommitRecord : public TransactionRecord
  {
    friend class LogRecord;

    protected:
    CommitRecord() : TransactionRecord( COMMIT_LOG_REC_TYPE ) {}

    virtual int instance( LogRecord *&result ) { result = new CommitRecord(); return 0; }

    public:
    CommitRecord( TransactionID *_tid ) : TransactionRecord( _tid ) {}
  };


  class RollbackRecord : public TransactionRecord
  {
    friend class LogRecord;

    protected:
    RollbackRecord() : TransactionRecord( ROLLBACK_LOG_REC_TYPE ) {}

    virtual int instance( LogRecord *&result ) { result = new RollbackRecord(); return 0; }

    public:
    RollbackRecord( TransactionID *_tid ) : TransactionRecord( _tid ) {}
  };


  class CkptRecord : public LogRecord
  {
    friend class LogRecord;

    protected:
    vector< TransactionID * > *tidVec;

    CkptRecord() : LogRecord( CKPT_LOG_REC_TYPE ) {}

    virtual int read( int fileDes );
    virtual int write( int fileDes );
    virtual int instance( LogRecord *&result ) { result = new CkptRecord(); return 0; }

    public:
    CkptRecord( vector< TransactionID *> *_tidVec ) : LogRecord(), tidVec( _tidVec ) {}

    virtual ~CkptRecord() { delete tidVec; }
  };


  class EndCkptRecord : public LogRecord
  {
    friend class LogRecord;

    virtual int read( int fileDes ) { return 0; }
    virtual int write( int fileDes ) { return 0; }
    virtual int instance( LogRecord *&result ) { result = new EndCkptRecord(); return 0; }

    public:
    EndCkptRecord() : LogRecord( END_CKPT_LOG_REC_TYPE ) {}
  };

};

#endif

