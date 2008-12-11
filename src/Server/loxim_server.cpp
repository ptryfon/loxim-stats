#include <Server/Server.h>
#include <string>
#include <iostream>
#include <getopt.h>
#include <Indexes/IndexManager.h>
#include <QueryExecutor/InterfaceMaps.h>
#include <QueryExecutor/OuterSchema.h>

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
	int error = config.init();
	if (error) {
	    cerr << "Config init failed with code " << error << "\n";
	    exit(1);
	}
	ErrorConsole &con(ErrorConsole::get_instance("Server"));
	LogManager::checkForBackup();
	con(V_INFO) << "Initializing Log manager and Store manager\n";
	LogManager *lm = new LogManager();
	lm->init();
	DBStoreManager *sm = new DBStoreManager();
	sm->init(lm);
	LockManager::init();
	TransactionManager::init(sm, lm);
	sm->setTManager(TransactionManager::getHandle());
	con(V_INFO) << "Starting Store manager...\n";
	sm->start();
	con(V_INFO) << "Starting Log manager...\n";
	lm->start(sm);
	QueryBuilder::startup();
	con(V_INFO) << "Starting Index manager...\n";
	Indexes::IndexManager::init(LogManager::isCleanClosed());
	ClassGraph::ClassGraph::init(-1);
	Schemas::InterfaceMaps::Instance().init(); //must be called after ClassGraph::init()
	Schemas::OuterSchemas::Instance().init();

	::Server::Server serv(hostname, port, config);
	if (!serv.prepare()){
		serv.main_loop();
		Indexes::IndexManager::shutdown(); //tutaj nie powinno juz byc zadnych aktywnych transakcji
		QueryBuilder::shutdown();
		sm->stop();
		unsigned idx;
		lm->shutdown(idx);//nie wiem czy tu to ma znaczenie,
		//ale z reguly najlepiej niszczyc w odwrotnej kolejnosci niz sie otwieralo.
		Schemas::OuterSchemas::Instance().deinit();
		Schemas::InterfaceMaps::Instance().deinit();
		ClassGraph::ClassGraph::shutdown();	
		delete TransactionManager::getHandle();
	}
	else
		con(V_SEVERE_ERROR) << "Couldn't connect to socket\n";
	return 0;
}
