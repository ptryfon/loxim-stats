#ifndef _DREAD_H_
#define _DREAD_H_


#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <string>
//#include "Stack.h"
#include "ClassNames.h"
#include "TransactionManager/Transaction.h"
#include "Store/DBStoreManager.h"
#include "Store/DBObjectPointer.h"


using namespace std;
//using namespace QStack;

namespace QParser {


	
	
	
	/*******************************************************
	 *													   *
	 *	Implementation of metabase in memory               *
	 *													   *
	 *******************************************************/

	class DataObjectDef
	{
	protected:
		DataObjectDef *nextBase;	/* list of definitions of base objects */
/* jsi ??? */	DataObjectDef *nextColl;	/* !=NULL only if it belongs to a collection */	
		DataObjectDef *nextSub;		/* list of subobjects of a complex object.*/
		DataObjectDef *nextHash;	/* list for one field in a hash table */
		int myId;	
		string name;
		string card;
		string kind;
		bool isBase;	/*if owner is NULL*/
		bool isTypedef;		/*true for typedef nodes. */
		bool isDistinct;	/* only for typedef base nodes.*/
		
		string type; 				/* only in ATOMIC */
		DataObjectDef *subObjects;	/* only in COMPLEX */
		//list<DataObjectDef *> subObjects;	/* only in COMPLEX */
		DataObjectDef *target;		/* only in LINK */
		int		targetId;
		string refName;				/* only in link objects - present when target object not present..? */
		DataObjectDef *owner;		/* null in base objects */
		int 		ownerId;
		string typeName;			/* for objects/subobjects declared by a defined type..*/
		DataObjectDef *typeObj;
		
	public:
		DataObjectDef() {initPointers(); typeName = ""; refName = "";}
		void initPointers() {
			subObjects = NULL; target = NULL; owner = NULL; nextBase = NULL; nextSub = NULL; nextHash = NULL; typeObj = NULL;
		}
		virtual ~DataObjectDef(){
			if (nextBase != NULL) delete nextBase;
			if (subObjects != NULL) delete subObjects;
			if (nextSub != NULL) delete nextSub;}
		virtual DataObjectDef * getSubObjects(){return subObjects;};
		virtual void setSubObjects(DataObjectDef * sub){subObjects = sub;}
		virtual void setNextHash(DataObjectDef *p){this->nextHash = p;}
		virtual DataObjectDef * getNextHash() {return nextHash;}
		virtual void setNextBase(DataObjectDef *next){nextBase=next;}
		virtual DataObjectDef * getNextBase(){return nextBase;}
		virtual DataObjectDef * getNextSub() {return nextSub;}
		virtual int getMyId(){return myId;}
		virtual void setMyId(int id){myId = id;}
		virtual string getName(){return name;}
		virtual void setName(string nname) {name = nname;}
		virtual string getCard(){return card;}
		virtual void setCard(string ncard){card = ncard;}
		
		virtual void setIsBase(bool b){isBase = b;}
		
		virtual bool getIsBase(){return isBase;}
		/** TC add-ons .. */
		virtual void setIsTypedef(bool isTd) {isTypedef = isTd;}
		virtual bool getIsTypedef() {return isTypedef;}
		virtual void setIsDistinct(bool isD) {isDistinct = isD;}
		virtual bool getIsDistinct() {return isDistinct;}
		virtual void setRefName(string rName) {refName = rName;}
		virtual string getRefName() {return refName;}
		virtual void setTypeName(string tName) {typeName = tName;}
		virtual string getTypeName() {return typeName;}		
		virtual DataObjectDef *getTypeObj() {return typeObj;}
		virtual void setTypeObj(DataObjectDef * type) {typeObj = type;}
		/** TC add-ons - end.. */
		virtual string getKind(){return kind;}
		
		virtual void setKind(string k){kind = k;}
		virtual DataObjectDef * getTarget(){return target;}
		virtual void  setTarget(DataObjectDef * t){target = t;}
		
