#ifndef CLEANER_H_
#define CLEANER_H_

#include <Indexes/RootEntry.h>
#include <Indexes/Comparator.h>

using namespace std;
namespace Indexes {

	class Cleaner {
		public:
			Cleaner();
			virtual ~Cleaner();
			
			/** mowi czy podczas wstawiania nowego wpisu dotyczacego tego samego lid trzeba usunac stary i nic nie dodawac.
			 *  true - nic nie wstawiamy i usuwamy stary
			 *  false - nie usuwamy starego i dodajemy nowy */
			bool putEqualLid(RootEntry* inTree, RootEntry* toPut);
			
			/** sprawdza czy wpis powinien byc zmieniony i w razie potrzeby
			 * oznacza wezel jako wymagajacy modyfikacji  
			 */
			//void checkChangeNeed(Node* node, RootEntry* entry);
			
			static Cleaner* getHandle();
			
			/** doprowadza do tego ze lisc ma cleanNeeded == false 
			 *  wyrzuca zbedne wpisy. node musi byc zablokowany do zapisu */
			void clean(CachedNode* node, Comparator* comp, IndexHandler* ih);
			
		private:
			static auto_ptr<Cleaner> instance;
	};

}

#endif /*CLEANER_H_*/
