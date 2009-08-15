#ifndef _STACK_H_
#define _STACK_H_


#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <list>
#include <QueryParser/Deb.h>
#include <QueryParser/ClassNames.h>
#include <Errors/ErrorConsole.h>
#include <Errors/Errors.h>

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
	/* TO CHYBA BEZ SENSU ..... ? No, bez sensu !*/
	/* abstract superclass for structures holding single elements. (...) */
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
	
		// ta metoda tu nic nie daje, nie powinno jej byc, 
		// podkasy maja jeszcze raz zdefiniowany atr content,
		// i to wywolanie tutaj zwraca ten kontent a nie ich wlasny !!!!
		virtual ElemContent *getContent() {return this->content;}	
		virtual void setContent(ElemContent *newCont) {this->content = newCont;}

		virtual int getNumber() {return this->myNumber;}
		virtual void setNumber(int newNr) {this->myNumber = newNr;}
		virtual QStackElem *getElt (int eltNo) = 0;
		virtual QStackElem *push(QStackElem *newElem) = 0;
		virtual QStackElem *pop() = 0;
		virtual QStackElem *top() = 0;
		virtual void putToString() {
			cout << "QstackElem:putToString:\n";
			if (this->getContent() == NULL) cout << "# emptyQStackElem #"; 
			else { cout << "#"<< myNumber << ": "; this->getContent()->putToString(); cout << " #";}
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
			else { cout <<" < my size: " << currSize << "||" << endl; 
				if (elts != NULL) elts->putToString(); 
				cout << " >";}
				cout << endl;
		}
