#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../Config/SBQLConfig.h"
#include <iostream>
#include "StoreLock.h"

using namespace std;
using namespace Config;

#define LOCK_PATH_KEY "lockpath"

int StoreLock::lock()
{
  string lockPath;
  int errCode = 0;
  struct stat statBuf;
  int fileDes;
  SBQLConfig config("Server");
  config.init(configFileName);

  if((errCode=config.getString( LOCK_PATH_KEY, lockPath ))!=0) {
  	cerr << "[Listener.Lock]-->Error while reading config: " << errCode << "\n"; 
  	return errCode;
  }

  if( ::stat( lockPath.c_str(), &statBuf)==0) {
    cerr << "[StoreLock]-->ERROR: Can't run server - backup is running. If not, delete file '" << lockPath.c_str() << "'\n";
    return -1; //TODO Error
  } else { 
    if((fileDes = ::open( lockPath.c_str(), O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR))<0)
    {
      cerr << "[SotreLock]-->Can't create file '" << lockPath << "': " << strerror(errno) << "n";
      return -1; //TODO Error
    }
  }
  ::close(fileDes);
  cerr << "[StoreLock]-->Done";
  return errCode;
}

int StoreLock::unlock()
{
  string lockPath;
  int errCode = 0;
  SBQLConfig config("Server");
  config.init(configFileName);  

  config.getString(LOCK_PATH_KEY, lockPath);

  if(::unlink(lockPath.c_str())<0)
  {
    cerr << "[StoreUnlock]-->Can't delete lock file '" << lockPath.c_str() << "': " << strerror(errno) << "\n";
    return errno;
  }
  cerr << "[StoreUnlock]-->Done";
  return errCode;
}
