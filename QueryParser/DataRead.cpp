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
#include "Errors/ErrorConsole.h"
#include "Errors/Errors.h"
#include "TransactionManager/Transaction.h"
#include "TypeCheck/TypeChecker.h"
#include "QueryParser/Stack.h"
#include "Deb.h"
using namespace std;
//using namespace QExecutor;
//using namespace Store;

namespace QParser {
	
	BinderWrap *DataScheme::statNested (int objId, TreeNode *treeNode) {
		Deb::ug("dataScheme::statNested start\n");
		DataObjectDef *candidate = this->getObjById(objId);
		return statNested(candidate, treeNode, candidate->getName(), candidate->getCard());
	}
		
	
	BinderWrap *DataScheme::statNested (DataObjectDef *candidate, TreeNode *treeNode, string obName, string obCard) {
		DataObjectDef *pom = NULL;
		BinderList *bindersCol = NULL;
			
		if (candidate == NULL){	
			Deb::ug("NULL !!!!\n");
			return bindersCol; //<empty collection>;
		}
		/* further we assume we have found such an object in database */
		Deb::ug("i found object nr %d.\n", candidate->getMyId());
		if (candidate->getKind() == TC_MDN_ATOMIC) return bindersCol;	//<empty collection>;
		if (candidate->getKind() == TC_MDN_LINK) {
			Deb::ug("object is a link !\n");
			pom = candidate->getTarget(); /*now pom points to the target object. */
			SigRef *sigRef = new SigRef (pom->getMyId());
			sigRef->setDependsOn(NULL);
			sigRef->setCard(obCard);
			string binderName = pom->getName();
			if (pom->getIsTypedef()) { //make it an artificial name, to be able to detect when binding.and not for typename not be be used, eg. emp.works_in.deptType.loc isn't correct.deptType will not be bound.
				binderName = TC_REF_TYPE_SUFF + binderName;
			}
			StatBinder * sb = new StatBinder (binderName, sigRef);	
			sb->setDependsOn(treeNode);
			sb->setCard(obCard);
			bindersCol = new BinderList(sb);
			return bindersCol;	//<kolekcja 1-eltowa zawierajaca sb>;	
		} else if (candidate->getKind() == TC_MDN_DEFTYPE) {
			Deb::ug("object defined by its type !!!\n");	
			pom = candidate->getTypeObj();	
			BinderList *retList = (BinderList *) statNested(pom, treeNode, obName, obCard);	
			return retList;				
		};
		/* further we assume that kind == "complex" -- objekt zlozony. */
		Deb::ug("the object is a complex one ! \n");
		pom = candidate->getSubObjects();
		Deb::ug("one sub - nr: %d.\n", pom->getMyId());
		while (pom != NULL) {
			SigRef * sigRef = new SigRef (pom->getMyId());
			sigRef->setDependsOn(NULL);
			sigRef->setCard(pom->getCard());
			sigRef->setTypeName(pom->getTypeName());
			StatBinder * sb = new StatBinder (pom->getName(), sigRef);
			sb->setDependsOn(treeNode);				// old to rem
			sb->setCard(pom->getCard());
			sb->setTypeName(pom->getTypeName());
				//binders->addBinder(sb);
			if (bindersCol == NULL) bindersCol = new BinderList(sb);
			else {
				bindersCol = (BinderList *) (bindersCol->addOne(new BinderList(sb)));
			}
			pom = pom->getNextSub();
			/**! pom = pom->getNextSub() ! >:-0 */
		}
		if (bindersCol == NULL) Deb::ug("dataScheme::statNested returns with NULL!!!\n");
		return bindersCol;		
	};

	Signature *DataScheme::signatureOfRef(int objId) {
		Deb::ug("DataScheme: getting signature of ref: %d.", objId);
		DataObjectDef *candidate = this->getObjById(objId);
		if (candidate == NULL){	
			Deb::ug("object with this id is NULL !!!!\n");
			return NULL; 
		}
		Signature *resultSig = candidate->createSignature();
		if (Deb::ugOn()) {
			cout << "Created signature based on dataobject def. Sig: " << endl;
			resultSig->putToString();
		}
		return resultSig;
	};			

	Signature *DataScheme::namedSignatureOfRef(int objId) {
		Deb::ug("DataScheme: getting NAMED signature of ref: %d.", objId);
		DataObjectDef *candidate = this->getObjById(objId);
		if (candidate == NULL){	
			Deb::ug("object with this id is NULL !!!!\n");
			return NULL; 
		}
		Signature *resultSig = candidate->createNamedSignature();
		if (Deb::ugOn()) {
			cout << "Created NAMED signature based on dataobject def. Sig: " << endl;
			resultSig->putToString();
			cout << "\n";
		}
		return resultSig;
	}
	
