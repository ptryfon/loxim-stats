#ifndef _LOGS_GLOBALS_CPP
#define _LOGS_GLOBALS_CPP

#include "LogThread.h"
#include "LogRecord.h"

map< int, LogRecord * > LogRecord::dictionary;
unsigned LogRecord::idSeq = 0;

#endif
