#include <string>
#include <iostream>
#include <getopt.h>
#include <netdb.h>
#include <Server/Server.h>
#include <Store/DBStoreManager.h>
#include <Indexes/IndexManager.h>
#include <QueryExecutor/InterfaceMaps.h>
#include <QueryExecutor/QueryBuilder.h>
#include <QueryExecutor/OuterSchema.h>
#include <QueryExecutor/ClassGraph.h>

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

/*in network order */
uint32_t get_host_ip(const string& hostname, ErrorConsole &err_cons)
{
	struct hostent *hp;
	hp = gethostbyname(hostname.c_str());
	if (!hp){
		severe_print(err_cons, "Could resolve hostname, check your configuration file");
		exit(1);
	}
	uint32_t res;
	memcpy(&res, hp->h_addr, 4);
	return res;
}

int main(int argc, char **argv)
{
	string hostname;
	int port;
	parse_args(argc, argv, &hostname, &port);

        SBQLConfig config("Server");
	int error = config.init();
	if (error) {
	    cerr << "Config init failed with code " << error << "\n";
	    exit(1);
	}
	signal(SIGPIPE, SIG_IGN);
	ErrorConsole &con(ErrorConsole::get_instance(EC_SERVER));
	LogManager::checkForBackup();
	info_print(con, "Initializing Log manager and Store manager");
	LogManager *lm = new LogManager();
	lm->init();
	DBStoreManager *sm = new DBStoreManager();
	sm->init(lm);
	LockManager::init();
	TransactionManager::init(sm, lm);
	sm->setTManager(TransactionManager::getHandle());
	debug_print(con, "this " << "is " << "a test for " << 5);
	info_print(con, "Starting Store manager...");
	sm->start();
	info_print(con, "Starting Log manager...");
	lm->start(sm);
	QueryBuilder::startup();
	info_print(con, "Starting Index manager...");
	Indexes::IndexManager::init(LogManager::isCleanClosed());
	ClassGraph::ClassGraph::init(-1);
	Schemas::InterfaceMaps::Instance().init(); //must be called after ClassGraph::init()
	Schemas::OuterSchemas::Instance().init();

	::Server::Server serv(get_host_ip(hostname, con), htons(port), config);
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
	return 0;
}
