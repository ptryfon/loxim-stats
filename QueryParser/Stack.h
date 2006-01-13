#ifndef _STACK_H_
#define _STACK_H_


#include <iostream>
#include <vector>
#include <string>
using namespace std;

namespace QStack {

	class ElemContent /* abstract superclass of elements found in sections belonging to a stack. */
	{
	
	};
	/* TO CHYBA BEZ SENSU ..... ? */
	/* abstract superclass for structures holding single elements. eg. in an ENV Stack, a section has
		a pointer to its group of binders. It may be a simple list of them, but may be implemented as 
		some data structure which would make binding names within a large section fast. */

	class QStackElem 
	{    
    protected:

		ElemContent *content;	
		/* wskaznik na wartosc tego elementu stosu. Np. w stosie ENVS el-tami sa sekcje,
		   wiec content wskazuje na zbior binderow. W QRESie, content to sygnatura.. */
		int myNumber;								   
    public:
    	QStackElem() { };
		QStackElem(ElemContent *cont) {this->content = cont;}
	
		virtual ElemContent *getContent() {return this->content;}
		virtual void setContent(ElemContent *newCont) {this->content = newCont;}

		virtual int getNumber() {return this->myNumber;}
		virtual void setNumber(int newNr) {this->myNumber = newNr;}
		virtual QStackElem *getElt (int eltNo) = 0;
		virtual QStackElem *push(QStackElem *newElem) = 0;
		virtual QStackElem *pop() = 0;
		virtual QStackElem *top() = 0;
		virtual ~QStackElem() {}
    };
    
	/* abstract stack class.. */
    class QStack 
    {    
    protected:	
	    QStackElem *elts;	
		int currSize;		/* the size of the stack (the number of QStackElem elements.
						 * eg. for EnvStack - its the nr. of sections */
    public:
		virtual QStackElem *getElts() {return this->elts;}
		virtual void setElts(QStackElem *newElts) { this->elts = newElts; }
		virtual int getSize() {return this->currSize;}
		virtual void setSize(int newVal) {this->currSize = newVal;}
		virtual int incSize () {(this->currSize)++; return this->currSize;}
		virtual int decSize () {(this->currSize)--; return this->currSize;}

		virtual QStackElem *getElt(int eltNo) {return this->elts->getElt(eltNo);}
		virtual bool isEmpty () { return (this->elts == NULL);};

		virtual int push (QStackElem *newSection) {
			if (newSection == NULL) return -1; /* can't push a NULL on the stack.. ?*/
			this->setElts (this->elts->push (newSection));
			this->incSize();
			return 0;
		};
		virtual int pop () {
			if (this->isEmpty()) return -1;/* TODO: blad tu jest zglaszany .. ? */
			else {
				this->decSize();
				this->setElts (this->elts->pop());
				return 0;}
		}
		virtual QStackElem *top () {
/*			return this->getElt(this->getSize()); */
			if (this->isEmpty()) return NULL;
			else return this->elts->top();
		}	    	    
		virtual ~QStack() {if (this->elts != NULL) delete this->elts;}
    };
	
	class Signature : public ElemContent
	{
	protected:
		int type;
		Signature *next; 	/* !=NULL when this sign. is part of a collection.. like sequence or bag or sth. */
	public:
	    enum SigType {
		SSEQ      = 1,	/*sigColl */
		SBAG      = 2,	/*sigColl*/
		SSTRUCT   = 3,	/*sigColl...?*/
		SBINDER   = 4,
		SINT      = 5,	/*atom*/
		SDOUBLE   = 6,	/*atom*/
		SSTRING   = 7,	/*atom*/
		SBOOL     = 8,  /*atom*/
		SREF      = 9,
		SNOTHING  = 10
		};
		Signature (int _type) : type(_type) {this->next = NULL;}
		Signature () {this->next = NULL;}
		virtual void setNext (Signature *newNext) {this->next = newNext;}
		virtual Signature *getNext () {return this->next;}
		
