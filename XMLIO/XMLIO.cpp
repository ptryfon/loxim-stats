#include <iostream>
#include <getopt.h>
#include "XMLimport.h"
#include "XMLexport.h"

using namespace std;

void printHelp() {
	cerr << "Uzycie: XMLIO [OPCJE] PLIK_XML\n";
	cerr << "\n";
	cerr << "Dostepne opcje:\n";
	cerr << "-i       - import danych\n";
	cerr << "-e QUERY - eksport danych bedacych wynikiem zapytania QUERY\n";
	cerr << "-s host  - adres serwera bazy danych (domyslnie 127.0.0.1)\n";
	cerr << "-p port  - port serwera bazy danych (domyslnie 6543)\n";
	cerr << "-v       - wyswietlanie dodatkowych informacji diagnostycznych na wyjsciu stderr\n";
	cerr << "-f db    - obsluga danych w wewnetrznym formacie (domyslnie)\n";
	cerr << "-f pure  - obsluga dowolnych danych XML\n";
	cerr << "-h       - wyswietla informacje o sposobie uzycia\n";
	cerr << "\n";
	
}

enum FileFormat {
	FORMAT_PLAIN = 0,
	FORMAT_XML = 1
};

int main(int argc, char* argv[]) {
	bool isImport = false;
	bool isExport = false;
	int verboseLevel = 0;
	string host = "127.0.0.1";
	string fileName = "";
	string query;
	int port = 6543;
	FileFormat format;
	
	int nextOption;
	
	string shortOptions = "ies:p:vf:"; // Litery prawidlowych krotkich opcji
	do
	{
	    nextOption = ::getopt( argc, argv, shortOptions.c_str());
	    switch( nextOption )
	    {
	      case 'i': // import danych
	      	isImport = true;
	      	break;
	
	      case 'e': // eksport danych
	      	isExport = true;
	      	query = optarg;
	        break;
	
	      case 's': // adres serwera
	        host = optarg;
	        break;
	
	      case 'p': // port serwera
	      	port = atoi(optarg);
	        break;
	
	      case 'v' : // restore flag
	      	verboseLevel++;
	        break;
	
	      case 'f': // verbose
	      	if (optarg == "db") format = FORMAT_PLAIN;
	      	else if (optarg == "pure") format = FORMAT_XML;
	        break;
	
	      case 'h': // nieprawidlowa opcja
	        printHelp();
	        return -1;
	
	      case -1: // koniec opcji
	        break;
	
	      default: // cos innego, niespodziewane
	      	printHelp();
	      	return -1;
	    }
    } while( nextOption != -1 );
    
    if (argc > 1) (fileName = argv[argc - 1]);
    
    /* TODO - weryfikacja danych */
      
    if (isImport) {
		XMLIO::XMLImporter *importer;
		importer = new XMLIO::XMLImporter(fileName, verboseLevel);
		// int result = importer->dumpToServer(host, port); - TESTY
		int result = importer->dumpToFile(host+".out");
		delete importer;
		return result;    	
    }
    
    if (isExport) {
    	XMLIO::XMLExporter *exporter;
    	exporter = new XMLIO::XMLExporter(host, port, verboseLevel);
    	int result = exporter->doExport(query, fileName);
    	delete exporter;
    	return result;
    }
	
	return 0;	
}
