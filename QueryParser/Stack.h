#ifndef _STACK_H_
#define _STACK_H_


#include <iostream>
#include <vector>
#include <string>
#include <list>
#include "Deb.h"
#include "ClassNames.h"


using namespace std;

namespace QParser {

	class ElemContent /* abstract superclass of elements found in sections belonging to a stack. */
	{
	public:
		ElemContent() {};
		virtual void putToString() {
			cout << "( Some ElemContent ) ";
		}
		virtual ~ElemContent() {};
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
		virtual void putToString() {
			if (this->content == NULL) cout << "# emptyQStackElem #"; 
			else { cout << "#"<< myNumber << ": "; content->putToString(); cout << " #";}
			cout << endl;
		}
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
		virtual bool isEmpty () { return (this->elts == NULL);};
		virtual QStackElem *getElt(int eltNo) {if (this->isEmpty()) return NULL; return this->elts->getElt(eltNo);}
		

		virtual QStackElem *top () {
/*			return this->getElt(this->getSize()); */
			if (this->isEmpty()) return NULL;
			else return this->elts->top();
		}	    	    
		virtual int push (QStackElem *newSection) {
			if (newSection == NULL) return -1; /* can't push a NULL on the stack.. ?*/
			if (this->isEmpty()) this->setElts (newSection);
			else this->setElts (this->elts->push (newSection));
			this->incSize();
			this->elts->setNumber(this->getSize());
			//newSection->setNumber(this->getSize());
			
			return 0;
		};
		virtual int pop () {
			if (this->isEmpty()) return -1;/* TODO: blad tu jest zglaszany .. ? */
			else {
				this->decSize();
				this->setElts (this->elts->pop());
				return 0;}
		}

		virtual void putToString() {
			if (this->isEmpty()) cout << " < EMPTY STACK > "; 
			else { cout <<" < my size: " << currSize << "||" << endl; elts->putToString(); cout << " >";}
			cout << endl;
		}
		virtual ~QStack() {if (this->elts != NULL) delete this->elts;}
    };
	

	class Signature : public ElemContent
	{
	protected:
		int typ;
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
		Signature (int _type) : typ(_type) {this->next = NULL;}
		Signature () {this->next = NULL;}
		virtual void setNext (Signature *newNext) {this->next = newNext;}
		virtual Signature *getNext () {return this->next;}
		
		virtual int type() {return 0;}
		virtual bool isAtom () {return false;} /* overridden only in SigAtom */
		virtual bool isColl () {return false;} /* overridden only in SigColl */
		virtual bool isBinder () {return false;}
		virtual BinderWrap *statNested() {return NULL;} /*the default behaviour of a signature*/
		virtual Signature *clone();
		virtual void putToString () { cout << "(signature) ";if (this->next != NULL) next->putToString();}
		virtual ~Signature() { if (this->next != NULL) delete this->next; }
	};
	
	class SigColl : public Signature  /* podklasa sygnatur - sygnatury kolekcji .. */
	{
	protected:
		/*TODO: czy lepiej zeby to byly vectory...? bo teraz to lista, po prostu kazdy ma atr. next... */
		/*vector<Signature*> set;
		  vector<Signature*>::iterator it;  */
		Signature *myList;
		Signature *myLast;
		int collType;	
	public:
		SigColl (int _collType) : collType(_collType) {myList = NULL; myLast = NULL; next = NULL;}
		SigColl () {this->collType = Signature::SBAG; myList = NULL; myLast = NULL; next = NULL;} /*bag is the default collection*/
		SigColl (int _collType, Signature *_myList) : myList(_myList), collType(_collType) {myLast = _myList; next = NULL;};
		virtual bool isColl () {return true;}
		virtual bool isEmpty() {return (myList == NULL);}
		virtual int type() {return this->collType;}
		virtual Signature *getMyLast () {return myLast;}		
		virtual void setMyLast (Signature *nl) {if (nl->getNext() == NULL) myLast = nl; else setMyLast (nl->getNext());}
		virtual void setElts (Signature *newList) {this->myList = newList; this->myLast = newList;}
		virtual Signature *getMyList () {return this->myList;}
		
		virtual void addToMyList(Signature *newElt) {
			if (this->isEmpty()) {this->setElts(newElt);}
			else {this->myLast->setNext(newElt); newElt->setNext (NULL); myLast = newElt;}
		}
		virtual BinderWrap *statNested();
		virtual Signature *clone();							
		virtual void putToString() {
			fprintf (stderr, "{");
			if (this->myList != NULL) myList->putToString();
			fprintf (stderr,  "} ");
			if (next != NULL) next->putToString();
		}
		virtual ~SigColl() {if (myList != NULL) delete this->myList; if (next != NULL) delete next;}	
	};
	