		virtual bool isAtom () {return false;} /* overridden only in SigAtom */
		virtual bool isColl () {return false;} /* overridden only in SigColl */
		virtual ~Signature() { if (this->next != NULL) delete this->next; }
	};
	
	class SigColl : public Signature  /* podklasa sygnatur - sygnatury kolekcji .. */
	{
	protected:
		/*TODO: czy lepiej zeby to byly vectory...? bo teraz to lista, po prostu kazdy ma atr. next... */
		/*vector<Signature*> set;
		  vector<Signature*>::iterator it;  */
		Signature *myList;
		int collType;	
	public:
		SigColl (int _collType) : collType(_collType) {this->myList = NULL;}
		SigColl () {this->myList = NULL;}
		SigColl (int _collType, Signature *_myList) : myList(_myList), collType(_collType) {};
		virtual bool isColl () {return true;}
		virtual int type() {return this->collType;}
		virtual void setElts (Signature *newList) {this->myList = newList;}
		virtual Signature *getMyList () {return this->myList;}
		virtual void addToMyList(Signature *newElt) {newElt->setNext (this->myList); this->setElts(newElt);}
		
		virtual ~SigColl() {delete this->myList;}	
	};
	
	class SigAtomType : public Signature 
	{
	protected:
		string atomType;	/*"int" lub "double" lub "string" lub "bool"*/		
	public:
		SigAtomType() {}
		SigAtomType(string _atomType) : atomType(_atomType) {}
		virtual bool isAtom () {return true;}
		virtual string type() {return this->atomType;}
		virtual void setType(string newType) {this->atomType = newType;}
		virtual ~SigAtomType() {}
	};
		
	class SigRef : public Signature 
	{
	protected:
		int refNo;
	public:
		SigRef() {}
		SigRef(int _refNo) : refNo(_refNo) {}
		virtual int getRefNo () {return this->refNo;}
		virtual void setRefNo (int newNo) {this->refNo = newNo;}
		virtual int type() {return Signature::SREF;}
		virtual ~SigRef() {}
		
	};
	class StatBinder : public Signature
	{
	protected:
		string name;
		Signature *value;
	public:
		StatBinder(string _name, Signature *_value) : name(_name), value(_value) {};
		StatBinder(string _name) : name(_name) {};
		virtual string getName() {return this->name;};
		virtual Signature *getValue() {return this->value;};
		virtual void setName(string newName) {this->name = newName;};
		virtual void setValue(Signature *newValue) {this->value = newValue;};
		virtual int type() {return Signature::SBINDER;}
		
		virtual ~StatBinder() {if (this->value != NULL) delete this->value;};
	};
	
	class BinderWrap : public ElemContent	/* abstract superclass... */
	{ /* subclasses of BinderWrap give different implementations of structures 
	   * for groups of binders within one section on ENVS (eg. BinderList */
	protected:	
		StatBinder *binder;
	public:
		virtual void setBinder(StatBinder *newBinder) {this->binder = newBinder;};
		virtual StatBinder *getBinder() {return this->binder;};

		virtual BinderWrap *addOne (BinderWrap *newElt) = 0;
		virtual BinderWrap *bindName(string aName) = 0;
	};	
	
	class BinderList : public BinderWrap
	{
	protected:
		BinderList *next;
	public:
		BinderList(StatBinder *_binder) {this->setBinder(_binder); this->next = NULL;}
		virtual void setNext(BinderList *newNext) {this->next = newNext;}

		virtual BinderList *addOne (BinderList *newBW) {
			newBW->setNext(this); return newBW;
		}
		virtual BinderList *bindName(string aName) {
			if (this->binder->getName() == aName) return this;
			else if (this->next == NULL) return NULL;
			else return this->next->bindName(aName);
		}
	};

	class ListQStackElem : public QStackElem 
	{
	protected:
		ListQStackElem *next;
	public:
		virtual ListQStackElem *getNext() {return this->next;}
		virtual void setNext (ListQStackElem *newNext) {this->next = newNext;}
		
