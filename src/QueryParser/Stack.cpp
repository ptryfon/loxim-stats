#include <iostream>
#include <string>
#include <sstream>

#include <QueryParser/Stack.h>
#include <QueryParser/QueryParser.h>
#include <QueryParser/TreeNode.h>
#include <QueryParser/DataRead.h>
#include <QueryParser/Deb.h>

using namespace std;
//using namespace TypeCheck;

namespace QParser {

/* testy dla stosow. ... */
//StatEnvStack *sQres;
//StatQResStack *sEnvs;


/*stworz stos ENVS i wsadz jako elementy bazowe kilka binderow...*/
	//initializes all attributes.
	void Signature::init() {
		this->next = NULL; this->dependsOn = NULL;
		distinctTypeName = ""; collectionKind = "";
		card = "1..1"; refed = false;
	}



	Signature* Signature::clone(){return NULL;}
	Signature* SigColl::clone(){
		SigColl *ns = new SigColl(type());
		if (myList != NULL) {
			ns->setElts (myList->clone());
		    /*	tak bylo kiedys, tak jest zle,
			ns->setMyLast (this->myLast);
			*/
			ns->setMyLast (ns->getMyList()); //recurrently goes through myList till it reaches the end..
			//Signature *ptr = ns->getMyList();
// 			while (ptr != NULL) {
// 				ns->setMyLast(ptr);
// 				ptr = ptr->getNext();
// 			}
		}
		if (next != NULL)
			ns->setNext(next->clone());
		ns->copyAttrsOf(this);
// 		ns->setCard(this->card);
// 		ns->setTypeName(this->distinctTypeName);
		return ns;
	}
	Signature* SigAtomType::clone(){
		SigAtomType *ns = new SigAtomType(atomType);
		ns->setDependsOn(this->getDependsOn());			// death
		if (next != NULL) ns->setNext(next->clone());
		ns->copyAttrsOf(this);
		//ns->setCard(this->card);
		//ns->setTypeName(this->distinctTypeName);
		return ns;
	}
	Signature* SigRef::clone(){
		SigRef *ns = new SigRef(refNo);
		ns->setDependsOn(this->getDependsOn());			// death
		if (next != NULL) ns->setNext(next->clone());
		ns->copyAttrsOf(this);
		//ns->setCard(this->card);
		//ns->setTypeName(this->distinctTypeName);
		return ns;
	}
	Signature* StatBinder::clone(){
		StatBinder *ns = new StatBinder(name);
		ns->setDependsOn(this->getDependsOn());			// death
		if (value != NULL) ns->setValue (value->clone());
		if (next != NULL) ns->setNext (next->clone());
		ns->copyAttrsOf(this);
		//ns->setCard(this->card);
		//ns->setTypeName(this->distinctTypeName);
		return ns;
	}

	Signature *Signature::cloneSole() {return NULL;}
	Signature *Signature::cloneFlat() {return NULL;}
	Signature *SigColl::cloneSole() {
		SigColl *ns = new SigColl(type());
		if (myList != NULL) {
			ns->setElts (myList->clone()); //use normal clone here, to handle the nexts automatically...
			ns->setMyLast (ns->getMyList());
		}
		ns->copyAttrsOf(this);
		return ns;
	}

	Signature *SigColl::cloneFlat() {
		SigColl *ns = new SigColl(type());
		Signature *ptr = this->getMyList();
		while (ptr != NULL) {
			if (ns->addFlat(ptr) != 0) return NULL;
			ptr = ptr->getNext();
		}
		ns->copyAttrsOf(this);
		return ns;
	}

	int SigColl::addFlat(Signature *son) {
		if (son->type() != Signature::SBINDER && son->type() != this->type()) return -1;
		if (son->type() == Signature::SBINDER) {
			this->addToMyList(son->cloneFlat());
		} else { //son is a SigColl
			Signature *ptr = ((SigColl *)son)->getMyList();
			while (ptr != NULL) {
				if (this->addFlat(ptr) != 0) return -1;
				ptr = ptr->getNext();
			}
		}
		return 0;
	}

