#ifndef __CACHEDNODE_H__
#define __CACHEDNODE_H__

#include "BTree.h"
#include "TransactionManager/Mutex.h"
#include "Errors/Errors.h"
#include "Node.h"

namespace Indexes {
using namespace SemaphoreLib;
	
	class CachedNode {
		private:
			
			/** zaladowany do pamieci wezel */
			Node* node;
	
			/** id wezla do wczytania */
			nodeAddress_t nodeID;
	
			/** synchronizuje wczytanie i dostep do wezla */
			Mutex* mutex;
			
			/** organizuje dostep do danych wezla odczyt-zapis */
			RWUJSemaphore* sem;
			
			/** czy wezel jest juz wczytany z hdd */
			bool loaded;
			
			/** podstawowa inicjalizacja obiektu */
			void init(nodeAddress_t nodeID);
	
		public:
			
			/** czy wezel zostal zmieniony i powinien zostac zapisany na dysk */
			bool dirty;
			
			/** ile transakcji uzywa tego node'a
			 *  zmienna jest synchronizowana tym samym semaforem co mapa cache'u bo tylko wtedy jest uzywana */
			long users;
			
			/** glebokosc wezla w drzewie. 1 - lisc; h - korzen */
			int depth;
			
			/** zwraca node'a. jesli jest w pamieci to od razu, wpp wczytuje z dysku */
			int getNode(Node* &result);
			
			int getNode(Node* &result, int priority, NodeAccessMode mode);
			
			/** usiluje nieblokujaco pobrac wezel do zapisu */
			int tryGetNode(Node* &result, int priority);
			
			/** zwalnia dostep do wezla,
			 *  uzywane przez metode freeNode i przy usuwaniu wezla */
			int unlock();
			
			/** zwraca blok danych. moze byc uzywana tylko gdy wiadomo ze wezel jest w pamieci */
			Node* getNode();
			
			/** zwraca priorytet jaki powinien miec ojciec tego wezla */
			int parentPriority();
			
			/** zwraca priorytet jaki powinno miec rodzenstwo wezla */
			int siblingPriority();
			
			int childPriority();
			
			/** zwraca id zwiazanego z nim wezla. nieblokujace */
			nodeAddress_t getNodeID();
			
			/** oznacza wezel jako brudny */
			void setDirty();
			
			/** tworzy nowy wezel scache'owany z juz wczytanym wezlem */
			CachedNode(Node* node);
			
			/** tworzy nowy wezel scache'owany wskazujacy na wezel o danym id. wezel trzeba bedzie wczytac */
			CachedNode(nodeAddress_t nodeID);
			
			~CachedNode();
	};
	
}

#endif /*CACHEDNODE_H_*/
