#ifndef __INDEX_HANDLER_H__
#define __INDEX_HANDLER_H__

//#include <Store/NamedRoots.h>
#include <Store/Store.h>
#include <Indexes/BTree.h>
#include <Indexes/const.h>
#include <Indexes/RootEntry.h>
#include <Indexes/Monitor.h>
#include <Indexes/CachedNode.h>
#include <QueryParser/TreeNode.h>
#include <Indexes/EntrySieve.h>
#include <Indexes/Constraints.h>

using namespace std;
using namespace Store;
namespace Indexes
{

	class IndexHandler {
		friend class Tester;
		friend class NonloggedDataSerializer;
		
		public:
			//publiczne do testow
			typedef map<int, set<CachedNode*> > freeNodes_t;
			typedef map<int, set<IndexHandler*> > freeHandlers_t;
			typedef map<nodeAddress_t, CachedNode*> cacheMapT;
			
			enum handlerState {BUILDING, READY, DROPPING};
			
			handlerState state;
			
		private:
			string name, root, field;
			LogicalID* lid; //do kasowania
	
			BTree* tree;
			
			/** liczba transakci aktualnie wyszukujaca po tym indeksie */
			unsigned int readingTransaction;
			
			/** pierwszy wezel na liscie wszystkich wezlow tego indeksu */
			nodeAddress_t firstNode;
			
			/** ostatni wezel na liscie wszystkich wezlow tego indeksu. potrzebne tylko po to zeby usuwac caly indeks bez wczytywania pierwszego wezla */
			nodeAddress_t lastNode;
			
			/** adres ostatnio usunietego wezla. w tym wezle zapisany adres do nastepnego
			 *  jesli ten adres == NEW_NODE to znaczy ze jeszcze nic nie bylo usuniete
			 *  */
			static nodeAddress_t droppedNode;
			
			/** ostatni wezel na liscie usunietych */
			static nodeAddress_t droppedLastNode;
			
			static list<nodeAddress_t> droppedCachedNodes;
			
			/** adres nastepnego wezla ktory jeszcze nie byl uzyty. bedzie uzyty gdy zabraknie dotychczasowych wezlow */
			static nodeAddress_t nextFreeNodeAddr;
			
			/** ile wezlow aktualnie jest w cache'u */
			static unsigned long cachedCount;
			
			/** ile roznych wezlow w cache'u aktualnie jest uzywanych */
			static unsigned long usedCount;
			
			/** synchronizacja dostepu do zmiennych wyliczajacych adres nowego wezla */
			static Util::Mutex* newNodeMutex;
			
			/** synchronizuje operacje dyskowe na pliku */
			static Util::Mutex* IOMutex;
			
			/** dostep do mapy scache'owanych wezlow b-drzewa */
			static Monitor* cacheMonitor;
			
			/** synchronizuje dostep do statusu handlera i liczby czytajacych transakcji */
			Monitor* statusMonitor;
			
			/** mapa scache'owanych wezlow */
			static cacheMapT cache;
			
			//static cacheMapT freeNodes;
			
			
			
			/** wysokosc wolnych wezlow. lisc = 1; korzen = h */
			freeNodes_t freeNodes;
			
			
			/** najwieksza glebokosc wolnego wezla -> zbior drzew */
			static freeHandlers_t freeHandlers;
			
			/** zwraca CachedNode. dodaje go do cache'u jesli go nie ma.
			 *  uaktualnia tez metadane uzywanych obiektow
			 * first oznacza czy to jest pierwsze pobieranie wezla podczas danego wyszukiwania */
			int getCachedNode(CachedNode* &cNode, nodeAddress_t nodeID, bool first);
			
			/** niesynchronizowana. dodaje do mapy nowy obiekt*/
			static void addToCacheMap(CachedNode *cnode);
			
			/** niesynchronizowana. wyszukuje CachedNode w pamieci. jesli nie ma zwraca null */
			static CachedNode* cacheLookup(nodeAddress_t nodeID);
			
			/** niesynchronizowana. zwraca wezel ktory nalezy zrzucic na hdd */
			static CachedNode* choose2flush();
			
			/** niesynchronizowana. zwraca wezel ktory nalerzy zrzucic na hdd.
			 *  wybiera tylko sposrod wezlow w danym handlerze */
			CachedNode* localChoose2flush(freeHandlers_t::iterator handlerIt);
			
			/** deskryptor pliku z indeksami */
			static int index_fd;
			
			/** synchronizowana. zrzuca cachedNode na hdd */
			static int flush(CachedNode* cnode);
			
			/** jw, nie synchronizowana */
			static int flushNotSync(CachedNode* cnode);
			
			/** niesynchronizowana. wczytuje z dysku wezel nodeID do zaallokowanej pamieci wskazywanej przez node */
			static int readNodeNotSync(Node* node, nodeAddress_t nodeID);
			
			/** jesli cache jest pelny to zrzuca jeden wezel na hdd
			 *  metoda zaklada ze jest wewnatrz cacheMonitor'a */
			static int flushIfNecessary();
			
			/** przeprowadza czyszczenie jesli wezel jest oznaczony jako wymagajacy tego.
			 *  powinna byc wywolywana gdy wezel jest juz wczytany */
			//void cleanNode(CachedNode* cnode);
			
