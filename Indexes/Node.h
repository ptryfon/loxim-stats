#ifndef __NODE_H__
#define __NODE_H__

#include <string.h>
#include "limits.h"
#include "const.h"

using namespace std;
namespace Indexes {

	enum NodeAccessMode {NODE_WRITE, NODE_READ};
	const int ROOT_PRIORITY = 0;
	
	/** kolejne wpisy wewnatrz wezla */
	struct NodeEntry {
		nodeAddress_t child; /** wskaznik do dziecka (NodeEntry number+1) */
		lid_t logicalID; /** czesc klucza */
		
		//dalej bedzie klucz zmiennej dlugosci. format klucza bedzie znany tylko Comparatorowi
		
		string toString();
	};
	
	/** struktura reprezentujaca wezel b-drzewa */
	struct Node {
		union {
			nodeAddress_t id; /** identyfikator wezla */
		//	nodeAddress_t nextDroppedNode; /** w usunietym wezle: identyfikator nastepnego usunietego wezla */ 
		};
		bool leaf; /** czy wezel jest lisciem */
		nodeSize_t left; /** ile bajtow jest jeszcze niewykorzystywanych */
		union {
			nodeAddress_t nextLeaf; /** dla lisci, nastepny lisc w kolejnosci */
			nodeAddress_t firstChild; /** dla wezlow wewnetrznych, wskaznik na skrajnie lewego syna */
		};
		/** poprzedni wezel na liscie wezlow danego indeksu */
		nodeAddress_t prevNode;
		/** nastepny wezel na liscie wezlow danego indeksu */
		nodeAddress_t nextNode;
		
		/** glebokosc w b-drzewie */
		int level;
		
		//bool cleanNeeded;
		
		// dalej beda kolejne NodeEntry numerowane od 0
		
		/** zwraca entry z danego node'a o numerze number (numerowane od 0). nie sprawdza zakresu, jesli bedzie bledny to seqfault */
		NodeEntry* getEntry(int number);
		
		/** zwraca adres tuz za naglowkiem Node'a */
		char* afterHeader();
		
		/** po zaallokowaniu nowego Node'a trzeba zainicjowac mu ilosc wpisow i rozmiar */
		void init();

		/** inicjalizacja wezla, nadanie mu id. metoda zwraca id nastepnego usunietego wezla */
		nodeAddress_t init(nodeAddress_t id);
		
		/** wskaznik tuz za ostatni wpis w wezle. jesli wezel nie jest pelny to tam mozna dokladac */
		char* afterAddr();
		
		/** ile jest miejsca miedzy dwoma wskaznikami. jak duzo pamieci trzeba przesunac */
		static nodeSize_t distanceInNode(void* smaller, void* greater);
		
		string headerToString();
		
		/** ilosc danych w wezle (oprocz struktury wezla) */
		nodeSize_t size();
		
	};
	
}

#endif /*__NODE_H__*/