	Signature* SigAtomType::cloneSole(){
		SigAtomType *ns = new SigAtomType(atomType);
		ns->setDependsOn(this->getDependsOn());			// death
		ns->copyAttrsOf(this);
		return ns;
	}

	Signature* SigRef::cloneSole(){
		SigRef *ns = new SigRef(refNo);
		ns->setDependsOn(this->getDependsOn());			// death
		ns->copyAttrsOf(this);
		return ns;
	}
	Signature* SigAtomType::cloneFlat() { return cloneSole();}
	Signature* SigRef::cloneFlat() { return cloneSole();}
	Signature *SigVoid::cloneFlat() {return cloneSole();}

	Signature* StatBinder::cloneSole(){
		StatBinder *ns = new StatBinder(name);
		ns->setDependsOn(this->getDependsOn());			// death
		if (value != NULL) ns->setValue (value->clone());
		ns->copyAttrsOf(this);
		return ns;
	}
	Signature* StatBinder::cloneFlat(){
		StatBinder *ns = new StatBinder(name);
		ns->setDependsOn(this->getDependsOn());			// death
		if (value != NULL) ns->setValue (value->cloneFlat());
		ns->copyAttrsOf(this);
		return ns;
	}
	Signature *SigVoid::clone() {
		SigVoid *ns = new SigVoid();
		ns->setDependsOn(this->getDependsOn());
		if (next != NULL) ns->setNext(next->clone());
		ns->copyAttrsOf(this);
		return ns;
	}
	Signature *SigVoid::cloneSole() {
		SigVoid *ns = new SigVoid();
		ns->setDependsOn(this->getDependsOn());
		ns->copyAttrsOf(this);
		return ns;
	}
	int Signature::compareRecNamedSigCrt(Signature *flatSig, bool needTName, vector<pair<int, int> > &vPairs) {
		return compareNamedSigCrt(flatSig, needTName);
	}

	int SigAtomType::compareNamedSigCrt(Signature *flatSig, bool needTName) {
		if (flatSig == NULL || type() != flatSig->type())
			return (ErrTypeChecker | ESigTypesDiffer);
		if (needTName && (this->getTypeName() != flatSig->getTypeName()))
			return (ErrTypeChecker | ESigTNamesDiffer);
		return 0;
	}

	int StatBinder::compareNamedSigCrt(Signature *flatSig, bool needTName) {
		if (flatSig == NULL || type() != flatSig->type())
			return (ErrTypeChecker | ESigTypesDiffer);
		if (needTName && (this->getTypeName() != flatSig->getTypeName()))
			return (ErrTypeChecker | ESigTNamesDiffer);
		if (name != ((StatBinder *) flatSig)->getName())	//names in binders must match...
			return (ErrTypeChecker | ESigTypesDiffer);
		if (value == NULL) return (((StatBinder *)flatSig)->getValue() == NULL ? 0 : (ErrTypeChecker | ESigTypesDiffer));
		//typename equality is actually enough - it implies structural equality...
		//would have to take typename of binder or of binder->getValue().. probably of value- 'name as' kills typename.
		//if (this->getTypeName() != "" && this->getTypeName() == flatSig->getTypeName()) return 0;
		return value->compareNamedSigCrt(((StatBinder*) flatSig)->getValue(), needTName);
	}

	int StatBinder::compareRecNamedSigCrt(Signature *flatSig, bool needTName, vector<pair<int, int> > &vPairs) {
		if (flatSig == NULL || type() != flatSig->type())
			return (ErrTypeChecker | ESigTypesDiffer);
		if (needTName && (this->getTypeName() != flatSig->getTypeName()))
			return (ErrTypeChecker | ESigTNamesDiffer);
		if (name != ((StatBinder *) flatSig)->getName())	//names in binders must match...
			return (ErrTypeChecker | ESigTypesDiffer);
		if (value == NULL) return (((StatBinder *)flatSig)->getValue() == NULL ? 0 : (ErrTypeChecker | ESigTypesDiffer));
		return value->compareRecNamedSigCrt(((StatBinder*) flatSig)->getValue(), needTName, vPairs);
	}