			/** oznacza ze wezel staje sie nieuzywany przez zadna transakcje */
			void setFree(CachedNode* cnode);
			
			/** oznacza ze wezel zaczyna byc uzywany przez jakas transakcje */
			void setNotFree(CachedNode* cnode);
			
			/** oznacza ze zmienila sie glebokosc najglebszego wolnego wezla w tym handlerze */
			void changePriority(int oldp,  int newp);
			
			/** usuwa ten handler (o zadanym priorytecie) z mapy */
			void removeHandler(int priority);
			
			/** dodaje ten handler do mapy pod zadanym priorytecie */
			void addHandler(int priority);
			
			/** zwraca usuniety wezel ktory jeszcze jest w cache'u. bedzie on splukiwany */
			static CachedNode* getDropped2flush();
			
			/** dodaje nowy wezel do listy wezlow w tym indeksie */
			int addToList(Node* node);
			
			/** usuwa wezel z listy wezlow w tym indeksie */
			int removeFromList(Node* node);
		public:
			
			/** atomowo zmienia stan z from na to. jesli stan nie jest from to rzucany jest blad ENoIndex
			 *  jesli usuniecie jest niemozliwe to usypia czekajac az wszyscy przestana go uzywac */
			int changeState(handlerState from, handlerState to);
			
			/** atomowo zmienia ilosc transakcji uzywajacych danego indeksu
			 *  jesli indeks nie jest ready to blad ENoIndex 
			 *  buildingTransaction okresla czy wejscia dokonuje transakcja ktora wlasnie buduje ten indeks */
			int userEnter(bool buildingTransaction = false);
			
			/** atomowo oznacza ze transakcja konczy uzywac indeks
			 *  jesli to byla ostatnia transakcja to budzony jest czekajacy watek (o ile jest) usilujacy usunac indeks
			 *  buildingTransaction okresla czy wejscia dokonuje transakcja ktora wlasnie buduje ten indeks */
			int userLeave(bool buildingTransaction = false);
			
			static const char* printHandlers();
			
			/** lid - LogicalID roota ktory zawiera metadane tego indeksu */
			IndexHandler(string name, string root, string field, LogicalID* lid);
			IndexHandler(string name, string root, string field);			
			/** pomaga konstruktorom w inicjalizacji */
			void indexHandler(string name, string root, string field);
			
			~IndexHandler();
			string getName();
			string getRoot();
			string getField();
			
			void setLid(LogicalID* lid);
			
			/** zwraca stringa okreslajacego typ indeksu (int/double/string) do wypisania klientowi */
			string getType();
			
			LogicalID* getLogicalID();
			
			/** identyfikator indeksu z logicalId */
			lid_t getId();
			
			/** zaklada nowy indeks na dysku.
			 *  po utworzeniu nowego IndexHandlera trzeba wywolac te metode lub load */
			int createNew(Comparator* comp);
			
			/** tworzy nowy korzen */
			int reset();
			
			/** wczytuje z bazy stary indeks.
			 *  po utworzeniu nowego IndexHandlera trzeba wywolac (te metode i ustawic dane w drzewie) lub createNew */
			void load(Comparator* comp);
			
			/** dostarcza z dysku wlasciwy wezel lub dorzuca nowy gdy nodeNumber = 0 */
			int getNode(CachedNode *&cNode, nodeAddress_t nodeID, bool first = false);

			/** synchronizowana. wczytuje z dysku wezel nodeID do zaallokowanej pamieci wskazywanej przez node */
			static int readNode(Node* node, nodeAddress_t nodeID);
			
			/** oznacza ze wezel nie jest juz potrzebny danej transakcji. nie oznacza skasowania
			 *  setFree oznacza czy wezel po zwolnieniu ma byc dostepny dla innych transakcji jako wolny wezel
			 *  unlock mowi czy nalezy zwolnic blokade do zapisu/odczytu. aktualizowanie listy wezlow odbywa sie bez zakladania blokad */
			int freeNode(CachedNode* node, bool setFree = true, bool unlock = true);
			
			/** wyrzuca wezel z cache'u i zrzuca na dysk */
			static int removeAndFlush(CachedNode* flushCNode);
			
			//int addEntry(Entry* entry, ObjectPointer *op, DataValue *value);
			int addEntry(DataValue *value, RootEntry* entry, bool buildingTransaction = false);
			//int addEntry(TransactionID* tid, ObjectPointer *op);
			
			/** inicjalizacja cache'u */
			static int init(string indexFileName);
			
			/** zamyka plik z danymi indeksow, niszczy mutexy itp */
			static int destroy();
			
			int dropNode(CachedNode* cnode);
			
			/** resetuje caly cache. niczego nie zrzuca na dysk */
			static void clearCache();
			
			/** splukuje cala zawartosc cache'a */
			static int flushAll();
			
			/** usuwa wszystkie wezly z tego indeksu */
			int drop();
			
			/** zmienia priorytety scache'owanych wezlow,
			 *  zaklada ze rootSem jest zablokowany do zapisu */
			void rootChange(int oldLevel, int newLevel);
			
			int search(Constraints* constr, EntrySieve* sieve);
	};
	
	

}
#endif /*INDEXHANDLER_H_*/