	class SigAtomType : public Signature 
	{
	protected:
		string atomType;	/*"int" lub "double" lub "string" lub "bool"*/		
	public:
		SigAtomType() {next = NULL;}
		SigAtomType(string _atomType) : atomType(_atomType) {next = NULL;}
		virtual bool isAtom () {return true;}
		virtual int getNumb (string atype) {
			if (atype == "int") return Signature::SINT;
			if (atype == "double") return Signature::SDOUBLE;
			if (atype == "string") return Signature::SSTRING;
			if (atype == "bool") return Signature::SBOOL;
			return -1; /*atype must be 1 of the above*/
		}
		virtual int type() {return this->getNumb(atomType);}
		virtual void setType(string newType) {this->atomType = newType;}
		virtual Signature *clone();
		virtual void putToString() {
			cout << "(" << this->atomType << ") ";
			if (next != NULL) next->putToString();
		}		
		virtual ~SigAtomType() {if (next != NULL) delete next;}
	};
		
	class SigRef : public Signature 
	{
	protected:
		int refNo;
	public:
		SigRef() {next = NULL;}
		SigRef(int _refNo) : refNo(_refNo) {next = NULL;}
		virtual int getRefNo () {return this->refNo;}
		virtual void setRefNo (int newNo) {this->refNo = newNo;}
		virtual int type() {return Signature::SREF;}
		virtual BinderWrap *statNested();/*implmnted in Stack.cpp*/
		virtual Signature *clone();
		virtual void putToString() {
			cout << "ref(" << refNo << ") ";
			if (next != NULL) next->putToString();
		}
		virtual ~SigRef() {if (next != NULL) delete next; Deb::ug("killed 1 ref\n");}
		
	};
	class StatBinder : public Signature
	{
	protected:
		string name;
		Signature *value;
	public:
		StatBinder(string _name, Signature *_value) : name(_name), value(_value) {next = NULL;};
		StatBinder(string _name) : name(_name) {next = NULL;};
		virtual string getName() {return this->name;};
		virtual Signature *getValue() {return this->value;};
		virtual void setName(string newName) {this->name = newName;};
		virtual void setValue(Signature *newValue) {this->value = newValue;};
		virtual int type() {return Signature::SBINDER;}
		virtual bool isBinder() {return true;}
		virtual BinderWrap *statNested();
		virtual Signature *clone();
		virtual void putToString() {
			cout << name <<"(";
			if (value == NULL) cout << "__";
			else value->putToString();
			cout << ") ";
			if (next != NULL) next->putToString();
		}
		virtual ~StatBinder() {if (value != NULL) {Deb::ug( "value to kil\n");delete value; }
							if (next != NULL){ Deb::ug("next binder to kil\n"); delete next;}
							Deb::ug("killed a binder\n");};
	};
	
	class BinderWrap : public ElemContent	/* abstract superclass... */
	{ /* subclasses of BinderWrap give different implementations of structures 
	   * for groups of binders within one section on ENVS (eg. BinderList */
	protected:	
		int sectNumb;
		StatBinder *binder;
	public:
		virtual void setBinder(StatBinder *newBinder) {this->binder = newBinder;};
		virtual StatBinder *getBinder() {return this->binder;};
	
		virtual void setSectNumb(int newNr) {this->sectNumb = newNr;};
		virtual int getSectNumb() {return this->sectNumb;};
		
		
		virtual BinderWrap *addOne (BinderWrap *newElt) = 0;
		virtual BinderWrap *addPureBinder (StatBinder *newBndr) = 0;
		virtual BinderWrap *bindName(string aName) = 0;
		virtual void putToString() {cout << "name(sign)" << endl;};
	};	
	
	class BinderList : public BinderWrap
	{
	protected:
		BinderList *next;
	public:
		BinderList(StatBinder *_binder) {this->setBinder(_binder); this->next = NULL;}
		BinderList() {binder = NULL; next = NULL;}
		virtual void setNext(BinderList *newNext) {this->next = newNext;}
		virtual void setSectNumb (int newNr) {
			this->sectNumb = newNr; 
			if (this->next != NULL) next->setSectNumb(newNr);
		}
		virtual BinderWrap *addOne (BinderWrap *newBW) {
			((BinderList *)newBW)->setNext(this); 
			newBW->setSectNumb(this->sectNumb);
			return newBW;
		}
		virtual BinderWrap *addPureBinder (StatBinder *newBndr) {
			return this->addOne (new BinderList (newBndr));				
		}
		virtual BinderWrap *bindName(string aName) {
			if (this->binder->getName() == aName) return this;
			else if (this->next == NULL) return NULL;
			else return this->next->bindName(aName);
		}
		virtual void putToString() {
			if (this->binder == NULL) cout << "_noBinder_";
			else {cout << "[s:" << this->sectNumb << ","; 
				binder->putToString(); cout << "] ";}
			if (this->next != NULL) this->next->putToString();
		}
		virtual ~BinderList() {if (next != NULL) {Deb::ug("next bdList to kil\n");delete next;} if (binder != NULL){Deb::ug("binder to kil\n"); delete binder;} Deb::ug("kiled 1 bList\n");}
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
		virtual QStackElem *push (QStackElem *newSect) {
			((ListQStackElem *)newSect)->setNext(this);
			//newSect->setNumber((this->myNumber) + 1);
			return newSect;
		}
		virtual QStackElem *pop () { /* we know there is at least 1 elt to pop.. */
			ListQStackElem *sectPoint = this->getNext();
			this->next = NULL;
//			delete(this);	/* TODO: czy tak mo¿na ... ??? */
			return sectPoint;
		}
		virtual QStackElem *top () {
			return this;
		}
		virtual void putToString() {
			if (this->content == NULL) cout << "# LISTSection #"; 
			else { cout << "#"<< myNumber << ": "; content->putToString(); cout << " #";}
			cout << endl;
			if (this->next != NULL) this->next->putToString();
		}				
		virtual ~ListQStackElem() {if (this->next != NULL) delete this->next;}
	};
			