	int SigRef::compareNamedSigCrt(Signature *flatSig, bool needTName) {
		if (flatSig == NULL || type() != flatSig->type())
			return (ErrTypeChecker | ESigTypesDiffer);
		if (needTName && (this->getTypeName() != flatSig->getTypeName()))
			return (ErrTypeChecker | ESigTNamesDiffer);
		int myRef = this->getRefNo();
		int sigRef = ((SigRef *) flatSig)->getRefNo();

		if (myRef != sigRef) {
			//Maybe these (or 1 of them) are refs to defined types.
			//in such case - the refNo's don't have to be equal - its what they point to that must match.
			//but this could lead to an endless loop. So - do a recursive comparison making sure you don't loop.
			vector<pair<int, int> > vPairs;	//vector of pairs visited.
			cout << "[TC] entering compare Rec in Sig ref..." << endl;
			return this->compareRecNamedSigCrt(flatSig, needTName, vPairs);
		}
		return 0;
	}

	int SigRef::compareRecNamedSigCrt(Signature *flatSig, bool needTName, vector<pair<int, int> > &vPairs) {
		if (flatSig == NULL || type() != flatSig->type())
			return (ErrTypeChecker | ESigTypesDiffer);
		if (needTName && (this->getTypeName() != flatSig->getTypeName()))
			return (ErrTypeChecker | ESigTNamesDiffer);
		int myRef = this->getRefNo();
		int sigRef = ((SigRef *) flatSig)->getRefNo();
		cout << "[TC] IN compare Rec for Sig refs: " << myRef << ", " << sigRef << endl;
		if (myRef != sigRef) {
			//if none is a typedef - return error - refs to objects must match.
			if ((not DataScheme::dScheme(-1)->isTypeDefId(myRef)) && (not DataScheme::dScheme(-1)->isTypeDefId(sigRef))) {
				return (ErrTypeChecker | ESigTypesDiffer);
			}
			//if at least one is a typedef - do safe deref and compare again.
			pair<int, int> key(myRef, sigRef);
			for (unsigned int i = 0; i < vPairs.size(); i++) {
				if (vPairs.at(i) == key) {
					cout << "[TC] Comparing sigRefs has lead to a loop\n";
					//this is not obvious. just found two simple loops - do we consider them the same? if so - return 0.
					//return (ErrTypeChecker | ESigTypesDiffer);
					return 0;
				}
			}
			vPairs.push_back(key);
			cout << "[TC] diggind deeper into compare Rec in Sig ref..." << endl;
			StatBinder *myVal = (StatBinder *) DataScheme::dScheme(-1)->namedSignatureOfRef(myRef);
			StatBinder *sigVal = (StatBinder *) DataScheme::dScheme(-1)->namedSignatureOfRef(sigRef);
			if (myVal == NULL || sigVal == NULL || myVal->getValue() == NULL) {
				return (ErrTypeChecker | ESigTypesDiffer);
			}
			cout << "[TC] derefed'em to: " << myVal->toString() << "  &  " << sigVal->toString() << endl;
			return myVal->getValue()->compareRecNamedSigCrt(sigVal->getValue(), needTName, vPairs);
		}
		return 0;
	}

	int SigColl::compareNamedSigCrt(Signature *flatSig, bool needTName) {
		vector<pair<int, int> > vPairs;
		return compareSigCrt(flatSig, needTName, false, vPairs);
	}

	int SigColl::compareRecNamedSigCrt(Signature *flatSig, bool needTName, vector<pair<int, int> > &vPairs) {
		return compareSigCrt(flatSig, needTName, true, vPairs);
	}