	void DataScheme::addBindEntity(DataObjectDef *obt, BinderWrap *&bw) {
		SigRef *sigRef = new SigRef (obt->getMyId());
		sigRef->setCard(obt->getCard());						// ?? ??- ktore trzeba?
		if (obt->getKind() == TC_MDN_DEFTYPE && obt->getTypeObj() != NULL && obt->getTypeObj()->getIsDistinct()) {
			sigRef->setTypeName(obt->getTypeName());
		}
		sigRef->setDependsOn(NULL);		// death
		StatBinder * sb = new StatBinder (obt->getName(), sigRef);
		sb->setCard(obt->getCard());							// ?? ??czy obydwa?
		sb->setTypeName(obt->getTypeName());
		sb->setDependsOn(NULL);
		if (bw == NULL) bw = new BinderList (sb);
		else bw = (BinderList *) bw->addPureBinder(sb);			
	}
	
	BinderWrap* DataScheme::bindBaseObjects() {
		BinderWrap *bw = NULL;
		for (DataObjectDef *obt = this->getBaseObjects(); obt != NULL; obt = obt->getNextBase()) {
			addBindEntity(obt, bw);
		}
		return bw;
	};
	
	BinderWrap* DataScheme::bindBaseTypes() {
		BinderWrap *bw = NULL;
		for (DataObjectDef *obt = this->getBaseTypes(); obt != NULL; obt = obt->getNextBase()) {
			addBindEntity(obt, bw);
		}
		return bw;
	}
	
	bool DataScheme::isTypeDefId(int objId) {
		DataObjectDef *obd = this->getObjById(objId);
		if (obd == NULL) return false;
		return obd->getIsTypedef();
	}
	
	Signature *DataObjectDef::createSignature() {
		Deb::ug("creating signature of myself, my id: %d.", this->getMyId());
		Signature *sig = NULL;
		if (kind == TC_MDN_LINK) {
			sig = new SigRef(targetId);
		} else if (kind == TC_MDN_ATOMIC) {
			sig = new SigAtomType(type);
		} else if (kind == TC_MDN_COMPLEX) {
			sig = new SigColl(Signature::SSTRUCT);
			DataObjectDef *point = this->getSubObjects();
			while (point != NULL) {
				Signature *son = point->createSignature();
				((SigColl *)sig)->addToMyList(son);
				point = point->getNextSub();
				/**! pom = pom->getNextSub() ! >:-0 */
			}
		} else if (kind == TC_MDN_DEFTYPE) {
			if (this->getTypeObj() != NULL) {
				sig = this->getTypeObj()->createSignature();
				if (this->getTypeObj()->getIsDistinct())
					sig->setTypeName(this->getTypeObj()->getName());
			}
		} else {
			sig = new SigRef(getMyId()); // default behavior, if unkown kind appears.
		}
		
		if (sig != NULL) { //set all its attributes.
			sig->setCard(card);
			if (getIsTypedef() && getIsDistinct()) sig->setTypeName(name);
		}
		return sig;
	}
	
	Signature *DataObjectDef::createNamedSignature() {
		Deb::ug("creating namedSignature of myself, my id: %d.", this->getMyId());
		Signature *sig = NULL;
		Signature *valSig = NULL;
		if (kind == TC_MDN_LINK) {
			valSig = new SigRef(targetId);
		} else if (kind == TC_MDN_ATOMIC) {
			valSig = new SigAtomType(type);
		} else if (kind == TC_MDN_COMPLEX) {
			valSig = new SigColl(Signature::SSTRUCT);
			DataObjectDef *point = this->getSubObjects();
			while (point != NULL) {
				Signature *son = point->createNamedSignature();
				((SigColl *)valSig)->addToMyList(son);
				point = point->getNextSub();
			}
		} else if (kind == TC_MDN_DEFTYPE) {
			if (this->getTypeObj() != NULL) {
				valSig = this->getTypeObj()->createNamedSignature();
				cout << "deftype namedSig: " << valSig->toString() << ", but will return...\n";
				if (valSig->type() == Signature::SBINDER) {
					valSig = ((StatBinder *) valSig)->getValue();
				}
			}
		} else {
			valSig = new SigRef(getMyId()); // default behavior, if unkown kind appears.
		}
		if (valSig != NULL) {
			valSig->setCard(card);
			if (getIsTypedef() && getIsDistinct()) valSig->setTypeName(name);
		}
		sig = new StatBinder(this->name, valSig);
		if (valSig != NULL) sig->copyAttrsOf(valSig);
// 		if (sig != NULL) { //set all its attributes.
// 			sig->setCard(card);
// 			if (isDistinct) sig->setTypeName((typeName != "" ? typeName : name));
// 		}
		return sig;
	}
		
