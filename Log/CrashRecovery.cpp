#include "CrashRecovery.h"

namespace Logs
{

int CRStart::nextStep(bool* itsRcEnd)
{
  LogRecord* lr=NULL;
  int err;
  if( ::lseek(cr->fd,0,SEEK_END) < 0 ) return errno;
  if( (err = LogRecord::readLogRecordBackward(lr, cr->fd, cr->sm)) != 0)
  {
    delete lr;
    if(err == UNEXPECTED_EOF_ERROR)//nie ma co przywracac jak plik jest pusty
    {
      *itsRcEnd = true;
      return 0;
    }
    return err;
  }
  if( (err = (lr->shutDown(cr)) ) != 0 )
    return err;
  *itsRcEnd = isShutDownProper;
  if( !isShutDownProper )//jesli to nie byl dobry rekord to nalezy go przeczytac
	  		 //jako cos innego
    if( ::lseek(cr->fd,0,SEEK_END) < 0 ) return errno;
  return 0;
}

int CRNoteAndRollback::nextStep(bool* itsRcEnd)
{
  LogRecord* lr=NULL;
  int err=0;
  *itsRcEnd = false;
  while( !isCkptReached && ((err = LogRecord::readLogRecordBackward(lr, cr->fd, cr->sm)) == 0) )
  {
    lr->modifySetsBackDir(cr);
    lr->rollBack(&(cr->tidsToRollback), cr->sm, cr);
    lr->ckptEnd(cr);
    lr->ckptStart(cr);
    delete lr;
    lr=NULL;
  }
  delete lr;
  if(err == UNEXPECTED_EOF_ERROR )
  {
    if((cr->tidsToRollback).empty())
      return 0;
    return LOG_CR_UNFINISHED_ROLLBACK;
  }
  else//nalezy przepisac wszystkie elementy tidsToRollback do rollbackedTids, bo CRRollback tego nie robi
      //trzeba tez zaznaczyc gdzie jest ckpt, zeby wiedziec gdzie wrocic
  {
    for(SetOfTransactionIDS::iterator  it = (cr->tidsToRollback).begin(); it!=(cr->tidsToRollback).end(); it++)
      (cr->rollbackedTids).insert((*it));
    return LogIO::getFilePos( cr->fd, cr->ckptPos );
  }
  return err;
}

int CRRollback::nextStep(bool* itsRcEnd)
{
  LogRecord* lr=NULL;
  int err=0;
  *itsRcEnd = false;
  printf( "Here: %d\n", cr->tidsToRollback.size() );
  while( !(cr->tidsToRollback).empty() && ((err = LogRecord::readLogRecordBackward(lr, cr->fd, cr->sm)) == 0) )//potrzebne skrócone wyliczanie
  {
    lr->rollBack(&(cr->tidsToRollback), cr->sm, cr);
    delete lr;
    lr=NULL;
  }
  delete lr;
  return err;
}


int CRCommit::nextStep(bool* itsRcEnd)
{
  LogRecord* lr=NULL;
  int err=0;
  *itsRcEnd = false;
  if( ::lseek(cr->fd,cr->ckptPos,SEEK_SET) < 0 ) return errno;//ustawiamy sie na checkpoincie
  while( !(cr->tidsToCommit).empty() && ((err = LogRecord::readLogRecordForward(lr, cr->fd, cr->sm)) == 0) )//do przodu
  {
    lr->commit(&(cr->tidsToCommit), cr->sm);
    delete lr;
    lr=NULL;
  }
  delete lr;
  return err;
}

/*
  LogRecord *record = new RollbackRecord( tid );
  logThread->push( record );
*/

int CRSaveRollback::nextStep(bool* itsRcEnd)
{
  int err=0;
  *itsRcEnd = true;
  if( ::lseek(cr->fd,0,SEEK_END) < 0 ) return errno;//ustawiamy sie na koncu pliku
  for(SetOfTransactionIDS::iterator  it = (cr->tidsToRollback).begin(); it!=(cr->tidsToRollback).end() && (err==0); it++)
  {//moznaby wycofywac tylko te ktorych nie bylo w tidsToRollback, byloby moze dokladniej
    err=cr->lm->printRollbackSign(*it);
  }
  cr->lm->flushLog();
  return err;
}

int CrashRecovery::init(LogManager* lm, StoreManager* sm)
  {
    int err=0;
    this->lm=lm;
    this->sm=sm;
    if((fd = ::open( lm->getLogFilePath().c_str(), O_RDWR)) < 0 ) return errno;
    return err;
  }

int CrashRecovery::Recover()
  {
    printf( "CrashRecovery::Recover!\n" );
    int err=0;
    bool isEnd=false;
    actualFaze = new CRStart();
    if( (err=actualFaze->init(this)) != 0)
      return err;
    if( (err = actualFaze->nextStep(&isEnd)) !=0 )
      return err;
    if(isEnd)
      return 0;
    delete actualFaze;
    actualFaze= new CRNoteAndRollback();
    if( (err=actualFaze->init(this)) != 0)
      return err;
    if( (err = actualFaze->nextStep(&isEnd)) !=0 )
      return err;
    delete actualFaze;
    actualFaze= new CRRollback();
    if( (err=actualFaze->init(this)) != 0)
      return err;
    if( (err = actualFaze->nextStep(&isEnd)) !=0 )
      return err;
    delete actualFaze;
    actualFaze= new CRCommit();
    if( (err=actualFaze->init(this)) != 0)
      return err;
    if( (err = actualFaze->nextStep(&isEnd)) !=0 )
      return err;
    delete actualFaze;
    actualFaze= new CRSaveRollback();
    if( (err=actualFaze->init(this)) != 0)
      return err;
    if( (err = actualFaze->nextStep(&isEnd)) !=0 )
      return err;
    delete actualFaze;
    actualFaze=NULL;
    return 0;
  }

int CrashRecovery::CkptStart()
{
  if(afterCkptEnd)
    return actualFaze->CkptReached();
  return 0;
}

CrashRecovery::~CrashRecovery()
{
  delete actualFaze;
  close(fd);
}

};
