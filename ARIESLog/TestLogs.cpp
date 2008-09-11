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
	LogManager *lm = new LogManager();
	*ec << "LogManager created";
	sm->init(lm);
	*ec << "Store initiated";
	lm->init();
	*ec << "LogManager initiated";
	lm->start();
	*ec << "LogManager started";
	
	record = new BeginLogRecord(0, 2);
	record->write(lm->fd);
	ec->printf("Log record written. LSN = %d, undoNxtLSN = %d\n", record->getLSN(), record->getUndoNxtLSN());
	delete record;
	record = new UpdateLogRecord(0, 2, new DBLogicalID(7), "ptak", new DBDataValue("kanarek"), new DBDataValue("sroka"));
	record->write(lm->fd);
	ec->printf("Log record written. LSN = %d, undoNxtLSN = %d\n", record->getLSN(), record->getUndoNxtLSN());
	delete record;
	
	LogRecord::read(lm->fd, 16, record);
	ec->printf("Log record written. LSN = %d, undoNxtLSN = %d\n", record->getLSN(), record->getUndoNxtLSN());
	delete record;
	
	lm->beginTransaction(2, LSN);
	ec->printf("Transaction started. Last LSN = %d\n", LSN);
	lm->write(2, new DBLogicalID(7), "ptak", new DBDataValue("kanarek"), new DBDataValue("sroka"), LSN);
	ec->printf("Update. Last LSN = %d\n", LSN);
	lm->write(2, new DBLogicalID(7), "ptak", new DBDataValue("kanarek"), new DBDataValue("sroka"), LSN);
	ec->printf("Update. Last LSN = %d\n", LSN);
	lm->rollbackTransaction(2, LSN);
	
	unsigned a = 101;
	lm->shutdown(a);
	*ec << "LogManager shut down";
	delete lm;
	*ec << "Finished";
	delete ec;
}
