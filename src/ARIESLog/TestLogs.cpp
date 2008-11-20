#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include "SystemStats/ConfigStats.h"
#include "SystemStats/AllStats.h"
#include "../Config/SBQLConfig.h"
#include "Errors/Errors.h"
#include "Errors/ErrorConsole.h"
#include "../Store/Store.h"
#include "../Store/DBStoreManager.h"
#include "Logs.h"


using namespace std;
using namespace Errors;
using namespace Logs;
using namespace Config;

int main()
{
	int res;
	unsigned int LSN;
	LogRecord* record;
	
	SBQLConfig c("ARIESLog");
	c.init("example.conf");
	
	ErrorConsole *ec = new ErrorConsole("ARIESLog");
	ec->init(1);
	*ec << "Starting";
	DBStoreManager *sm = new DBStoreManager();
	*ec << "Store created";
	LogManager *log = new LogManager();
	*ec << "LogManager created";
	sm->init(log);
	*ec << "Store initiated";
	log->init();
	*ec << "LogManager initiated";
	log->start();
	*ec << "LogManager started";
	
	*ec << "testing LogRecord...";
	record = new BeginLogRecord(0, 2);
	record->write(log->fd);
	ec->printf("Log record written. LSN = %d, undoNxtLSN = %d\n", record->getLSN(), record->getUndoNxtLSN());
	delete record;
	record = new UpdateLogRecord(0, 2, new DBLogicalID(7), "ptak", new DBDataValue("kanarek"), new DBDataValue("sroka"));
	record->write(log->fd);
	ec->printf("Log record written. LSN = %d, undoNxtLSN = %d\n", record->getLSN(), record->getUndoNxtLSN());
	delete record;
	record = new CompensationLogRecord(16, 2, new DBLogicalID(7), 0);
	record->write(log->fd);
	ec->printf("Log record written. LSN = %d, undoNxtLSN = %d\n", record->getLSN(), record->getUndoNxtLSN());
	delete record;
	record = new EndLogRecord(80, 2);
	record->write(log->fd);
	ec->printf("Log record written. LSN = %d, undoNxtLSN = %d\n", record->getLSN(), record->getUndoNxtLSN());
	delete record;
	
	LogRecord::read(log->fd, 16, record);
	ec->printf("Log record written. LSN = %d, undoNxtLSN = %d\n", record->getLSN(), record->getUndoNxtLSN());
	delete record;
	
	*ec << "testing LogManager...";
	log->beginTransaction(2, LSN);
	ec->printf("Transaction started. Last LSN = %d\n", LSN);
	log->write(2, new DBLogicalID(7), "ptak", new DBDataValue("kanarek"), new DBDataValue("sroka"), LSN);
	ec->printf("Update. Last LSN = %d\n", LSN);
	log->write(2, new DBLogicalID(7), "ptak", new DBDataValue("kanarek"), NULL, LSN);
	ec->printf("Update. Last LSN = %d\n", LSN);
	log->rollbackTransaction(2, LSN);
	
	log->beginTransaction(3, LSN);
	ec->printf("Transaction started. Last LSN = %d\n", LSN);
	log->commitTransaction(3, LSN);
	ec->printf("Transaction commited. Last LSN = %d\n", LSN);
	
	unsigned a = 101;
	log->shutdown(a);
	*ec << "LogManager shut down";
	delete log;
	*ec << "Finished";
	delete ec;
}
