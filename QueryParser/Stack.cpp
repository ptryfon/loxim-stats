#include <iostream>
#include <string>
#include <sstream>

#include "Stack.h"
#include "QueryParser.h"
#include "TreeNode.h"
#include "DataRead.h"
#include "Deb.h"

using namespace std;





namespace QParser {

/* testy dla stosow. ... */
//StatEnvStack *sQres;
//StatQResStack *sEnvs;


/*stworz stos ENVS i wsadz jako elementy bazowe kilka binderow...*/
	
	Signature* Signature::clone(){return NULL;}
	Signature* SigColl::clone(){
		SigColl *ns = new SigColl(type()); 
		if (myList != NULL) {ns->setElts (myList->clone()); ns->setMyLast (this->myLast);}
		
		if (next != NULL) ns->setNext(next->clone());
		return ns;}
	Signature* SigAtomType::clone(){
		SigAtomType *ns = new SigAtomType(atomType);
		ns->setDependsOn(this->getDependsOn());			// death
		if (next != NULL) ns->setNext(next->clone());
		return ns;}
	Signature* SigRef::clone(){
		SigRef *ns = new SigRef(refNo);
		ns->setDependsOn(this->getDependsOn());			// death
		if (next != NULL) ns->setNext(next->clone());
		return ns;}
	Signature* StatBinder::clone(){
		StatBinder *ns = new StatBinder(name);
		ns->setDependsOn(this->getDependsOn());			// death
		if (value != NULL) ns->setValue (value->clone());
		if (next != NULL) ns->setNext (next->clone());
		return ns;
		}
	BinderWrap* SigRef::statNested(TreeNode *treeNode) {
		Deb::ug("doing statNested on DATA SCHEME ! "); 
		cout << "treeNode->getName(): " + treeNode->getName() << endl;
		return DataScheme::dScheme()->statNested(refNo, treeNode);
		}		
	BinderWrap* StatBinder::statNested(TreeNode *treeNode) { 
		if (this->dependsOn != treeNode){
			Deb::ug("jsi ERROR zobacz StatBinder::statNested\n");
		}
		return new BinderList((StatBinder *) this);
	};

	BinderWrap* SigColl::statNested(TreeNode *treeNode) {
		Deb::ug("stat nested na sigColl start\n");
		Deb::ug( "statNested::sigColl -- should not be evoked...");	
		Signature *pt = this->getMyList();
		BinderList *bindersCol;
			while (pt != NULL) { 
				BinderWrap *binders = pt->statNested(pt->getDependsOn());
		//		BinderWrap *binders = pt->statNested(treeNode);
				if (bindersCol == NULL) bindersCol = (BinderWrap*) binders;
				else {
					BinderList *one = bindersCol;
					while(one!=NULL){
						bindersCol = (BinderList *) (bindersCol->addOne(one));
						one = one->next;
					}
				}
				pt = pt->getNext();
			}
		
		return bindersCol;
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
        	sEnvs->pushBinders(DataScheme::dScheme()->bindBaseObjects());	
		sEnvs->putToString();
		
		/* modify the tree setting special info in nameNodes and NonAlgOpNodes */
		cout << "no, mam envs gotowy ze startowymi obiektami w pierwszej sekcji .. " << endl;
		int res = tn->staticEval (sQres, sEnvs);
		tn->putToString();
	
		return 0;	
	}
		

	


}


