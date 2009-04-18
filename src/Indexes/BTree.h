#ifndef __BTREE_H__
#define __BTREE_H__

//#include <Indexes/IndexHandler.h>
#include <iostream>
#include <string.h>
#include <Indexes/const.h>
#include <Indexes/Node.h>
#include <Indexes/RootEntry.h>
#include <Store/Store.h>
#include <TransactionManager/Semaphore.h>
#include <Indexes/CachedNode.h>
//#include <QueryParser/TreeNode.h>

//using namespace QParser;

namespace Indexes
{
	
	class IndexHandler;
	class CachedNode;
	class NodeEntry;
	class Comparator;

	enum NodeCapacityType {SPLIT_NEEDED, DELETE_NEEDED, PROPER_SIZE};
	
	class BTree {
		friend class Tester;
		
		private:
			nodeAddress_t rootAddr;
			Comparator* defaultComparator;
			int rootLevel;
			
			/** dodaje do drzewa o niepelnym korzeniu. zaklada ze node jest juz wczytany
			 *  po wykonaniu operacji zwalnia wezel */ 
			int addEntry(CachedNode* node, Comparator *filledComparator);
	
			/** tak samo jak splitChild ale z dodatkowa informacja czy operowac bedziemy na NodeEntry czy na RootEntry */
			template <class I>
			int splitChild(CachedNode* parent, CachedNode* child, char* toChild, I* type);
			
			/** rozbija child wzgledem srodkowego klucza, ktory jest przesuwany do niepelnego ojca (parent).
			 *  toChild jest wskaznikiem do miejsca tuz za wpisem wskazujacym na child.
			 *  czyli tam gdzie powinien sie znalezc dodatkowy wpis 
			 *  zwalnia wezly child oraz nowo utworzony wezel */ 
			int splitChild(CachedNode* parent, CachedNode* child, char* toChild);
			
			/** inny wezel staje sie rootem. nie synchronizowana */
			void rootChange(CachedNode* newRoot, CachedNode* oldRoot);
			
			/** sprawdza czy wezel moze przyjac kolejny klucz. jesli nie to go czysci i ponownie sprawdza */
			NodeCapacityType canTakeExtraKey(CachedNode* node);
			
			/** zamienia klucz z wpisu entry z wezla node, na klucz z comparatora comp */
			void changeKey(Node* node, NodeEntry* entry, Comparator* comp);
			
			/** scala wezly to (z lewej) z from (z prawej). i uaktualnia wskazniki na nie.
			 *  przenosi wpisy do wezla z lewej strony. usuwa wezel from */
			int merge(CachedNode* to, CachedNode* from, CachedNode* parent, NodeEntry* fromPtrEntry);
			
			/** wezel cchild ma zbyt malo wpisow. metoda stara sie albo zbalansowac albo usunac jakis wezel
			 *  jesli cchild zostanie usuniety to podpiety bedzie pod niego wezel z ktorym nastapilo scalanie
			 *  prev - wpis wskazujacy na poprzedniego brata cchild (lub null jesli cchild pierwszy)
			 *  next - wpis wskazujacy na nastepnego brata cchild (lub null jesli cchild ostatni)
			 *  childEntry - wpis wskazujacy na cchild
			 *  cparent - ojciec wezla cchild */
			int remove(NodeEntry* prev, CachedNode* &cchild, NodeEntry* next, NodeEntry* childEntry, CachedNode* cparent);
			
			/** czy te dwa wezly mozna zbalansowac */
			bool isBalancePossible(Node* from, Node* to);

			/** balansuje wezly. przenosi dane z poprzednika do nastepnika */
			template <class I>
			void balanceForward(Node* from, Node* to, Node* parent, NodeEntry* toPtrEntry, I* entryType);
			
			/** balansuje wezly. przenosi dane z nastepnika do poprzednika */
			template <class I>
			void balanceBack(Node* to, Node* from, Node* parent, NodeEntry* fromPtrEntry, I* entryType);
			
			/** sprawdza czy wezel trzeba usunac? */
			NodeCapacityType checkForDelete(Node* node);
			
			/** zamienia klucz w wezle parent, we wpisie entry */
			template <class I>
			void changeParentKey(I* entryWithKey, Node* parent, NodeEntry* entry);
			
			/** wylicza ile danych powinno zostac przeniesione podczas balansowania */
			nodeSize_t balanceSize(nodeSize_t secondSize, Node* node);
			
			/** sprawdza czy root powinien zostac usuniety */
			bool shouldRemoveRoot(Node* root);
			
			/** usuwa roota, zaklada ze nie jest lisciem i ze jest wewnatrz locka */
			int removeRoot(CachedNode* root);
			
		public:
			BTree(Comparator *defaultComparator, IndexHandler* ih);
			virtual ~BTree();
			
			/** szereguje dostep do korzenia */
			SemaphoreLib::Semaphore *rootSem;
			
			//open(char* file)
			 /** jesli wczytywany jest instniejacy index to musi byc zainicjalizowany */
			void init(nodeAddress_t rootAddr, int rootLevel);
			
			/** dostaje comparator z zainicjowanym odniesieniem. zwraca kod bledu, znaleziony wezel, offset do membera
			 *  zwraca skrajnie lewy wiekszy lub rowny wpis w drzewie,  
			 *  zwalnia wszystkie wezly oprocz liscia w ktorym szukal wyniku
			 * jesli w danym wezle zawartosc comparatora jest najwieksza to entryResult bedzie null
			 * */
			int find(Comparator *filledComparator, CachedNode *&nodeResult, RootEntry *&entry);
			
			/** tworzy nowe b-drzewo. dostaje zaallokowana strone i robi tam pusty wezel roota */
			int createRoot();
			
			/** zwraca nowy pusty comparator odpowiedni dla danego drzewa */
			Comparator* getNewComparator();
			
			/** dodaje zawartosc comparatora do b-drzewa */
			int addEntry(Comparator *filledComparator);

			IndexHandler *ih;
			
			/** zwraca priorytet korzenia */
			int rootPriority();
			
			/** ustawia nowy priorytet korzenia */
			void setRootPriority(int level);
			
			/** znajduje punkt podzialu przy balansowaniu wezlow.
			 *  frontTransfer - czy transfer zaczynamy od poczatku wezla from.
			 *  lastEntry - ostatni wpis ktory znajdzie sie w poprzedniku. czyli jesli przesuwamy do nastepnika to ostatni nieskopiowany a jesli przesuwamy do poprzednika to ostatni skopiowany
			 *  zwraca tez ostatni wpis przed podzialem */
			template <class I>
			char* getBalanceBorder(Node* from, nodeSize_t toSize, I* &lastEntry, bool frontTransfer, NodeEntry* secondPtrEntry);
			
			friend class NonloggedDataSerializer;
	};
	
}

#endif /*__BTREE_H__*/
