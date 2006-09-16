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
	cout <<"=============================================================================" << endl;
	cout <<"-----------------------------------------------------------------------------" << endl;
	cout <<"-----------------------------------------------------------------------------" << endl;
	cout << "start rmvDeath\n";
	this->qTree = qTree;
	cout<< "rmvDeath start" << endl;
	cout << "dostal :" << endl;
	qTree->putToString();
	cout << endl;
	while(true){
//		Deb::ug("\n rmvDeath loop start -------------------------------------------------------\n");
		qParser->statEvaluate(qTree);
		qTree->markNeeded();		
		TreeNode * death = qTree->getDeath();
		if (death == NULL) break;
		cout << "\n rem \n";
		death->putToString();
		cout << "\n from \n";
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
	cout<< "rmvDeath end" << endl;
	cout << "zwraca : \n" << endl;
	qTree->putToString();
	cout << endl;
	return 0;	
}

}