	/*a simple list implementations of sections of the ENV stack. */
    class StatEnvSection : public ListQStackElem 	/* these sections are elements of StatEnvStacks */
	{
	protected:
		//int myNumber;	/*numer danej sekcji */
		BinderWrap *content;
	public:
		StatEnvSection() {this->next = NULL; content = NULL;};
		StatEnvSection (BinderWrap *cnt) {this->content = cnt; this->next = NULL;
											this->content->setSectNumb(myNumber);}
		virtual void addBinder (BinderWrap *bw) { this->content = (this->content->addOne(bw));}
		virtual void addPureBinder (StatBinder *sb) {this->content = (this->content->addPureBinder(sb));}
		virtual void setContent(ElemContent *newCont) {this->content = (BinderWrap *)newCont; this->content->setSectNumb(myNumber);}
		virtual void setNumber(int newNr) {
			this->myNumber = newNr; 
			if (this->content != NULL) content->setSectNumb(newNr);
		}
		virtual BinderWrap *bindName (string aName) {
			BinderWrap *bw = this->content->bindName(aName);
			if (bw == NULL) {
				if (this->next == NULL) return NULL;
				else return ((StatEnvSection *)this->next)->bindName(aName);
			} else return bw;			
		}		

		virtual void putToString() {
			if (this->content == NULL) cout << "# emptySection #"; 
			else { cout << "#"<< myNumber << ": "; content->putToString(); cout << " #";}
			cout << endl;
			if (this->next != NULL) this->next->putToString();
		}
		virtual ~StatEnvSection () { 
			Deb::ug( "killing elts..\n");
			if (content != NULL) {delete (this->content);} 
			if (next != NULL) {delete (this->next);}
			Deb::ug("killed a section\n");};
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
			Deb::ug("znalazlem getElt");
			if (s == NULL) { Deb::ug("bndNameHere-zwracam NULL"); return NULL;}
			else return ((StatEnvSection *) s)->bindName(aName);
			return NULL;
		}
		virtual int pushBinders (BinderWrap *bw) {/* a collection of binders, not packed in a section yet... */
			if (bw == NULL) {Deb::ug("null binders not pushed on envs\n"); return -1;};
			ListQStackElem *newSect = new StatEnvSection (bw);
			if (this->isEmpty()) this->setElts (newSect);
			else this->setElts (elts->push (newSect));
			this->incSize();
			this->elts->setNumber(this->getSize());
			return 0;
		}	
		virtual int push (QStackElem *newSection) {
			if (newSection == NULL) return -1; /* can't push a NULL on the stack.. ?*/
			if (this->isEmpty()) this->setElts (newSection);
			else this->setElts (this->elts->push (newSection));
			this->incSize();
			this->elts->setNumber(this->getSize());
			//newSection->setNumber(this->getSize());
			
			return 0;
		};	
		
		virtual ~StatEnvStack() { 
		if (elts != NULL) { delete this->elts;} Deb::ug("killed elts\n");};
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
		
		virtual Signature *getContent() {return this->content;}
		virtual ~StatQResElt() {if (content != NULL) delete content; if (next != NULL) delete this->next;}
	};
	
	class StatQResStack : public QStack
	{
	protected:
/*		Signature *results;   --> elts
		int currSize;     --> currSize from super class. 
*/		
	public:
		StatQResStack() {this->setElts(NULL); this->setSize(0);}
		virtual int pushSig (Signature *newSig) {
			if (newSig == NULL) return -1;
			return this->push(new StatQResElt (newSig));
		} 

		
		virtual Signature *topSig() {
			return ((Signature *)((StatQResElt *)elts)->getContent())->clone();
		}		
		virtual ~StatQResStack() {if (elts != NULL) delete elts;}

//		virtual Signature *topSig () {
//			return ((StatQResElt *)elts)->getContent();
//		}
		

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
	
	class Optimiser {
	protected:
	    StatQResStack *sQres;
	    StatEnvStack *sEnvs;
	public:
	    Optimiser() {sQres = NULL; sEnvs = NULL;}
	    virtual void setQres(StatQResStack *nq);
	    virtual void setEnvs(StatEnvStack *ne);
	    virtual int simpleTest();
	    virtual int stEvalTest(TreeNode *&tn);
	    virtual ~Optimiser();
	    
	
	
	};
	
	


}

#endif


