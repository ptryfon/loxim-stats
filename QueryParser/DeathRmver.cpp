#include "DeathRmver.h"


namespace QParser{

DeathRmver::DeathRmver(QueryParser *qParser)
{
	this->qParser = qParser;
}

DeathRmver::~DeathRmver()
{
}

int DeathRmver::rmvDeath(TreeNode *&qTree){
	cout << "rmvDeath start\n";
	cout <<"=============================================================================" << endl;
	cout <<"-----------------------------------------------------------------------------" << endl;
	cout <<"-----------------------------------------------------------------------------" << endl;
	
	this->qTree = qTree;
	cout << "dostal :" << endl;
	qTree->serialize();
//	qTree->putToString();
	cout << endl;
	while(true){
//		Deb::ug("\n rmvDeath loop start -------------------------------------------------------\n");
		qParser->statEvaluate(qTree);
		qTree->markNeeded();		
		if (Deb::ugOn()){
		    cout << "\n szuka w \n";
		    qTree->putToString();
		    cout << endl;
		}
		TreeNode * death = qTree->getDeath();
		if (death == NULL) break;
		if (Deb::ugOn()){
		    cout << "\n rem \n";
		    death->putToString();
		    cout << "\n from \n";
		}
		death->getParent()->putToString();
		TwoArgsNode * parent = (TwoArgsNode*) death->getParent();
		TreeNode * live = (parent->getLArg() == death)?parent->getRArg():parent->getLArg();
		if (parent == qTree){
			qTree = live;
			qTree->setParent(NULL);
		} else {
			parent->getParent()->swapSon(parent, live);	
		}
//		Deb::ug("\nrmvDeath loop end -------------------------------------------------------\n");
	}		
	cout <<"=============================================================================" << endl;
	cout <<"-----------------------------------------------------------------------------" << endl;
	cout <<"-----------------------------------------------------------------------------" << endl;
	cout<< "rmvDeath end, zwraca" << endl;
//	qTree->putToString();
	qTree->serialize();
	cout << endl;
	return 0;	
}

}