		virtual ListQStackElem *getElt(int sectNo) {
			if (this->myNumber == sectNo) return this;
			else if (this->myNumber < sectNo) return NULL;
			else if (this->next == NULL) return NULL;
			else return this->next->getElt(sectNo);
		}						
		virtual ListQStackElem *push (ListQStackElem *newSect) {
			newSect->setNext(this);
			newSect->setNumber((this->myNumber) + 1);
			return newSect;
		}
		virtual ListQStackElem *pop () { /* we know there is at least 1 elt to pop.. */
			ListQStackElem *sectPoint = this->getNext();
			this->next = NULL;
			delete(this);	/* TODO: czy tak mo¿na ... ??? */
			return sectPoint;
		}
		virtual ListQStackElem *top () {
			return this;
		}				
		virtual ~ListQStackElem() {if (this->next != NULL) delete this->next;}
	};
			
	/*a simple list implementations of sections of the ENV stack. */
    class StatEnvSection : public ListQStackElem 	/* these sections are elements of StatEnvStacks */
	{
	protected:
		int myNumber;	/*numer danej sekcji */
		BinderWrap *content;
	public:
		StatEnvSection() {this->next = NULL;};
		virtual void addBinder (BinderWrap *bw) { this->content = (this->content->addOne(bw));}

		virtual BinderWrap *bindName (string aName) {
			BinderWrap *bw = this->content->bindName(aName);
			if (bw == NULL) {
				if (this->next == NULL) return NULL;
				else return ((StatEnvSection *)this->next)->bindName(aName);
			} else return bw;			
		}		
		virtual ~StatEnvSection () { delete (this->content); delete (this->next);};
	};
	
	class StatEnvStack : public QStack 
	{

	public:
		StatEnvStack () {this->setElts(NULL); this->setSize(0);};
		StatEnvStack (QStackElem *_elts, int _currSize) 
			{this->setSize(_currSize); this->setElts(_elts);}

		virtual BinderWrap *bindName (string aName) {
			if (this->isEmpty()) return NULL;
			else return ((StatEnvSection *)this->top())->bindName(aName);
		}
		virtual BinderWrap *bindNameHere (string aName, int sectNo) {
			QStackElem *s = this->getElt(sectNo);
			if (s == NULL) return NULL;
			else return ((StatEnvSection *) s)->bindName(aName);
			return NULL;
		}
		virtual ~StatEnvStack() { delete this->elts; };
	};

	
	/*TODO: czy ta klasa wprowadza cokolwiek nowego poza tym, ze content jest typu 
		Signature ... ? ? ? */	
	class StatQResElt : public ListQStackElem
	{
	protected:
		Signature *content;
	public:
		StatQResElt() {this->next == NULL; this->content == NULL;};
		StatQResElt(Signature *cont) : content(cont) {this->next = NULL;};
		
		virtual ~StatQResElt() {delete content; delete this->next;}
	};
	
	class StatQResStack : public QStack
	{
	protected:
/*		Signature *results;   --> elts
		int currSize;     --> currSize from super class. 
*/		
	public:
//		virtual int push (QStackElem *newElt) {
//			if (newElt == NULL) return -1; /* can't push a NULL on the stack.. ?*/
//			newElt->setNext (this->getElts());
//			this->incSize();
//			newElt->setNumber(this->getSize());
//			this->setElts (newSection);			
//			return 0;
//		};
//		virtual int pop () {
//			QStackElem *pom;
//			if (this->isEmpty()) /* blad tu jest zglaszany */
//				return -1;
//			else {
//				this->decSize();
//				pom = this->getElts()->getNext();
//				this->getElts->setNext(NULL);
//				delete (this->getElts());
//				this->setElts (pom);
//				
//				return 0;
//			}
//		}
//		virtual QStackElem *top () {
/*			return this->getElt(this->getSize());*/
//			if (this->isEmpty()) return NULL;
//			else return this->getElts();
//		}		
	
	};
	
	


}

#endif


