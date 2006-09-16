#include <list>
#include <vector>
#include "TreeNode.h"
#include "DataRead.h"
#include "Stack.h"
#include "ClassNames.h"
#include "QueryExecutor/QueryResult.h"
#include "QueryExecutor/QueryExecutor.h"
#include "Store/Store.h"
#include "Store/DBStoreManager.h"
#include "Store/DBObjectPointer.h"
#include "TransactionManager/Transaction.h"
#include "Deb.h"
using namespace std;
//using namespace QExecutor;
//using namespace Store;

namespace QParser {
		BinderWrap *DataScheme::statNested (int objId, TreeNode *treeNode) {
			DataObjectDef *candidate = this->getObjById(objId);
			DataObjectDef *pom = NULL;
//			list<StatBinder *> bindersCol;
			BinderList *bindersCol = NULL;
			if (candidate == NULL)	
				return bindersCol; //<pusta kolekcja chyba ... >;
			/*dalej zakladamy ze znalezlismy taki objekt w metabazie... */
			Deb::ug("i found object nr %d.\n", candidate->getMyId());
			if (candidate->getKind() == "atomic") return bindersCol;	//<pusta kolekcja>;
			if (candidate->getKind() == "link") {
				Deb::ug("object is a link !\n");
				pom = candidate->getTarget(); /*now pom points to the target object. */
				SigRef *sigRef = new SigRef (pom->getMyId());
				sigRef->setDependsOn(NULL);
				sigRef->setCard(candidate->getCard());
				StatBinder * sb = new StatBinder (pom->getName(), sigRef);				
				sb->setDependsOn(treeNode);
				sb->setCard(candidate->getCard());
				//bindersCol.push_back(sb);
				bindersCol = new BinderList(sb);
				return bindersCol;	//<kolekcja 1-eltowa zawierajaca sb>;				
			};
			/*dalej zakladamy ze kind == "complex" -- objekt zlozony. */
			Deb::ug("the object is a complex one ! \n");
			pom = candidate->getSubObjects();
				// <kolekcja statBinderow> *binders = new <pusta kolekcja>;	
			Deb::ug("one sub - nr: %d.\n", pom->getMyId());
			while (pom != NULL) {
				SigRef * sigRef = new SigRef (pom->getMyId());
				sigRef->setDependsOn(NULL);
				sigRef->setCard(pom->getCard());
				StatBinder * sb = new StatBinder (pom->getName(), sigRef);
				sb->setDependsOn(treeNode);				// old to rem
				sb->setCard(pom->getCard());
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
				SigRef *sigRef = new SigRef (obts->getMyId());
				sigRef->setCard(obts->getCard());						// ?? ??- ktore trzeba?
				sigRef->setDependsOn(NULL);		// death
				StatBinder * sb = new StatBinder (obts->getName(), sigRef);
				sb->setCard(obts->getCard());							// ?? ??czy obydwa?
				sb->setDependsOn(NULL);
				if (bw == NULL) bw = new BinderList (sb);
				else bw = (BinderList *) bw->addPureBinder(sb);			
			}
			return bw;
		};		

DataObjectDef * DataScheme::createDataObjectDef(ObjectPointer *mainOp, Transaction *tr){
    Deb::ug("start createDataObjectDef \n");
    //jsi cerr << "diag2.2 " << mainOp->getLogicalID()->toInteger() << endl;
    this->getObjById(mainOp->getLogicalID()->toInteger());
    if (this->getObjById(mainOp->getLogicalID()->toInteger()) != NULL){
	Deb::ug("dany obiekt juz jest wczytany \n");
	return this->getObjById(mainOp->getLogicalID()->toInteger());
    };
        
    DataObjectDef * datObDef = new DataObjectDef();	// ten obiekt wypelniam 
    datObDef->setMyId(mainOp->getLogicalID()->toInteger());
    DataValue *dv = mainOp->getValue();	//to jest obiekt zlozony, tu jest wektor wskaznikow do logicalId
    vector<LogicalID*>* v = dv->getVector();  
//    cerr << "rodzaj obiektu " << mainOp->getName() << endl;
    if (mainOp->getName() == "__MDN__"){
        this->addBaseObj(datObDef);
    } else {
        this->addObj(datObDef);
    }
    // teraz ustawiam name, card, kind....    
    for (unsigned   int j = 0; j < v->size(); j++){
        // czy acces mode sie robi w ten sposob?
	ObjectPointer *op = NULL;
	tr->getObjectPointer( (*v)[j], Store::Read, op);
//	cerr << " wczytal " << op->getName() << "---------------------" << endl;
        // moze tak nie mozna porownywac getName zwraca stringa **
        if (op->getName() == "name"){
//	    cout << "wczytal name " << op->getValue()->getString() << endl;
	    datObDef->setName(op->getValue()->getString());    
	} else if (op->getName() == "kind") {
//	    cout << "wczytal kind " << op->getValue()->getString() << endl;
	    datObDef->setKind(op->getValue()->getString());    
	} else if (op->getName() == "card"){
//	    cout << "wczytal card " << op->getValue()->getString() << endl;
	    datObDef->setCard(op->getValue()->getString());    
	} else if (op->getName() == "type") {
//	    cout << "wczytal type " << op->getValue()->getString() << endl;
	    datObDef->setType(op->getValue()->getString());    
	} else if (op->getName() == "owner") {
//	    cout << "WAZNE wczytal owner " << op->getValue()->getPointer()->toInteger() << endl;
	    datObDef->setOwnerId(op->getValue()->getPointer()->toInteger());
    //	    tr->getObjectPointer( (*v)[j], Store::Read, op);
	    ObjectPointer * pom;
	    tr->getObjectPointer(op->getValue()->getPointer(), Store::Read, pom);
//	    cout << "spodziewam sie, ze w nas linii napisze ze dany obiekt jest juz wczytany " << endl;
	    datObDef->setOwner(this->createDataObjectDef(pom, tr));
	} else if (op->getName() == "target") {
//	    cout << "WAZNE wczytal target " << op->getValue()->getPointer()->toInteger();
	    datObDef->setTargetId(op->getValue()->getPointer()->toInteger());
	    ObjectPointer * pom;
	    tr->getObjectPointer(op->getValue()->getPointer(), Store::Read, pom);
	    datObDef->setTarget(this->createDataObjectDef(pom, tr));
	} else if (op->getName() == "subobject") {				// czy to jest link??? chyba tak 
//	    cerr << "WAZNE wczytal subobject " << endl;				// tak samo jak owner i target
//	    cerr << "to chyba nie to id/ a jednak moze to " << op->getLogicalID()->toInteger();
//	    cerr << "to chyba dobre id " << op->getValue()->getPointer()->toInteger() << endl;
	    ObjectPointer * pom;
	    tr->getObjectPointer(op->getLogicalID(), Store::Read, pom);
//	    cerr << "wola rek " << endl;
	    DataObjectDef * subobj = this->createDataObjectDef(pom, tr);	    
	    if (subobj->getOwner() == datObDef){
//		cerr << "OK subobj->getOwner() == datObDef" << endl;
	    } else {
		cout << "jsi ERROR subobj->getOwner() != datObDef" << endl;
	    }
	    datObDef->addSubObject(subobj);
//	    cerr << "end subobject " << endl;	    
	}
    }    
//    cerr << "end createDataObjectDef " << endl;    
    return datObDef;
}

int DataScheme::readData(){
    Deb::ug("-------------------readData START--------------------------------------");
    vector<ObjectPointer *> * roots;
if (true) {

    Transaction * tr;     
    if (TransactionManager::getHandle()->createTransaction(tr))
	Deb::ug("nie udalo sie zrobic tranzakcji");
        
	
    tr->getRoots("__MDN__", roots);	
    if (Deb::ugOn()) cout << "odebral " << roots->size() << " obiektow"<< endl;
    for (unsigned int i = 0; i < roots->size(); i++){
	ObjectPointer * mdnOp = (*roots)[i];
//	cout << "tworzy DataObjDef(__MDN__) ---------------------------------\n";
	createDataObjectDef(mdnOp, tr);
    }
}    
    

Deb::ug("-------------------readData END--------------------------------------");
	return 0;
};
/***********************************************************************************************************/

/***********************************************************************************************************/
	DataScheme* DataScheme::datScheme;
	
	DataScheme* DataScheme::dScheme() {
		if (datScheme == NULL) {
			datScheme = new DataScheme();
			datScheme->readData();
		}		
		DataObjectDef *pom = datScheme->getBaseObjects();
		Deb::ug("DataScheme has these base objects: ");
	    if (Deb::ugOn()) {
		while (pom != NULL) {		
			fprintf(stderr, "[%d]", pom->getMyId());	
			pom = pom->getNextBase();
		} 
		cout << endl;
	    }
	    return datScheme;
	};

}
/************************** KONIEC tam dalej jest syf *********************************************************/













/*************************************************************************************************************/

	// wszystko dalej jest do wywalenia, moze poza tym recznym na wszelki wypadek


/*************************************************************************************************************/
/*
void reczny(){
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

}
*/