	DataObjectDef * DataScheme::createDataObjectDef(ObjectPointer *mainOp, Transaction *tr){
		Deb::ug("start createDataObjectDef \n");
		if (mainOp == NULL || mainOp->getLogicalID() == NULL) { return NULL;}
		DataObjectDef *oldOp = NULL;
		if ((oldOp = this->getObjById(mainOp->getLogicalID()->toInteger())) != NULL) {
			Deb::ug("Object already created and hashed\n");
			return oldOp;
		};
		DataObjectDef * datObDef = new DataObjectDef();	// this object is to be created and filled in. 
		datObDef->setMyId(mainOp->getLogicalID()->toInteger());
		vector<LogicalID*>* v = mainOp->getValue()->getVector();
		if (mainOp->getName() == TC_MDN_NAME){
			datObDef->setIsTypedef(false);
			datObDef->setIsBase(true);
			this->addBaseObj(datObDef);
		} else if (mainOp->getName() == TC_MDNT_NAME) {
			datObDef->setIsTypedef(true);
			datObDef->setIsBase(true);
			this->addBaseType(datObDef);
		} else {
			datObDef->setIsTypedef(false);
			this->addObj(datObDef);
		}
		for (unsigned   int j = 0; j < v->size(); j++){
			ObjectPointer *op = NULL;
			tr->getObjectPointer( (*v)[j], Store::Read, op, false);
			if (op->getName() == TC_MDS_NAME){
				datObDef->setName(op->getValue()->getString());
			} else if (op->getName() == TC_MDS_KIND) {
				datObDef->setKind(op->getValue()->getString());
			} else if (op->getName() == TC_MDS_CARD){
				datObDef->setCard(op->getValue()->getString());
			} else if (op->getName() == TC_MDS_ISDIST) {
				datObDef->setIsDistinct(op->getValue()->getString() == "1" ? true : false);
				//datObDef->setTypeName
			} else if (op->getName() == TC_MDS_REFNAME) {
				string refName = op->getValue()->getString();
				datObDef->setRefName(refName);
//		if (datObDef->getTarget() == NULL) {
				LogicalID *lid = getRootIdByName(refName);
				if (lid == NULL) {
					cout << "datascheme incomplete, missing " << refName << ".." << endl;
					this->setComplete(false);
					this->addMissedRoot(refName);
					return NULL;
				} else {
					ObjectPointer *pom;
					datObDef->setTargetId(lid->toInteger());
					tr->getObjectPointer(lid, Store::Read, pom, false);
					datObDef->setTarget(this->createDataObjectDef(pom, tr));
				}
//		} else {cout << "TARGET already filled BY ORDINARY TARGET SUBOBJ. !" << endl;}
			} else if (op->getName() == TC_MDS_TYPENAME) {
				string tName = op->getValue()->getString();
				datObDef->setTypeName(tName);
				cout << "filling type object ref., name: " << tName << endl;
				LogicalID *lid = getRootIdByName(tName);
				if (lid == NULL) {
					cout << "datascheme incomplete, missing type: " << tName << "." << endl;
					this->setComplete(false);
					this->addMissedRoot("type: "+tName);
				} else {
					ObjectPointer *ptr;
					tr->getObjectPointer(lid, Store::Read, ptr, false);
					datObDef->setTypeObj(this->createDataObjectDef(ptr, tr));
					if ((datObDef->getTypeObj() == NULL) || !(datObDef->getTypeObj()->getIsTypedef())) {
						this->setComplete(false);
						this->addMissedRoot("type: "+tName);
					}
				}
			} else if (op->getName() == TC_MDS_TYPE) {
				datObDef->setType(op->getValue()->getString());    
			} else if (op->getName() == TC_MDS_OWNER) {
				datObDef->setOwnerId(op->getValue()->getPointer()->toInteger());
				ObjectPointer * pom;
				tr->getObjectPointer(op->getValue()->getPointer(), Store::Read, pom, false);
				datObDef->setOwner(this->createDataObjectDef(pom, tr));
			} else if (op->getName() == TC_MDS_TARGET) {
				datObDef->setTargetId(op->getValue()->getPointer()->toInteger());
				if (datObDef->getTarget() == NULL) { //may have been filled by name..s
					cout << "FILLING TARGET AS TARGET" << endl;
					ObjectPointer * pom;
					tr->getObjectPointer(op->getValue()->getPointer(), Store::Read, pom, false);
					datObDef->setTarget(this->createDataObjectDef(pom, tr));
				}
			} else if (op->getName() == TC_SUB_OBJ_NAME) {
				ObjectPointer * pom;
				tr->getObjectPointer(op->getLogicalID(), Store::Read, pom, false);
				DataObjectDef * subobj = this->createDataObjectDef(pom, tr);	    
				if (subobj != NULL && subobj->getOwner() == datObDef){
					datObDef->addSubObject(subobj);
				}
			}
		}
		return datObDef;
	}

