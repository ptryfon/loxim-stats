

#include <list>
#include <vector>
#include "TreeNode.h"
#include "DataRead.h"
#include "Stack.h"
//#include "ClassNames.h"
#include "QueryExecutor/QueryResult.h"
#include "QueryExecutor/QueryExecutor.h"
#include "Store/Store.h"
#include "Store/DBStoreManager.h"
#include "Store/DBObjectPointer.h"
#include "TransactionManager/Transaction.h"

using namespace std;
using namespace QExecutor;
using namespace Store;

namespace QParser {


		BinderWrap *DataScheme::statNested (int objId) {
			DataObjectDef *candidate = this->getObjById(objId);
			DataObjectDef *pom;
//			list<StatBinder *> bindersCol;
			BinderList *bindersCol;
			if (candidate == NULL)	
				return bindersCol; //<pusta kolekcja chyba ... >;
			/*dalej zakladamy ze znalezlismy taki objekt w metabazie... */
			
			
			if (candidate->getKind() == "atomic") return bindersCol;	//<pusta kolekcja>;
			if (candidate->getKind() == "link") {
				pom = candidate->getTarget(); /*now pom points to the target object. */
				StatBinder * sb = new StatBinder (pom->getName(), new SigRef (pom->getMyId()));				
				//bindersCol.push_back(sb);
				bindersCol = new BinderList(sb);
				return bindersCol;	//<kolekcja 1-eltowa zawierajaca sb>;				
			};
			/*dalej zakladamy ze kind == "complex" -- objekt zlozony. */
			pom = candidate->getSubObjects();
				// <kolekcja statBinderow> *binders = new <pusta kolekcja>;	
			while (pom != NULL) {
				StatBinder * sb = new StatBinder (pom->getName(), new SigRef (pom->getMyId()));
				//binders->addBinder(sb);
				if (bindersCol == NULL) bindersCol = new BinderList(sb);
				else {
					bindersCol = (BinderList *) (bindersCol->addOne(new BinderList(sb)));
				}
//				bindersCol.push_back(sb);
				pom = pom->getNextSub();
				/**NIEEE !!! pom = pom->getNextSub() ! >:-0 */
			}
			return bindersCol;		
		};


		BinderWrap* DataScheme::bindBaseObjects() {
			BinderList *bw = NULL;
			for (DataObjectDef *obts = this->getBaseObjects(); obts != NULL; obts = obts->getNextBase()) {
				StatBinder * sb = new StatBinder (obts->getName(), new SigRef (obts->getMyId()));
				if (bw == NULL) bw = new BinderList (sb);
				else bw = (BinderList *) bw->addPureBinder(sb);			
			}
			return bw;
		};		



int DataScheme::readData(){

    // ????????????         co zrobic zebym mogl tu wywolac executora i stora?        ??????????
    cout << "-------------------readData START--------------------------------------" << endl;
    // QueryExecutor * ecec = new QueryExecutor();		// ??????????????????????????
    vector<ObjectPointer *> * roots;
/*		nie wiem dlaczego ale to nie dziala
    StoreManager * sm = new DBStoreManager();
    sm->getRoots(0, "dupa", roots);
    cout << "odebral " << roots->size() << " obiektow"<< endl;
    sm->getRoots(0, roots);
    cout << "odebral " << roots->size() << " obiektow"<< endl;
*/
    
    Transaction * tr;     
    if (TransactionManager::getHandle()->createTransaction(tr))
	cout << "nie udalo sie zrobic tranzakcji" << endl;
    
    
    tr->getRoots("dupa", roots);	
    cout << "odebral " << roots->size() << " obiektow"<< endl;
    
    /* teraz to obrabiam w 2 podejciach, najpierw tworze obiekty, wstawiam do
	this->baseObjects i this->refTable
	oraz ustawiam pola rozne od owner i target
	
	potem przechodze jeszcze raz i moge ustawic pola owner i target
    */
    
    // a moze  w owner i target zamiast wskaznika dac id - int
    // moze listy zamienic listami stl'a i refTable hashMapem'
    
    //tr->getRoots(roots);	// nie wiem dlaczego ale tto tez nie dziala
    //cout << "odebral " << roots->size() << " obiektow"<< endl;
    
    for (int i = 0; i < roots->size(); i++){
	
    }
    
    // -----------------------------------------------------------------------
    // recznie tworze przykladowy schemat
    
    DataObjectDef * obj1 = new DataObjectDef();
    obj1->setMyId(1);
    obj1->setName("EMP");
    obj1->setCard("0..*");
    obj1->setKind("complex");
    this->addBaseObj(obj1);
    
    DataObjectDef * obj10 = new DataObjectDef();
    obj10->setMyId(10);
    obj10->setName("NAME");
    obj10->setCard("1..1");
    obj10->setKind("atomic");
    obj10->setType("string");
    this->addObj(obj10);    
    
    DataObjectDef * obj11 = new DataObjectDef();
    obj11->setMyId(11);
    obj11->setName("SAL");
    obj11->setCard("1..1");
    obj11->setKind("atomic");
    obj11->setType("int");
    this->addObj(obj11);
    
    DataObjectDef * obj12 = new DataObjectDef();
    obj12->setMyId(12);
    obj12->setName("WORKS_IN");
    obj12->setCard("1..1");
    obj12->setKind("link");
    this->addObj(obj12);
    
    DataObjectDef * obj30 = new DataObjectDef();
    obj30->setMyId(30);
    obj30->setName("DEPT");
    obj30->setCard("0..*");
    obj30->setKind("complex");
    this->addBaseObj(obj30);
    
    DataObjectDef * obj37 = new DataObjectDef();
    obj37->setMyId(37);
    obj37->setName("DNAME");
    obj37->setCard("1..1");
    obj37->setKind("atomic");
    obj37->setType("string");
    this->addObj(obj37);
    
    DataObjectDef * obj38 = new DataObjectDef();
    obj38->setMyId(38);
    obj38->setName("LOC");
    obj38->setCard("1..*");
    obj38->setKind("atomic");
    obj38->setType("string");
    this->addObj(obj38);
    
    
    obj10->setOwner(getObjById(1));    
    obj11->setOwner(getObjById(1));
    obj12->setOwner(getObjById(1));
    obj12->setTarget(getObjById(30));
    obj1->addSubObject(getObjById(10));
    obj1->addSubObject(getObjById(11));
    obj1->addSubObject(getObjById(12));

    obj37->setOwner(getObjById(30));
    obj38->setOwner(getObjById(30));
    obj30->addSubObject(getObjById(37));
    obj30->addSubObject(getObjById(38));

cout << "-------------------readData END--------------------------------------" << endl;
	return 0;
};


}
	DataScheme* DataScheme::datScheme = 0;
	
	DataScheme* DataScheme::dScheme() {
		if (datScheme == NULL) {
			datScheme = new DataScheme();
			datScheme->readData();
		}
		return datScheme;
	};






