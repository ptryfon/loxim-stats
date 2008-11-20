#ifndef COMPARATOR_H_
#define COMPARATOR_H_

#include <string.h>
#include <Indexes/const.h>
#include <Indexes/Node.h>
#include <Store/Store.h>
#include <QueryParser/TreeNode.h>

namespace QParser {
//		class QueryNode;
}


using namespace std;
namespace Indexes {

	//Comparatory
	
	class Comparator {
		protected:
			/** tu bedzie trzymana wartosc do wstawienia */
			RootEntry* rootEntry;
			bool hasValue;
			Comparator();
			
			/** czy zawartosc comparatora jest wieksza od zawartosc pamieci za *item */
			virtual int compare (char* item, unsigned int itemSize, lid_t lidDiff)=0;
			
			/** wstawia klucz z opisem. itemSize to rozmiar struktury ktorej dotyczy klucz */
			virtual void putKey(char* where, nodeEntry_off_t itemSize)=0;
			
			/** nie wymagaja zeby comparator byl wypelniony */
			
			/** majac adres item'a i jego rozmiar zwraca adres nastepnego item'a w wezle */
			virtual char* getNextItem(char* itemAddress, unsigned int itemSize)=0;

			/** zwraca NodeItem o numerze entryNumber. liczone od 0 */
			virtual char* getItem(int itemNumber, char* item0Address, unsigned int itemSize)=0;
			
			/** zwraca klucz (w posteci stringa) zwiazany z danym item'em o danym rozmiarze */
			virtual string keyToString(char* item, unsigned int itemSize)=0;
			
			/** ustawia klucz comparatora na taki jaki jest za item */
			virtual void getValue(char* item, unsigned int itemSize)=0;
			
			static size_t normLen(size_t valLen);
			
			/** zwraca minimalna dopuszczalna zajetosc wezla */
			virtual nodeSize_t constInit();
			
			/** do zapisania typu indeksu do bazy */
			enum comparatorType {STRING_C = 0, INT_C = 1, DOUBLE_C = 2};
			
			virtual comparatorType getType() const =0;
			
		public:
			
			nodeSize_t MIN_NODE_SIZE_ALLOWED;
			
			/** porownuje swoja zawartosc z kluczem za item'em.
			 *  < 0 jesli zawartosc comparatora jest mniejsza niz Item */
			template <class I>
			int compare (I* item, bool useLid = true);			
		
			/** czy zawartosc comparatora jest mniejsza niz wartsc klucza zwiazanego z item */
			template <class I>
			bool lessThan (I* item);
			
			/** uwzglednia lidy bo wywolywany z btree */
			template <class I>
			bool greaterThan (I* item);
			
			/** nie uwzglednia lid'ow bo wywolywany z travellera */
			template <class I>
			bool greaterOrEqual (I* item);
	
			template <class I>
			bool lessOrEqual (I* item);
	
			template <class I>
			bool equal (I* item);
	
			template <class I>
			bool infinityReached(I* item);
	
			/** ile miejsca potrzeba (razem ze struktura Item) zeby wstawic zawartosc comparatora */
			template <class I>
			nodeEntry_off_t getSpaceNeeded(I* item);
			
			//Comparator(RootEntry* entry);
			/** nie usuwa obiektow ktore dostal w konstruktorze */
			virtual ~Comparator(){}

			/** wstawia do liscia RootEntry (z comparatora) wraz z kluczem */
			void putValue(RootEntry* where);
			
			/** wstawia klucz (z comparatora) za struktura I, sam wpis pozostaje niezmieniony */
			template <class I>
			void putKey(I* where);
			
			/** ile miejsca potrzeba (oprocz regularnej struktury) zeby wstawic zawartosc comparatora */
			virtual nodeEntry_off_t getSpaceNeeded()=0;
			