	int SigColl::compareSigCrt(Signature *flatSig, bool needTName, bool isRec, vector<pair<int, int> > &vPairs) {
		//we know all subs of this and of flatSig are binders. else this method would not have been called.
		if (flatSig == NULL || type() != flatSig->type())
			return (ErrTypeChecker | ESigTypesDiffer);
		if (needTName && (this->getTypeName() != flatSig->getTypeName()))
			return (ErrTypeChecker | ESigTNamesDiffer);
		SigColl *s = (SigColl *)flatSig;
		//now comparing complex structural equality... !
		//map: for each subObj. name: keep lowest and highest cardinality of all adequate subObjcts from flatSig.
		map<string, std::pair<int, int> > subMap;
		Signature *sptr = s->getMyList();
		int result = 0;
		//for each elt of guest signature - check we have a matching node.
		while (sptr != NULL) {
			string sName = ((StatBinder *) sptr)->getName();
			Signature *mptr = this->getMyList();
			bool foundName = false;
			Signature *match = NULL;
			while (mptr != NULL && not foundName) {
				if (((StatBinder *) mptr)->getName() == sName) {
					foundName = true;
					match = mptr;
				}
				mptr = mptr->getNext();
			}
			if (not foundName || match == NULL) return (ErrTypeChecker | ESigTypesDiffer);
			int partResult = 0;
			if (isRec) {
				partResult = match->compareRecNamedSigCrt(sptr, needTName, vPairs);
			} else {
				partResult = match->compareNamedSigCrt(sptr, needTName);
			}
			if (partResult != 0 && partResult != (ErrTypeChecker | ESigCdDynamic))
				return partResult;
			//make sure dynamic coertion is not lost.
			if (partResult != 0) result = partResult;
			//now match is ok - equal to sptr, so report it in the subMap;
			std::pair<int, int> cdVal = cardToMapPair(sptr->getCard());
			if (subMap.find(sName) == subMap.end()) {
				subMap[sName] = cdVal;
				cout << "just put in map: "<<sName<<", ("<<subMap[sName].first<<".."<<subMap[sName].second<<")\n";
			} else {
				subMap[sName].first += cdVal.first;
				subMap[sName].second += cdVal.second;
				cout <<"just ADDED to map:"<<sName<<", ("<<subMap[sName].first<<".."<<subMap[sName].second<<")\n";
			}
			sptr = sptr->getNext();
		}
		//now -knowing the subMap - check for: subOverflows, subsMissing, cdDynamicChecks.
		Signature *ptr = myList;
		while (ptr != NULL) {
			string name = ((StatBinder *)ptr)->getName();
			string card = ptr->getCard();
			//check if its missing: not found in subMap  -> error
			if (subMap.find(name) == subMap.end()) {
				if (card[0] == '1')	return (ErrTypeChecker | ESigMissedSubs);
				subMap[name] = std::pair<int,int>(0,0);
			}
			//check for overflow: if rightBound of card '<' subMap[name].first -> error
			std::pair<int, int> crt = subMap[name];
			if (card[3]!= '*' && (card[3]-'0') < crt.first) return (ErrTypeChecker | ESigCdOverflow);
			//check for dynamic card checks
			if (crt.first < (card[0]-'0') || (card[3]!= '*' && (card[3]-'0') < crt.second))
				result = (ErrTypeChecker | ESigCdDynamic);
			ptr = ptr->getNext();
		}
		return result;
	}

	std::pair<int, int> Signature::cardToMapPair(string card) {
		int lb = (card[0]-'0');
		int rb = (card[3] == '*' ? 10 : (card[3] - '0'));
		return std::pair<int,int>(lb, rb);
	}

	Signature *SigRef::deref() {
		return DataScheme::dScheme(-1)->signatureOfRef(this->getRefNo());
	}

	BinderWrap* SigRef::statNested(TreeNode *treeNode) {
		Deb::ug("doing statNested on DATA SCHEME ! ");
		if (treeNode != NULL && Deb::ugOn()) cout << "treeNode->getName(): " + treeNode->getName() << endl;
		return DataScheme::dScheme(-1)->statNested(refNo, treeNode);
		}
	BinderWrap* StatBinder::statNested(TreeNode *treeNode) {
		if (this->dependsOn != treeNode){
			Deb::ug("jsi ERROR zobacz StatBinder::statNested\n");
		}
		return new BinderList((StatBinder *) this);
	};

