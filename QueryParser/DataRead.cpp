

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

using namespace std;
//using namespace QExecutor;
//using namespace Store;

namespace QParser {


		BinderWrap *DataScheme::statNested (int objId) {
			DataObjectDef *candidate = this->getObjById(objId);
			DataObjectDef *pom = NULL;
//			list<StatBinder *> bindersCol;
			BinderList *bindersCol = NULL;
			if (candidate == NULL)	
				return bindersCol; //<pusta kolekcja chyba ... >;
			/*dalej zakladamy ze znalezlismy taki objekt w metabazie... */
			
			fprintf(stderr, "i found object nr %d.\n", candidate->getMyId());
			if (candidate->getKind() == "atomic") return bindersCol;	//<pusta kolekcja>;
			if (candidate->getKind() == "link") {
				fprintf(stderr, "object is a link !\n");
				pom = candidate->getTarget(); /*now pom points to the target object. */
				StatBinder * sb = new StatBinder (pom->getName(), new SigRef (pom->getMyId()));				
				//bindersCol.push_back(sb);
				bindersCol = new BinderList(sb);
				return bindersCol;	//<kolekcja 1-eltowa zawierajaca sb>;				
			};
			/*dalej zakladamy ze kind == "complex" -- objekt zlozony. */
			fprintf(stderr, "the object is a complex one ! \n");
			pom = candidate->getSubObjects();
				// <kolekcja statBinderow> *binders = new <pusta kolekcja>;	
			fprintf (stderr, "one sub - nr: %d.\n", pom->getMyId());
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
        
	
    tr->getRoots("MetaDataNode", roots);	
    cout << "odebral " << roots->size() << " obiektow"<< endl;
    for (int i = 0; i < roots->size(); i++){
	ObjectPointer * mainOp = (*roots)[i];
	DataObjectDef * datObDef = new DataObjectDef();	// ten obiekt wypelniam 
	cout << "id " << mainOp->getLogicalID()->toInteger() << endl;
	cout << "name (wydaje mi sie zer powinno byc MetaDataNode) jesli co iinego to ???? " << mainOp->getName() << endl;
	cout << "to string : " << mainOp->toString() << endl;
	// wydaje mi sie ze tu sa same obiekty zlozone
	DataValue *dv = mainOp->getValue();	// zakladam ze tu jest wektor wskaznikow do logicalId
	vector<LogicalID*>* v = dv->getVector();
	/*	zakladam ze w tym wektorze sa wskazniki do logicalID obiektow:
		o nazwie name kind card type owner target subobject 
	*/
	datObDef->setMyId(mainOp->getLogicalID()->toInteger());
	
	ObjectPointer *op = NULL;
	// ----------------------------------------------------- name
	for (int j = 0; j < v->size(); j++){
	    // czy acces mode sie robi w ten sposob?
	    tr->getObjectPointer( (*v)[j], Store::Read, op);
	    // moze tak nie mozna porownywac getName zwraca stringa **
	    if (op->getName() == "name")
		j = v->size();	// break
	}
	if (op == NULL)
	    cout << "ERROR w readData nie moze ustalic nazwy obiektu " << endl;
	
	cout << "wyluskal taka nazwa obiektu " << op->getValue()->getString();    
	datObDef->setName(op->getValue()->getString());    
	
	// ----------------------------------------		kind
	
	op = NULL;
	for (int j = 0; j < v->size(); j++){
	    // czy acces mode sie robi w ten sposob?
	    tr->getObjectPointer( (*v)[j], Store::Read, op);
	    // moze tak nie mozna porownywac getName zwraca stringa **
	    if (op->getName() == "kind")
		j = v->size();	// break
	}
	if (op == NULL)
	    cout << "ERROR w readData nie moze ustalic kind obiektu " << endl;
	
	cout << "wyluskal taka kind obiektu " << op->getValue()->getString();    
	datObDef->setKind(op->getValue()->getString());    
	
	// ----------------------------------------		card
	
	op = NULL;
	for (int j = 0; j < v->size(); j++){
	    // czy acces mode sie robi w ten sposob?
	    tr->getObjectPointer( (*v)[j], Store::Read, op);
	    // moze tak nie mozna porownywac getName zwraca stringa **
	    if (op->getName() == "card")
		j = v->size();	// break
	}
	if (op == NULL)
	    cout << "ERROR w readData nie moze ustalic card obiektu " << endl;
	
	cout << "wyluskal taka card obiektu " << op->getValue()->getString();    
	datObDef->setCard(op->getValue()->getString());    
	
	// ----------------------------------------		card
	cout << "sprawdza czy obiekt jest atomowy - byc moze w zly sposob " << endl;
	// FIXME nie wiem czy tak mozna getKind zwraca stringa    
	if (datObDef->getKind() == "atomic"){
	    op = NULL;
	    for (int j = 0; j < v->size(); j++){
		// czy acces mode sie robi w ten sposob?
	        tr->getObjectPointer( (*v)[j], Store::Read, op);
		// moze tak nie mozna porownywac getName zwraca stringa **
	        if (op->getName() == "type")
			j = v->size();	// break
		else op = NULL;	
		}
	    if (op == NULL)
		cout << "ERROR w readData nie moze ustalic card obiektu " << endl;
	
	    cout << "wyluskal taki type obiektu " << op->getValue()->getString();    
	    datObDef->setType(op->getValue()->getString());    
	}
	
	// ----------------------------------------		owner - nie wypelniam tylko sprawdzam czy ustawiony/jest
						    // jezeli jest tzn. ze obiekt jest bazowy 
	op = NULL;
	for (int j = 0; j < v->size(); j++){
	    // czy acces mode sie robi w ten sposob?
	    tr->getObjectPointer( (*v)[j], Store::Read, op);
	    // moze tak nie mozna porownywac getName zwraca stringa **
	    if (op->getName() == "owner")
		j = v->size()+10;	// break
	    else op = NULL;	
	}
	if (op == NULL){
	    cout << "readData nie moze ustalic owner - czyli obiekt nei jest bazowy" << endl;
	    this->addBaseObj(datObDef);
	} else {
	    this->addObj(datObDef);
	}
	
	// ---------------------- pozostaje jeszcze ustawic owner, target, subobject
	
    }	// end for dla kazdego obiektu MetaDataNode
    
    
    
    
    // teraz jeszcze raz tak samo wczytuje i ustawiam obiektom pola owner, target, subobject
    // kazdy podobiekt 
    
    
    
        
    for (int i = 0; i < roots->size(); i++){
	ObjectPointer * mainOp = (*roots)[i];
	DataObjectDef * datObDef = getObjById(mainOp->getLogicalID()->toInteger());// ten obiekt wypelniam 

	DataValue *dv = mainOp->getValue();	// zakladam ze tu jest wektor wskaznikow do logicalId
	vector<LogicalID*>* v = dv->getVector();
	/*	zakladam ze w tym wektorze sa wskazniki do logicalID obiektow:
		o nazwie owner target subobject 
	*/
	ObjectPointer *op = NULL;
		
	// ----------------------------------------		owner - nie wypelniam tylko sprawdzam czy ustawiony/jest
						    // jezeli jest tzn. ze obiekt jest bazowy 
	op = NULL;
	for (int j = 0; j < v->size(); j++){
	    // czy acces mode sie robi w ten sposob?
	    tr->getObjectPointer( (*v)[j], Store::Read, op);
	    // moze tak nie mozna porownywac getName zwraca stringa **
	    if (op->getName() == "owner")
		j = v->size();	// break
	    else op = NULL;
	}
	if (op == NULL){
	    cout << "readData nie moze ustalic owner - czyli obiekt nei jest bazowy" << endl;
	    datObDef->setOwner(NULL);
	} else {
	    datObDef->setOwner(getObjById(op->getValue()->getPointer()->toInteger()));
	}

	// -------------------------------------------		target ustawiamy jesli to obiekt typu link

	if (datObDef->getKind()=="link"){
	    
	    op = NULL;
	    for (int j = 0; j < v->size(); j++){
		// czy acces mode sie robi w ten sposob?
	        tr->getObjectPointer( (*v)[j], Store::Read, op);
		// moze tak nie mozna porownywac getName zwraca stringa **
	        if (op->getName() == "target");
		    j = v->size();	// break
	    }
	    
	    datObDef->setTarget(getObjById(op->getValue()->getPointer()->toInteger()));
	
	}
	
	// -------------------------------------------		suboject y ustawiamy jesli to obiekt typu complex
	
	if (datObDef->getKind() == "complex"){
	    
	    op = NULL;
	    for (int j = 0; j < v->size(); j++){
		// czy acces mode sie robi w ten sposob?
	        tr->getObjectPointer( (*v)[j], Store::Read, op);
		// moze tak nie mozna porownywac getName zwraca stringa **
	        if (op->getName() == "subobject"){
		    DataObjectDef * subobj = getObjById(op->getLogicalID()->toInteger());  
		    subobj->getOwner()->addSubObject(subobj);
		}
	    }
	    
//	    dataObDef->setTarget(getObjById(op->getValue()->getPointer()->toInteger()));
	
	}
	
    }	// end for dla kazdego obiektu MetaDataNode
    
    
    
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



	DataScheme* DataScheme::datScheme;
	
	DataScheme* DataScheme::dScheme() {
		if (datScheme == NULL) {
			datScheme = new DataScheme();
			datScheme->readData();
		}		
		DataObjectDef *pom = datScheme->getBaseObjects();
		//cout << "i read my Data:" << pom->getMyId() <<  endl;
		fprintf(stderr, "DataScheme has these base objects: ");
		while (pom != NULL) {			
			fprintf(stderr, "[%d]", pom->getMyId());	
			pom = pom->getNextBase();
		} cout << endl;
		return datScheme;
	};

}