			//virtual char* getKeyAddr(NodeEntry* entry)=0;
			//virtual nodeEntry_off_t getKeySize(NodeEntry* entry)=0;
			//virtual NodeEntry* getNextItem(NodeEntry* entry)=0;
			/*
			template <class I>
			I* getItemKey(I* item);
			*/

			/** nie wymagaja zeby comparator byl wypelniony */
			
			/** ile maksymalnie moze zajac wpis. uzywane do profilaktycznego rozbicia wezla w jednoprzebiegowym wstawianiu */
			virtual nodeEntry_off_t maxEntrySize() = 0;
			
			/** usuwa wpis z wezla */
			virtual void removeEntry(Node* node, RootEntry* entry);
			
			/** majac item zwraca nastepny item w wezle */
			template <class I>
			I* getNextItem(I* item);

			/** czy wezel zmiesci jeszcze jeden maksymalnie duzy wpis. uzywane przy profilaktycznym rozbiciu wezla */			
			virtual bool canTakeExtraKey(Node* node);
			
			/** do wezla parent wstawia wskaznik na child. child staje sie dzieckiem nr childNumber (liczac od 0) */
			//virtual void putPtr(int childNumber, Node* parent, Node* child);
					
			/** zwraca obiekt I* o numerze entryNumber z node'a. liczone od 0 */
			template <class I>
			I* getItem(int itemNumber, Node* node, I* item);	
			
			/** zwraca klucz (w posteci stringa) zwiazany z danym Item'em */
			template <class I>
			string keyToString(I* item);
			
			/** zwraca pusta kopie comparatora odpowiedniego typu (bez wartosci i rootEntry) */
			virtual Comparator* emptyClone()=0;
			
			virtual bool isInitialized();
			
			virtual void setEntry(RootEntry* entry);
			virtual RootEntry* getEntry();
			
			virtual int setValue(string value);
			virtual int setValue(int value);
			virtual int setValue(double value);
			virtual int setValue(Store::DataValue* value);
			virtual int setValue(QExecutor::QueryResult* value);
			
			virtual string toString();
			
			template<class I>
			string nodeToString(Node* node, I* item);
			
			/** ustawia klucz comparatora na taki jaki jest za item */
			template<class I>
			void getValue(I* item);
			
/*			
			// zwraca ile miejsca potrzeba zeby skopiowac klucz z from i zeby zmiescila sie jeszcze struktua NodeEntry
			template<class I>
			nodeSize_t spaceForCopy(I* from);
	*/
			
			/** podczas rozbijania kopiuje klucz z from i robi strukture w to. nie ustawia struktury
			 *  zaklada ze miejsce juz jest zrobione
			 *  zwraca ile miejsca sie zwolni jesli usuniemy from wraz z kluczem */
		/*	 
			template<class I>
			nodeSize_t copyToParent(I* from, char* to);
		*/
			
			virtual string nodeToString(Node* node);
			
			/** sprawdza czy wezel moze miec zajetosc size() + additionalSize. do testow */
			virtual void checkSize(Node* node, int additionalSize = 0); 
			
			/** tworzy comparator wlasciwego typu na podstawie liczby zapisanej w bazie */
			static Comparator* deserialize(int);
			
			/** serializuje swoj typ do zapisu w bazie */
			virtual int serialize() const;
			
			/** zwraca typ wypisywany klientowi na konsoli */
			virtual string typeToString() const = 0; 
			
			friend class Tester;
	};
	
	/**
	 * NodeEntry za naglowkiem trzyma:
	 * 				nodeEntry_off_t nextEntry = ile trzeba dodac do this zeby dostac poczatek nastepnego wpisu
	 * 				char* key = klucz o dlugosci nextEntry - sizeof(NodeEntry) - sizeof(nodeEntry_off_t)
	 */
	class StringComparator : public Comparator {
		friend class Tester;
		private:
			char* value;
			nodeEntry_off_t spaceNeeded;
			
			/** jaka jest dlugos przechowywanego napisu */
			nodeEntry_off_t strSize;
			
			template<typename T>
			void setValueConverted(T* value);
			//void init();
			
