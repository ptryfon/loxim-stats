#include <iostream>
#include <getopt.h>
#include "XMLcommon.h"
#include "XMLimport.h"
#include "XMLexport.h"
#include "StoreLock.h"


using namespace std;
using namespace XMLIO;

enum Function {
	IMPORT = 0,
	EXPORT = 1
};
		
void printHelp(string err) {
	cerr << "Uzycie: XMLimport [OPCJE] PLIK_KONFIG PLIK_XML\n";
	cerr << "\n";
	cerr << "Dostepne opcje:\n";
	cerr << "-i       - import danych\n";
	cerr << "-e       - eksport danych (domyslnie)\n";
	cerr << "-v       - wyswietlanie dodatkowych informacji diagnostycznych na wyjsciu stderr\n";
	cerr << "-f ext   - obsluga danych w formacie rozszerzonym\n";
	cerr << "-f pure  - obsluga dowolnych danych XML (domyslnie) \n";
	cerr << "-h       - wyswietla informacje o sposobie uzycia\n";
	cerr << "\n";
	
	cerr << err << '\n';
}

int main(int argc, char* argv[]) {
	int verboseLevel = 0;
	Function function = EXPORT;
	FileFormat fileFormat = FORMAT_SIMPLE;
	string xmlPath;
	string configFileName;	
	
	if (argc < 2) {
		printHelp("Za malo parametrow");
		return -1;
	}
	
	int nextOption;
	
	string shortOptions = "ievf:h"; // Litery prawidlowych krotkich opcji
	do
	{
	    nextOption = ::getopt( argc, argv, shortOptions.c_str());
	    switch( nextOption )
	    {	
	      case 'i' : //import
		      function = IMPORT;
		      break;
	      case 'e' : //export
			function = EXPORT;	
			break;
	      case 'v' : //verbose
	      	verboseLevel++;
	        break;
	      case 'f': // verbose
	      	cerr << "B!" << optarg << "!\n";
	      	if (strcmp(optarg, "ext") == 0) {
	      		fileFormat = FORMAT_EXTENDED;
	      		cerr << "!!!!\n";
	      	}
	      	else if (strcmp(optarg, "pure") == 0) fileFormat = FORMAT_SIMPLE;
	        break;
	
	      case 'h': // nieprawidlowa opcja
	        printHelp("");
	        return -1;
	
	      case -1: // koniec opcji
	        break;
	
	      default: // cos innego, niespodziewane
	      	printHelp("");
	      	return -1;
	    }
    } while( nextOption != -1 );	
	
	configFileName = argv[argc - 2];
	xmlPath = argv[argc - 1];
	
	SBQLConfig* config = new SBQLConfig("Store");
	config->init(configFileName);
	ErrorConsole* ec = new ErrorConsole("Store");
	ec->init(1);
	LogManager* log = new LogManager();
	log->init();
	
	StoreLock storeLock(configFileName);
	
	if (storeLock.lock() != 0) return 0;
	
	DBStoreManager* store = new DBStoreManager();
	store->init(log);
	store->setTManager(TransactionManager::getHandle());
	store->start();		
	
	if (function == IMPORT) {
		XMLImporter* imp = new XMLImporter(store, verboseLevel, fileFormat);
		imp->make(xmlPath);
	} else {
		XMLExporter* exp = new XMLExporter(store, verboseLevel, fileFormat);
		exp->make(xmlPath);
	}
	
	if (storeLock.unlock() != 0) return 0;
	store->stop();
	return 0;
}
