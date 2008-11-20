#include <LoximServer/LoximServer.h>
#include <string>
#include <iostream>
#include <getopt.h>
#include <Indexes/IndexManager.h>
#include <QueryExecutor/InterfaceMaps.h>

using namespace std;

#define DEFAULT_PORT 2000
#define DEFAULT_HOST "localhost"

struct option options[] = { { "host", 1, NULL, 'H' }, { "port", 1, NULL, 'p' },
		{ NULL, 0, NULL, 0 } };

void print_usage(char *prog_name) {
	printf("Usage: %s [-H host] [-p port]\n", prog_name);
}

void parse_args(int argc, char **argv, string *hostname, int *port) {
	int opt;
	*hostname = DEFAULT_HOST;
	*port = DEFAULT_PORT;
	while ((opt = getopt_long(argc, argv, "H:p:", options, NULL))
			!= EOF) {
		switch (opt) {
		case 'H':
			*hostname=optarg;
			break;
		case 'p':
			*port=atoi(optarg);
			break;
		default:
			print_usage(argv[0]);
			exit(0);
			break;
		}
	}
}

int main(int argc, char **argv) {
	string hostname;
	int port;
	parse_args(argc, argv, &hostname, &port);

        SBQLConfig config("Server");
	int error = config.init("Server.conf");
	if (error) {
	    fprintf(stderr, "Config init failed with %d\n", error);
	    exit(1);
	}
	ErrorConsole con("Server");
//XXX GT HARDCODING!! changed to be read from conf file
        error = con.init();
	if (error) {
	    fprintf(stderr, "ErrorConsole init failed with %d\n", error);
	    exit(2);
	}
	LogManager::checkForBackup();
	con.printf("[Listener.Start]--> Initializing Log manager and Store manager \n");
	LogManager *lm = new LogManager();
	lm->init();
	DBStoreManager *sm = new DBStoreManager();
	sm->init(lm);
	LockManager::init();
	TransactionManager::init(sm, lm);
	sm->setTManager(TransactionManager::getHandle());
	con.printf("[Listener.Start]--> Starting Store manager.. \n");
	sm->start();
	con.printf("[Listener.Start]--> Starting Log manager.. \n");
	lm->start(sm);
	QueryBuilder::startup();
	con.printf("[Listener.Start]--> Starting Index manager.. \n");
	Indexes::IndexManager::init(LogManager::isCleanClosed());
	ClassGraph::ClassGraph::init(-1);
	Schemas::InterfaceMaps::Instance().init(); //must be called after ClassGraph::init()

	::LoximServer::LoximServer serv(hostname, port, &config);
	if (!serv.prepare()){
		serv.main_loop();
		Indexes::IndexManager::shutdown(); //tutaj nie powinno juz byc zadnych aktywnych transakcji
		QueryBuilder::shutdown();
		sm->stop();
		unsigned idx;
		lm->shutdown(idx);//nie wiem czy tu to ma znaczenie,
		//ale z reguly najlepiej niszczyc w odwrotnej kolejnosci niz sie otwieralo.
		Schemas::InterfaceMaps::Instance().deinit();
		ClassGraph::ClassGraph::shutdown();	
		delete TransactionManager::getHandle();
	}
	else
		cout << "prepare failed" << endl;
	return 0;
}
