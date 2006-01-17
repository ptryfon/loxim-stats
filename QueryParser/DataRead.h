#ifndef _DREAD_H_
#define _DREAD_H_


#include <iostream>
#include <vector>
#include <list>
#include <string>
//#include "Stack.h"
#include "ClassNames.h"


using namespace std;
//using namespace QStack;

namespace QParser {


	
	
	
	/*******************************************************
	 *													   *
	 *	RAMOWA IMPLEMENTACJA STRUKTURY METABAZY W PAMIECI  *
	 *													   *
	 *******************************************************/

	class DataObjectDef
	{
	protected:
		DataObjectDef *nextBase;	/* lista definicji bazowych obiektow */
/* jsi ??? */	DataObjectDef *nextColl;	/* !=NULL tylko tam, gdzie to elt. jakiejs kolekcji. */	
		DataObjectDef *nextSub;		/* lista podobjektow jakiegos complex object.*/
		DataObjectDef *nextHash;	/* lista dla jednego pola tab. haszujacej. */
		int myId;	/*TODO: czy to jest int? czy unsigned int? czy co? trzeba chyba u executorow sprawdzic*/
		string name;
		string card;
		string kind;
		bool isBase;	/*czy owner jest NULL*/
		
		string type; 				/* TYLKO U ATOMIC */
		DataObjectDef *subObjects;	/* TYLKO U COMPLEX */
		//list<DataObjectDef *> subObjects;	/* TYLKO U COMPLEX */
		DataObjectDef *target;		/* TYLKO U LINK */
		DataObjectDef *owner;		/* o ile to nie ob. bazowy. wtedy null. */
		
	public:
		virtual ~DataObjectDef(){};
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
		
		virtual string getKind(){return kind;}
		
		virtual void setKind(string k){kind = k;}
		virtual DataObjectDef * getTarget(){return target;}
		virtual void  setTarget(DataObjectDef * t){target = t;}
		/* TODO: get/set na wiekszosci tego powyzej... + konstruktory sensowne. */
		virtual void addSubObject(DataObjectDef * nobj){
		    nobj->nextSub = this->nextSub;
		    this->nextSub = nobj;
		}
		virtual void setType(string t){type = t;}
		virtual string getType(){return type;}
		virtual void setOwner(DataObjectDef * o){owner = o;}
		virtual DataObjectDef * getOwner(){return owner;} 
	};
	
	class DataScheme 
	{	

	static DataScheme* datScheme;
	protected:
	DataObjectDef *baseObjects;
	DataObjectDef *refTable[100]; /*TODO: czy tak sie deklaruje tablice wskaznikow???*/

	
	public:
		DataScheme(){};
		int readData();	
	    	
		static DataScheme *dScheme();
		virtual int hashFun (int objId) {return (objId % 100); }
	
		virtual void hashIn (int tabPos, DataObjectDef *newObj) {
			newObj->setNextHash(refTable[tabPos]);	
			refTable[tabPos] = newObj;			
		}
		
		virtual DataObjectDef *getBaseObjects() {return baseObjects;}
		virtual void addObj(DataObjectDef *obj){
		    int newPlace = this->hashFun(obj->getMyId());
		    this->hashIn(newPlace, obj);
		}
			
		virtual void addBaseObj (DataObjectDef *newOne) {
			// DataObjectDef *pom;
//			int newPlace;
			/*	//	FIXME	nie wiem o co tu chodzilo, czy dodanie obiektu do listy??
			if (this->baseObjects != NULL) {
				pom = this->baseObjects->getNextBase();		// czy w ten sposob nie traci sie 
				this->baseObjects->setNext(NULL);		// pierwszego elemnetu??
				delete this->baseObjects;
				newOne->setNext(pom);				
			}
			*/			
			newOne->setNextBase(this->baseObjects);
			this->baseObjects = newOne;		/*no matter if it was empty before or not. */
			/* now insert it to the hashtable... */
			addObj(newOne);
			// newPlace = this->hashFun(newOne->getMyId());
			// this->hashIn(newPlace, newOne);
		}
		
		
		virtual DataObjectDef *getObjById (int objId) {
			int itsPlace = this->hashFun (objId);
			DataObjectDef *point = refTable [itsPlace];
			bool halt = false;
			while (not halt) {
				if (point == NULL) {halt = true;}
				else if (point->getMyId() == objId) 
					halt = true;
				else point = point->getNextHash();		
			};
			return point; /*UWAGA to moze byc NULLem (jesli nie znalazl. )*/
			
		};
		
		
		virtual BinderWrap *statNested (int objId);
//		virtual list<StatBinder *> statNested (int objId); 

		
		/*function below uses a list impl. of BinderWraps. you can change it ...  */
		virtual BinderWrap* bindBaseObjects(); 
		
	};	
		
}

#endif