	int DataScheme::readData() {
		readData(NULL);
		return 0;
	}

	int DataScheme::readData(Transaction *tr){
		Deb::ug("-------------------readData START--------------------------------------");
		vector<ObjectPointer *> * roots;
		vector<ObjectPointer *> * rootTypes;

		bool newTransaction = false;
		if (tr == NULL) {
			Deb::ug("Have to create new transaction to read datascheme.");
			TransactionManager::getHandle()->createTransaction(tr);
			newTransaction = true;
		}	
		tr->getRoots(TC_MDN_NAME, roots);	
		tr->getRoots(TC_MDNT_NAME, rootTypes);
		if (Deb::ugOn()) 
			cout << "GOT " << roots->size() << " roots(MDN) and " << rootTypes->size() << " rootTypes(MDNT)\n";
		registerRoots(roots, tr);
		registerRoots(rootTypes, tr);
	
		createObjectDefs(roots, tr);
		createObjectDefs(rootTypes, tr);

		delete roots;
		delete rootTypes;
		if (newTransaction) tr->commit();
		Deb::ug("-------------------readData END--------------------------------------");
		return 0;
	};

	void DataScheme::registerRoots(vector<ObjectPointer *> *roots, Transaction *tr) {
		for (unsigned int i = 0; i < roots->size(); i++){
			ObjectPointer * mdnOp = (*roots)[i];
			vector<LogicalID*> *v = mdnOp->getValue()->getVector();
			for (unsigned int j = 0; j < v->size(); j++) {
				ObjectPointer *op = NULL;
				tr->getObjectPointer( (*v)[j], Store::Read, op,false);
				if (op->getName() == "name") {
					rootMap[op->getValue()->getString()] = mdnOp->getLogicalID();
					break;
				} 
			}
		}
	}

	void DataScheme::createObjectDefs(vector<ObjectPointer *> *roots, Transaction *tr) {
		for (unsigned int i = 0; i < roots->size(); i++){
			ObjectPointer * mdnOp = (*roots)[i];
			createDataObjectDef(mdnOp, tr);
		}
	}
	
	//all existing metadata roots should be available this way
	LogicalID *DataScheme::getRootIdByName(string name) {
		if (rootMap.find(name) == rootMap.end()) return NULL;
		else return rootMap[name];
	}

	//add given name to vector of missing roots only if its not there already.
	void DataScheme::addMissedRoot(string name) {
		for (unsigned int i = 0; i < missedRoots.size(); i++) {
			if (missedRoots[i] == name) return;
		}
		missedRoots.push_back(name);
	}
/***********************************************************************************************************/

	DataScheme* DataScheme::datScheme;
	
	DataScheme* DataScheme::dScheme() {
		if (datScheme == NULL) {
			ErrorConsole ec("QueryParser");
			ec << "dScheme initialized out of any transactions.\n";
			datScheme = new DataScheme();
			datScheme->readData();
		}
		return datScheme;
	}
	
	DataScheme* DataScheme::dScheme(Transaction *tr) {
		Deb::ug("DataScheme::dScheme(tr) \n");
		if (datScheme == NULL) {
			datScheme = new DataScheme();
			datScheme->readData(tr);
			Deb::ug("DataScheme::dScheme() has just read its objects and types\n");
		}		
		DataObjectDef *pom = datScheme->getBaseObjects();
		Deb::ug("DataScheme has these base objects: ");
		if (Deb::ugOn()) {
		/* ?? segfault used to come up here, at first call */
			while (pom != NULL) {
				fprintf(stderr, "[%d]", pom->getMyId());	
				pom = pom->getNextBase();
			}
		}
		return datScheme;
	};
	
	void DataScheme::reloadDScheme() {
		reloadDScheme(NULL);
	}
	
	void DataScheme::reloadDScheme(Transaction *tr){
		Deb::ug("reloading data scheme\n");
		if (datScheme != NULL) delete datScheme;
		datScheme = new DataScheme();
		datScheme->readData(tr);
	}


}
/************************** KONIEC tam dalej sa stare wersje *********************************************************/



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