		virtual void addSubObject(DataObjectDef * nobj){
		    nobj->nextSub = this->subObjects;
		    this->subObjects = nobj;
		}
		virtual void setType(string t){type = t;}
		virtual string getType(){return type;}
		virtual void setOwner(DataObjectDef * o){owner = o;}
		virtual DataObjectDef * getOwner(){return owner;} 
		virtual void setOwnerId(int id){ownerId = id;};
		virtual int getOwnerId(){return ownerId;};
		virtual void setTargetId(int id){targetId = id;};
		virtual int getTargetId(){return targetId;};
		virtual Signature *createSignature();
	};
	
	class DataScheme 
	{	

	static DataScheme* datScheme;
	protected:
	DataObjectDef *baseObjects;
	DataObjectDef *baseTypes;
	DataObjectDef *refTable[100]; 
	map<string, LogicalID*> rootMap;
	bool isComplete;	/* flag saying if the whole data scheme is ok (eg. not missing any pointer elts) */
	bool isUpToDate;
	vector<string> missedRoots;
	
	DataObjectDef *createDataObjectDef(ObjectPointer *op, Transaction * tr);
	void registerRoots(vector<ObjectPointer *> *roots, Transaction *tr);
	void createObjectDefs(vector<ObjectPointer *> *roots, Transaction *tr);
	
	public:
		DataScheme(){
			this->baseObjects = NULL;
			this->baseTypes = NULL;
			this->isComplete = true;
			this->isUpToDate = true;
			for (int i = 0; i < 100; i++)
				refTable[i] = NULL;
		};
		virtual ~DataScheme() {
			if (baseObjects != NULL) delete baseObjects; 
			if (baseTypes != NULL) delete baseTypes; 
			cout << "\n jsi \n !!!!!!!!!! \n destr dataSCHME !!!!!!\n";
		};
		int readData();	
		int readData(Transaction *tr);
	    	
		LogicalID *getRootIdByName(string name); //looks for roots with given name. returns NULL value when name not found.
		static DataScheme *dScheme();
		static DataScheme *dScheme(Transaction *tr);
		static void reloadDScheme(Transaction *tr);
		static void reloadDScheme();
		
		void addMissedRoot(string name);
		vector<string> getMissedRoots() {return missedRoots;}
		void setComplete(bool complete) {isComplete = complete;}
		bool getIsComplete() {return isComplete;}
		bool getIsUpToDate() {return isUpToDate;}
		void setUpToDate(bool uptd) {isUpToDate = uptd;}
		
		virtual int hashFun (int objId) {return (objId % 100); }
	
		virtual void hashIn (int tabPos, DataObjectDef *newObj) {
			newObj->setNextHash(refTable[tabPos]);	
			refTable[tabPos] = newObj;			
		}
		
		virtual DataObjectDef *getBaseObjects() {return baseObjects;}
		virtual DataObjectDef *getBaseTypes() {return baseTypes;}
		
		virtual void addObj(DataObjectDef *obj){
		    int newPlace = this->hashFun(obj->getMyId());
		    this->hashIn(newPlace, obj);
		}
			
		virtual void addBaseType (DataObjectDef *newOne) {
			newOne->setNextBase(this->baseTypes);
			this->baseTypes = newOne;
			addObj(newOne);
		}
		
		virtual void addBaseObj (DataObjectDef *newOne) {	
			newOne->setNextBase(this->baseObjects);
			this->baseObjects = newOne;		/*no matter if it was empty before or not. */
			/* now insert it to the hashtable... */
			addObj(newOne);
		}
		
		
		virtual DataObjectDef *getObjById (int objId) {
			int itsPlace = this->hashFun (objId);
			DataObjectDef *point = refTable [itsPlace];

			bool halt = false;
			while (not halt) {
				if (point == NULL) {halt = true;}
				else { 
				    if (point->getMyId() == objId) {
						halt = true;
				    }
				    else {
						point = point->getNextHash();
				    }	
				}
			};
	;
			return point; /*it can be NULL if wasnt found*/
			
		};
		
		virtual BinderWrap *statNested (int objId, TreeNode *treeNode);
		virtual BinderWrap *statNested (DataObjectDef *candidate, TreeNode *treeNode, string obName, string obCard);
		/*function below uses a list impl. of BinderWraps. you can change it ...  */
		virtual BinderWrap* bindBaseObjects(); 
		virtual Signature *signatureOfRef(int objId);
	};	
		
}

#endif