	BinderWrap* SigColl::statNested(TreeNode *treeNode) {
		Deb::ug("stat nested na sigColl start----------------------------------------------------------\n");
		if (Deb::ugOn()){
		    cout << "zawartosc sigColl: \n";
		    this->putToString();
		    cout << endl;
		}
		Signature *pt = this->getMyList();
		BinderWrap *bindersCol = NULL;
		BinderWrap *resultBinders = NULL;
			while (pt != NULL) {
				Deb::ug("loop start\n");
				bindersCol = NULL;
				bindersCol = pt->statNested(pt->getDependsOn());
				BinderWrap *one;
				// uwaga, bindersCol moze byc NULL -
				// wtedy gdy jest to obiekt atomowy, lub
				// stat nested dostalo NULL jako argument(pusta kol)
				if (Deb::ugOn()){
				    one = bindersCol;
				    cout << "statNested zwrocilo : ";
				while(one!=NULL){
					    one->getBinder();
					    cout << one->getBinder()->getName() << " ";
					    one = one->getNext();
				}
				cout << endl;
				}
				if (resultBinders == NULL){
				     Deb::ug("jsi sn if start\n");
				     resultBinders = bindersCol;
				}
				else {
					Deb::ug("jsi sn else start\n");
					one = bindersCol;

					while(one!=NULL){
						BinderWrap *nextPom = one->getNext();
						//cout << "adding: " << one->getBinder()->getName() << endl;
						resultBinders =  resultBinders->addOne(one);
						one = nextPom;
						// tak byloby zle:
						//one =  one->getNext();
					}
					Deb::ug("jsi sn else end\n");
				}
				pt = pt->getNext();
			}
		Deb::ug("stat nested na sigColl END\n");
		return resultBinders;
		}

	void Optimiser::setQres(StatQResStack *nq) {this->sQres = nq;}
	void Optimiser::setEnvs(StatEnvStack *nq) {this->sEnvs = nq;}
	Optimiser::~Optimiser() {if (sQres != NULL) delete sQres;
				if (sEnvs != NULL) delete sEnvs;}


	int Optimiser::simpleTest() {
		setEnvs(new StatEnvStack());
		setQres(new StatQResStack());
		StatBinder *b1 = new StatBinder ("PRAC", new SigRef (221));
		StatBinder *b2 = new StatBinder ("DEPT", new SigRef (322));
		StatBinder *b3 = new StatBinder ("PRAC", new SigRef (423));
		StatBinder *b4 = new StatBinder ("ZONA", new SigRef (518));
		StatBinder *b5 = new StatBinder ("ZONA", new SigRef (1000));
		StatBinder *b6 = new StatBinder ("DRUGAZONA", new SigRef (518));

		StatEnvSection *sec1 = new StatEnvSection (new BinderList (b1));
		StatEnvSection *sec2 = new StatEnvSection (new BinderList (b3));
			sec2->addBinder(new BinderList(b4)); sec2->addBinder(new BinderList(b6));
		StatEnvSection *sec3 = new StatEnvSection (new BinderList (b2));
			sec3->addBinder(new BinderList (b5));
		this->sEnvs->push(sec1);
		this->sEnvs->push(sec2);
		this->sEnvs->push(sec3);

		cout << "stos: "; sEnvs->putToString();
/*		this->sEnvs->pop();
		cout << " zrobilem POPa !!!! "<< endl;
		sEnvs->putToString();
*/		cout << sEnvs->getElts()->getNumber()<<endl;
		cout <<"PRACz 1 sekcji: " << sEnvs->bindNameHere("PRAC", 1)->getSectNumb() << endl;
		return 0;
	}

	int Optimiser::stEvalTest(TreeNode *&tn) {
		setEnvs(new StatEnvStack());
		setQres(new StatQResStack());
		/* plus mamy globalna zmienna DataScheme *dScheme*/

		/* set the first section of the ENVS stack with binders to
		   definitions of base objects ... */
        	sEnvs->pushBinders(DataScheme::dScheme(-1)->bindBaseObjects());
		sEnvs->putToString();

		/* modify the tree setting special info in nameNodes and NonAlgOpNodes */
		cout << "no, mam envs gotowy ze startowymi obiektami w pierwszej sekcji .. " << endl;
		int res = tn->staticEval (sQres, sEnvs);

		tn->putToString();

		return res;
	}





}


