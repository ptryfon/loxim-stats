#ifndef _CRASH_RECOVERY_
#define _CRASH_RECOVERY_

namespace Logs
{
class CrashRecovery;
class CRState;
};

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "Logs.h"
#include "../Store/Store.h"
#include "LogRecord.h"
#include "LogIO.h"

using namespace std;
using namespace Store;

namespace Logs
{

#define LOG_CR_UNFINISHED_ROLLBACK 300001

typedef set<int> SetOfTransactionIDS;

class CRState
{
protected:
  CrashRecovery* cr;
public:
  virtual int init(CrashRecovery* cr) {this->cr = cr; return 0;}
  virtual int CkptReached(){return 0;};
  virtual int nextStep(bool* itsRcEnd){ *itsRcEnd = false; return 0; };
  virtual int properlyShut(){return 0;};
  virtual ~CRState(){}
};

class CRStart : public CRState
{
protected:
  bool isShutDownProper;
public:
  CRStart() : isShutDownProper(false){}
  virtual int properlyShut() { isShutDownProper=true; return 0;}
  virtual int nextStep(bool* itsRcEnd);
  virtual ~CRStart(){}
};

class CRNoteAndRollback : public CRState
{
protected:
  bool isCkptReached;
public:
  CRNoteAndRollback(): isCkptReached(false){}
  virtual int CkptReached(){ isCkptReached = true; return 0;}
  virtual int nextStep(bool* itsRcEnd);
  virtual ~CRNoteAndRollback(){}
};

class CRRollback : public CRState
{
public:
  virtual int nextStep(bool* itsRcEnd);
  virtual ~CRRollback(){}
};

class CRCommit : public CRState
{
public:
  virtual int nextStep(bool* itsRcEnd);
  virtual ~CRCommit(){}
};

/*
  LogRecord *record = new RollbackRecord( tid );
  logThread->push( record );
*/

class CRSaveRollback : public CRState
{
public:
  virtual int nextStep(bool* itsRcEnd);
  virtual ~CRSaveRollback(){}
};

class CrashRecovery
{

friend class CRState;
friend class CRStart;
friend class CRNoteAndRollback;
friend class CRRollback;
friend class CRCommit;
friend class CRSaveRollback;

protected:
  unsigned long ckptStart;
  bool afterCkptEnd;
  LogManager* lm;
  StoreManager*sm;
  CRState* actualFaze;
  int fd;
  off_t ckptPos;
  
public:
  SetOfTransactionIDS tidsToRollback;
  SetOfTransactionIDS rollbackedTids;
  SetOfTransactionIDS tidsToCommit;

  CrashRecovery():afterCkptEnd(false), actualFaze(NULL){}
  
  virtual int init(LogManager* lm, StoreManager* sm);
  
  virtual int Recover();
  
  virtual int CkptEnd() { afterCkptEnd = true; return 0;}
  
  virtual int CkptStart();

  virtual int shutDown() { actualFaze->properlyShut(); return 0; }
  
  virtual ~CrashRecovery();

};

};
#endif