//		virtual ~QStack() {if (this->elts != NULL) delete this->elts;}
		virtual ~QStack(){}
    };
	

	class Signature : public ElemContent
	{
	protected:
		int typ;
		Signature *next; 	/* !=NULL when this sign. is part of a collection.. like sequence or bag or sth. */
		TreeNode *dependsOn;	// death subqry
		string card;			// death
		string distinctTypeName;	//typeChecking system. this will often be empty/null. 
		string collectionKind;	//sequence OR bag OR ""- empty means its not a collection
		bool refed;				// was the 'ref' flag added.
	public:
		enum SigType {
			SSEQ      = 1,	/*sigColl */ //should not be used any more. (functionality replaced by collectionKind
			SBAG      = 2,	/*sigColl*/  //should not be used any more
			SSTRUCT   = 3,	/*sigColl*/	
			SBINDER   = 4,
			SINT      = 5,	/*atom*/
			SDOUBLE   = 6,	/*atom*/
			SSTRING   = 7,	/*atom*/
			SBOOL     = 8,  /*atom*/
			SREF      = 9,
			SNOTHING  = 10,
			SVAR	  = 11
		};
		//TODO: MUST WORK THIS OUT ! because collectionKind should be a parameter of a signature, 
		// just like card, or distinctTypeName. Whereas - here its a seperate type of signatures. 
		// So... should check all places where its really used (probably not many such places, because
		// thats not how it works). And introduce the new parameter: collectionKind. 
		// And introduce VariantSignatures !!! which appear v. seldom (basically when you do a union), 
		// but must have their own type here and somehow be represented in some decisionTables...? -> force the cast
		// operator on them... MH
		
		virtual string textType() {
			switch (type()) {// ?? shouldn't this be typ? instead of type? its ok.
				case Signature::SSEQ: return "list";
				case Signature::SBAG: return "bag";
				case Signature::SSTRUCT: return "struct";
				case Signature::SBINDER: return "binder";
				case Signature::SINT: return "integer";
				case Signature::SDOUBLE: return "double";
				case Signature::SSTRING: return "string";
				case Signature::SBOOL: return "boolean";
				case Signature::SREF: return "ref";
				case Signature::SVAR: return "variant";
				case Signature::SNOTHING: return "void";
				default: return "unknown";
			}
			return "";
		}
		
		Signature (int _type) : typ(_type) {init();}
		Signature () {init();}
		virtual void init();
		virtual void setNext (Signature *newNext) {this->next = newNext;}
		virtual Signature *getNext () {return this->next;}
	
		virtual string getTypeName() {return this->distinctTypeName;}
		virtual bool emptyTypeName() {return (this->distinctTypeName == "");} 
		virtual bool isDistinct() {return emptyTypeName();}
		virtual void setTypeName(string typeName) {this->distinctTypeName = typeName;}
		virtual string getCollKind() {return collectionKind;}
		virtual void setCollKind(string ck) {collectionKind = ck;}
		virtual TreeNode* getDependsOn(){return this->dependsOn;}
		virtual void setDependsOn(TreeNode *_dependsOn){this->dependsOn = _dependsOn;}
		virtual string getCard(){return this->card;}
		virtual void setCard(string _card){
			this->card = _card; //and set default collectionKind if needed...
			if (card[3] == '1') collectionKind = "";
			else if ((card[3] == '*') && (collectionKind == "")) collectionKind = "bag";
		}
		virtual void setCardMultiple() {
			stringstream conv;
			conv << card[0] << ".." << '*';
			card = conv.str();
		}
		virtual bool isRefed() {return refed;}
		virtual void setRefed() {refed = true;}
		virtual int type() {return 0;}
		virtual bool isAtom () {return false;} /* overridden only in SigAtom */
		virtual bool isColl () {return false;} /* overridden only in SigColl */
		virtual bool isBinder () {return false;}
		virtual string textCollKind() {return "";} //deprecated..now getCollKind() is enough.
		virtual BinderWrap *statNested(TreeNode *) {return NULL;} /*the default behaviour of a signature*/
		virtual Signature *clone();
		virtual Signature *cloneSole();
		virtual Signature *cloneFlat();
		virtual void putToString () { cout << "(signature) ";if (this->next != NULL) next->putToString();}
		virtual string toString() { return "BaseSignature";}
		virtual string attrsToString() {return distinctTypeName + "[" +card + "]" + collectionKind;}
		virtual void copyAttrsOf(Signature *sig) {
			setTypeName(sig->getTypeName()); setCollKind(sig->getCollKind()); setCard(sig->getCard());
			refed = sig->isRefed();
		}
		virtual bool isStructurallyEqualTo(Signature *sig) {//by default
			return ((sig != NULL) && (this->type() == sig->type()));
		} 
		virtual bool isSolelyEqualTo(Signature *sig) {//by default. in subclasses: does not compare nexts!
			return ((sig != NULL) && (this->type() == sig->type()));
		}
		virtual int compareNamedSigCrt(Signature * /*flatSig*/, bool /*needTName*/) {return 0;}
		virtual int compareRecNamedSigCrt(Signature *flatSig, bool needTName, vector<pair<int, int> > &vPairs);
		std::pair<int, int> cardToMapPair(string card);
		virtual Signature *deref(){return this->cloneSole();}//by default. works fine for atom signatures.s
		virtual ~Signature() {}// cout << "entered top SIG destructor" << endl; if (this->next != NULL) delete this->next; }
	};
	
	class SigColl : public Signature  /* Structures or variants... */
	{
	protected:
		/* this really ought to be a vector...would get rid of all the 'next' attributes... */
		/*vector<Signature*> set;vector<Signature*>::iterator it;  */
		Signature *myList;
		Signature *myLast;
		int collType;	
		
		virtual int compareSigCrt(Signature *flatSig, bool needTName, bool isRec, vector<pair<int, int> > &vPairs);
	public:
		SigColl (int _collType) : collType(_collType) {myList = NULL; myLast = NULL;}
		SigColl () {this->collType = Signature::SSTRUCT; myList = NULL; myLast = NULL; } /* STRUCT is the default sigcoll (now collections are handled through collectionKind attr.!*/
		SigColl (int _collType, Signature *_myList) : myList(_myList), collType(_collType) {myLast = _myList; };
		virtual bool isColl () {return true;}
		//Mthd below deprecated. Wrong approach to sig. collkind. Now collkind is an attribute of every signature.
		virtual string textCollKind() {if (this->collType == Signature::SSTRUCT) return ""; return this->textType();}
		virtual bool isEmpty() {return (myList == NULL);}
		virtual int type() {return this->collType;}
		virtual void setCollType(int type) {this->collType = type;}
		virtual void setCollTypeByString(string sType) {
			if (sType == "struct") this->collType = Signature::SSTRUCT;
			if (sType == "variant") this->collType = Signature::SVAR;
			//deprecated...
			if (sType == "list") this->collType = Signature::SSEQ;		// deprecated - use collectionKind attr
			if (sType == "bag") this->collType = Signature::SBAG;			// deprecated - use collectionKind attr
		}
		virtual Signature *getMyLast () {return myLast;}		
		virtual void setMyLast (Signature *nl) {if (nl->getNext() == NULL) myLast = nl; else setMyLast (nl->getNext());}
		virtual void setElts (Signature *newList) {this->myList = newList; this->myLast = newList;}
		virtual Signature *getMyList () {return this->myList;}
	
		virtual void addToMyList(Signature *newElt) {
		
			if (this->isEmpty()) {this->setElts(newElt);}
			else {
				if (newElt != NULL) {
					this->myLast->setNext(newElt); 
					newElt->setNext (NULL); 
					myLast = newElt;
				}
			}
		}
		
		virtual bool isStructurallyEqualTo(Signature *sig) {
			if (sig == NULL) return false;
			if (this->type() != sig->type()) return false;
			SigColl *s = (SigColl *) sig;
			bool nextEqual = (next == NULL ? (s->getNext() == NULL) : next->isStructurallyEqualTo(s->getNext()));
			if (not nextEqual) return false;
			//recur. check subSignatures...
			return (myList == NULL ? (s->getMyList() == NULL) : myList->isStructurallyEqualTo(s->getMyList()));
		}
		
		virtual bool isSolelyEqualTo(Signature *sig) {
			if (sig == NULL) return false;
			if (this->type() != sig->type()) return false; //variant is not the same as struct.
			SigColl *s = (SigColl *)sig;
			Signature *ptr = myList;
			Signature *sptr = s->getMyList();
			if (ptr == NULL) return (sptr == NULL);
			bool allSubsEqual = true;
			//naive comparison - requires proper order to match
			while (ptr != NULL && allSubsEqual) {
				allSubsEqual = ptr->isSolelyEqualTo(sptr);
				ptr = ptr->getNext();
				sptr = sptr->getNext();
			}
			return allSubsEqual;
		}
		virtual int compareNamedSigCrt(Signature *flatSig, bool needTName);
		virtual int compareRecNamedSigCrt(Signature *flatSig, bool needTName, vector<pair<int, int> > &vPairs);
		
		virtual BinderWrap *statNested(TreeNode * treeNode);
		virtual Signature *clone();
		virtual Signature *cloneSole();
		virtual Signature *cloneFlat();
		virtual int addFlat(Signature *son);
		virtual void putToString() {
			cout << "("<< textType() <<"," << distinctTypeName << "[" << card << "]" << collectionKind << "{";
			if (this->myList != NULL) myList->putToString();
			cout << "}) ";
			if (next != NULL) next->putToString(); 
		}
		virtual string toString() {
			string ret =  textType() + "," + attrsToString() + "{";
			//if (this->myList != NULL) ret += myList->toString();
			Signature *ptr = myList;
			while (ptr != NULL) {
				ret += ptr->toString();
				ptr = ptr->getNext();
			}
			ret += "} ";
			//if (next != NULL) ret += next->toString();
			return ret;
		}
		virtual Signature *deref() {
			SigColl *retSig = new SigColl(collType);
			Signature *ptr = myList;
			while (ptr != NULL) {
				retSig->addToMyList(ptr->deref());
				ptr = ptr->getNext();
			}
			return retSig;
		}
		virtual ~SigColl() {
			if (myList != NULL) delete myList;
			if (next != NULL) delete next;
		}
		/*virtual ~SigColl() {
			//cout << "entered sigcoll destructor" << endl;
			//if (next == NULL) { cout << "next is null " << endl;}
			//else {cout << "next AINT NULL!" << endl; next->putToString();}
			if (myList != NULL) {
				//cout << "will delete sth from myList" << endl;
				delete this->myList; 
				//cout << "deleted myList";
			} 
// 			else {
// 				cout << "myList is null" << endl;
// 			}
			if (next != NULL) {
				//cout << "well, we know 'next' is not null, so lets delete it. ";
				delete next;
			}
		}*/	
	};
	
	class SigAtomType : public Signature 
	{
		protected:
			string atomType;	/*"int" lub "double" lub "string" lub "bool"*/		
		public:
			SigAtomType() {}
			SigAtomType(string _atomType) : atomType(_atomType) {}
			virtual bool isAtom () {return true;}
			virtual int getNumb (string atype) {
				if (atype == "int") return Signature::SINT;
				if (atype == "double") return Signature::SDOUBLE;
				if (atype == "string") return Signature::SSTRING;
				if (atype == "boolean") return Signature::SBOOL;
				return -1; /*atype must be 1 of the above*/
			}
			virtual int type() {return this->getNumb(atomType);}
			virtual void setType(string newType) {this->atomType = newType;}
			virtual Signature *clone();
			virtual Signature *cloneSole();
			virtual Signature *cloneFlat();
			virtual bool isStructurallyEqualTo(Signature *sig) {
				if (sig == NULL) return false;
				if (this->type() != sig->type()) return false;
				return (next == NULL ? (sig->getNext() == NULL) : next->isStructurallyEqualTo(sig->getNext()));
			}
			virtual bool isSolelyEqualTo(Signature *sig) {
				if (sig == NULL) return false;
				return (this->type() == sig->type());
			}
			virtual int compareNamedSigCrt(Signature *flatSig, bool needTName);
			virtual void putToString() {
				cout << "(" << this->atomType << "," << distinctTypeName << "[" << card << "]" << collectionKind << ") ";
				if (next != NULL) next->putToString();
			}		
			virtual string toString() {
				string ret =  "(" + atomType + "," + attrsToString() + ") ";
				//if (next != NULL) ret += next->toString();
				return ret;
			}
			virtual Signature *deref() {
				return this->cloneSole();
			}
			virtual ~SigAtomType() {if (next != NULL) delete next; }
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
			virtual BinderWrap *statNested(TreeNode *treeNode);/*implmnted in Stack.cpp*/
			virtual Signature *clone();
			virtual Signature *cloneSole();
			virtual Signature *cloneFlat();
			virtual bool isStructurallyEqualTo(Signature *sig) {
				if (sig == NULL) return false;
				if (this->type() != sig->type()) return false;
				if (this->getRefNo() != ((SigRef *) sig)->getRefNo()) {
					vector<pair<int, int> > vPairs;	//vector of pairs visited.
					cout << "[TC] SigRef::StructuralyEqual(): entering compare Rec in Sig ref..." << endl;
					return this->compareRecNamedSigCrt(sig, false, vPairs);
					//return false;
				}
				return (next == NULL ? (sig->getNext() == NULL) : next->isStructurallyEqualTo(sig->getNext()));
			}
			virtual bool isSolelyEqualTo(Signature *sig) {
				if (sig == NULL) return false;
				if (this->type() != sig->type()) {
					vector<pair<int, int> > vPairs;	//vector of pairs visited.
					cout << "[TC] SigRef::SolelyEqual(): entering compare Rec in Sig ref..." << endl;
					return this->compareRecNamedSigCrt(sig, false, vPairs);
					//return false;
				}
				return (this->getRefNo() == ((SigRef *) sig)->getRefNo());
			}
			virtual int compareNamedSigCrt(Signature *flatSig, bool needTName);
			virtual int compareRecNamedSigCrt(Signature *flatSig, bool needTName, vector<pair<int, int> > &vPairs);
			virtual void putToString() {
				cout << "(ref->" << refNo << "," << distinctTypeName << "[" << card << "]" << collectionKind << ") ";
				//if (next != NULL) next->putToString();
			}
			virtual Signature *deref();
			
			virtual string toString() {
				stringstream ss;
				ss << refNo;
				string ret = "(ref->" + ss.str() + "," + attrsToString() + ") ";
				//if (next != NULL) ret += next->toString();
				return ret;
			}
			virtual ~SigRef() { if (next != NULL) delete next; }
		
	};
	
	class SigVoid : public Signature
	{	
		public:
			SigVoid(){}
			virtual Signature *clone();
			virtual Signature *cloneSole();
			virtual Signature *cloneFlat();
			virtual int type() {return Signature::SNOTHING;}
			virtual int compareNamedSigCrt(Signature *flatSig, bool /*needTName*/) {
				return (this->type() == flatSig->type() ? 0 : (Errors::ErrTypeChecker | Errors::ESigTypesDiffer));}
			virtual void putToString() {cout << "(void)";}
			virtual string toString() {
				string ret = "(void," + attrsToString() + ") ";
				//if (next != NULL) ret += next->toString();
				return ret;
			}
			virtual ~SigVoid() {}
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
			virtual bool isBinder() {return true;}
			virtual BinderWrap *statNested(TreeNode *treeNode);
			virtual Signature *clone();
			virtual Signature *cloneSole();
			virtual Signature *cloneFlat();
			virtual bool isStructurallyEqualTo(Signature *sig) {
				if (sig == NULL) return false;
				bool valEqual;
				if (this->type() != sig->type()) { //sig is not a binder
					valEqual = (value != NULL && value->isStructurallyEqualTo(sig));
				}//return false;
				else { //sig is a binder
				//No need for names to match - just the pointed signature...
					StatBinder *s = (StatBinder *) sig;
					valEqual = (value == NULL ? s->getValue() == NULL : value->isStructurallyEqualTo(s->getValue()));
				}
				if (not valEqual) return false;
				return (next == NULL ? (sig->getNext() == NULL) : next->isStructurallyEqualTo(sig->getNext()));
			}
			virtual bool isSolelyEqualTo(Signature *sig) {
				if (sig == NULL) return false;
				if (this->type() != sig->type()) 
					return (value != NULL && value->isStructurallyEqualTo(sig));
				StatBinder *s = (StatBinder*) sig;
				return (value == NULL ? s->getValue() == NULL : value->isSolelyEqualTo(s->getValue()));
			}
			virtual int compareNamedSigCrt(Signature *flatSig, bool needTName);
			virtual int compareRecNamedSigCrt(Signature *flatSig, bool needTName, vector<pair<int, int> > &vPairs);
			virtual void putToString() {
				cout << "(" << name <<"(";
				if (value == NULL) cout << "__";
				else value->putToString();
				cout << ")) ";
				if (next != NULL) next->putToString();
			}
			virtual string toString() {
				string ret = name + "," + attrsToString() + "(" + (value == NULL ? "__" : value->toString()) + ") ";
				//if (next != NULL) ret += next->toString();
				return ret;
			}
			virtual Signature *deref() {
				if (value == NULL) return new StatBinder(name);
				else return new StatBinder(name, value->deref());
			}
			
			virtual ~StatBinder() {cout << "entered statBinder destr" << endl; if (value != NULL) {Deb::ug( "value to kil\n");delete value; }
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
			virtual void bindName2(string aName, vector<BinderWrap*> *vec) = 0; 
			virtual void bindNameEllipsis(string aName, vector<BinderWrap*> *vec, string &expander) = 0; 
			virtual void putToString() {cout << "bw-name(sign)" << endl;};
			virtual int size() = 0;	//returns number of binders in this section
			virtual BinderWrap* getNext() = 0; // returns next elem, sth like iterator
										// in list impl its just next
	};	
	
	class BinderList : public BinderWrap
	{
		protected:
			BinderList *next;
		public:
			virtual BinderWrap * getNext(){return next;}
			virtual int size(){
				BinderList* pom = this;
				int ile = 0;
				do {
					if (pom->getBinder()!=NULL) 
						ile++;
					pom = pom->next;
				} while(pom != NULL);
				return ile;
			}
			BinderList(StatBinder *_binder) {this->setBinder(_binder); this->next = NULL;}
			BinderList() {binder = NULL; next = NULL;}
			virtual void setNext(BinderList *newNext) {this->next = newNext;}
			virtual void setSectNumb (int newNr) {
	//		cout << "jsi setSectNumb start\n";
	//		cout << "binder name: " << this->getBinder()->getName() << ".\n";
				this->sectNumb = newNr; 
	//		cout << "set nr\n";
				if (this->next != NULL) next->setSectNumb(newNr);
	//		cout << "served next, ending\n";
			}
			virtual BinderWrap *addOne (BinderWrap *newBW) {
				((BinderList *)newBW)->setNext(this); 
				newBW->setSectNumb(this->sectNumb);
				if (newBW == NULL) cout << "\n\n\n\n jsi ERROR ------------------------------ addOne zwraca NULL !!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n";
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
		
			virtual void bindName2(string aName, vector<BinderWrap*> *vec){
				if (this->binder->getName() == aName) vec->push_back(this);
				if (this->next == NULL) return;
				else this->next->bindName2(aName, vec);
			}
			virtual void bindNameEllipsis(string name, vector<BinderWrap*> *vec, string &expander) {
				cout << "ELL-binderList" << endl;
				BinderWrap *bw = this->binder->getValue()->statNested(NULL);
				if (bw != NULL) bw->bindName2(name, vec);
				if (vec->size() > 0) {
					expander = this->binder->getName();
				} else {
			//if (bw != NULL && bw->getBinder()->getName() == name) vec->push_back(bw);
					if (this->next == NULL) return;
					else this->next->bindNameEllipsis(name, vec, expander);
				}
			}
		
			virtual void putToString() {
				if (this->binder == NULL) cout << "_noBinder_";
				else {cout << "bwl-[s:" << this->sectNumb << ","; 
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
				//			delete(this);	/* TODO: czy tak mo�na ... ??? */
				return sectPoint;
			}
			virtual QStackElem *top () {
				return this;
			}
			virtual void putToString() {
				if (this->getContent() == NULL) cout << "# LISTSection #"; 
				else { cout << "lqse#"<< myNumber << ": "; getContent()->putToString(); cout << " #";}
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
	
		// bez tego nie mozna bylo wywolac tej metody zdefiniowanej w nadklasie - bo tam tez byl zdef atr content
		// i zwracal ten z nadklasy a nie ten z tej klasy !!!
				virtual ElemContent *getContent(){return this->content;};
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
				virtual void bindName2(string aName, vector<BinderWrap*> *vec){
					this->content->bindName2(aName, vec);
					if (vec->size() == 0) {
						if (this->next == NULL) return ;
						else  ((StatEnvSection *)this->next)->bindName2(aName, vec);
					}	
				}
		
				virtual void bindNameEllipsis(string aName, vector<BinderWrap*> *vec, string &expander) {
					if (this->next == NULL) return; //DO NOT CHECK BASE SECTION FOR ELLIPSIS
					this->content->bindNameEllipsis(aName, vec, expander);
					if (vec->size() == 0) {
						((StatEnvSection *)this->next)->bindNameEllipsis(aName, vec, expander);
					}
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
		
			virtual void bindName2 (string aName, vector<BinderWrap*> *vec) {
				if (this->isEmpty()) return;
				else ((StatEnvSection *)this->top())->bindName2(aName, vec);
			}
		
			virtual void bindNameEllipsis(string name, vector<BinderWrap*> *vec, string &expander) {
				Deb::ug("Stack::bindNameEllipsis()");
				if (this->isEmpty()) return;
				((StatEnvSection *) this->top())->bindNameEllipsis(name, vec, expander);
			}

			virtual BinderWrap *bindNameHere (string aName, int sectNo) {
				QStackElem *s = this->getElt(sectNo);
				Deb::ug("znalazlem getElt");
				if (s == NULL) { Deb::ug("bndNameHere-zwracam NULL"); return NULL;}
				else return ((StatEnvSection *) s)->bindName(aName);
				return NULL;
			}
			virtual int pushBinders (BinderWrap *bw) {/* a collection of binders, not packed in a section yet... */
				if (bw == NULL) {Deb::ug("NULL binders not pushed on envs\n"); return -1;};
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
			StatQResElt() {this->next = NULL; this->content = NULL;};
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
				if (Deb::ugOn()) {cout << "pushed " << newSig->toString() << " on sQres.\n";}
				return this->push(new StatQResElt (newSig));
			} 

		
			virtual Signature *topSig() {
				if (this->isEmpty()) {
					return NULL;
				} 
				return ((Signature *)((StatQResElt *)elts)->getContent())->clone();
			}		

			virtual Signature *popSig() {
				Signature *result = topSig();
				if (result != NULL)  {
					this->pop();
				}
				return result;
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