		protected:
			virtual comparatorType getType() const;
			
		public:
			/** musi byc zakonczony 0 */
			//StringComparator(char* value, RootEntry* entry);
			StringComparator();
			virtual ~StringComparator();
			virtual int compare (char* item, unsigned int itemSize, lid_t lidDiff);
			
			virtual char* getKeyAddr(char* item, unsigned int itemSize);
			virtual nodeEntry_off_t getKeySize(char* item, unsigned int itemSize);
			virtual char* getNextItem(char* itemAddress, unsigned int itemSize);
			
			virtual nodeEntry_off_t maxEntrySize();
			virtual char* getItem(int itemNumber, char* itemAddress, unsigned int itemSize);
			virtual nodeEntry_off_t getSpaceNeeded();
			virtual void putKey(char* keyPlace, nodeEntry_off_t itemSize);
			virtual Comparator* emptyClone();
			virtual int setValue(string value);
			virtual int setValue(int value);
			virtual int setValue(double value);
			
			virtual string keyToString(char* item, unsigned int itemSize);
			
			virtual void getValue(char* item, unsigned int itemSize);
			virtual string typeToString() const;
	};
	
	/**
	 * NodeEntry za naglowkiem trzyma:
	 * 				T key = klucz o stalym rozmiarze
	 */
	template <class T>
	class FixedLengthComparator : public Comparator {
		protected:
			T value;
			void init();
			
		public:
			FixedLengthComparator();
			FixedLengthComparator(T value, RootEntry* entry);
			virtual int compare (char* item, unsigned int itemSize, lid_t lidDiff);
			virtual ~FixedLengthComparator(){}
			
			virtual char* getKeyAddr(char* item, unsigned int itemSize);
			virtual nodeEntry_off_t getKeySize(char* item, unsigned int itemSize);
			
			T getKey(char* item, unsigned int itemSize);
			
			virtual char* getNextItem(char* itemAddress, unsigned int itemSize);
			string keyToString (char* item, unsigned int itemSize);
			virtual nodeEntry_off_t maxEntrySize();
			virtual char* getItem(int itemNumber, char* itemAddress, unsigned int itemSize);
			virtual nodeEntry_off_t getSpaceNeeded();
			virtual void putKey(char* where, nodeEntry_off_t itemSize);
			
			virtual void getValue(char* item, unsigned int itemSize);
	};
	
	class IntComparator : public FixedLengthComparator<int> {
		protected:
			virtual comparatorType getType() const;
			
		public:
		//	IntComparator();
		//	IntComparator(int value, RootEntry* entry);
			
			virtual int setValue(int value);
			//virtual int setValue(double value);
			
			virtual IntComparator* emptyClone();
			
			virtual string toString();
			virtual string typeToString() const;
	};
	
	class DoubleComparator : public FixedLengthComparator<double> {
		protected:
			virtual comparatorType getType() const;
		public:
			//DoubleComparator();
			//DoubleComparator(double value, RootEntry* entry);
			virtual int setValue(int value);
			virtual int setValue(double value);
			virtual DoubleComparator* emptyClone();
			virtual string typeToString() const;
	};

	class MinComparator : public Comparator {
		protected:
			virtual comparatorType getType() const;
		public:
			//** zwraca NodeItem o numerze 0. pozostale wartosci rzucaja blad /
			char* getItem(int itemNumber, char* item0Address, unsigned int itemSize);
			
			int compare(char*, unsigned int, lid_t lidDiff);
			nodeEntry_off_t getSpaceNeeded();
			char* getNextItem(char*, unsigned int);
			nodeEntry_off_t maxEntrySize();
			void putKey(char*, nodeEntry_off_t);
			Comparator* emptyClone();
			void getValue(char* item, unsigned int itemSize);
			
			
			virtual string keyToString(char* item, unsigned int itemSize);
			virtual string typeToString() const;
	};

}

#endif /*COMPARATOR_H_*/
