#include "DeathRmver.h"


namespace QParser{

DeathRmver::DeathRmver(QueryParser *qParser)
{
	cout << "DeathRmver constr start" << endl;
	this->qParser = qParser;
//	this->qTree = NULL;
}

DeathRmver::~DeathRmver()
{
}

int DeathRmver::rmvDeath(TreeNode *&qTree){
	this->qTree = qTree;
	cout<< "rmvDeath start" << endl;
	cout << "dostal :" << endl;
	qTree->putToString();
	while(true){
		qParser->statEvaluate(qTree);
		qTree->markNeeded();		
		TreeNode * death = qTree->getDeath();
		if (death == NULL) break;
		TwoArgsNode * parent = (TwoArgsNode*) death->getParent();
		TreeNode * live = (parent->getLArg() == death)?parent->getRArg():parent->getLArg();
		if (parent == qTree){
			qTree = live;
		} else {
			parent->getParent()->swapSon(parent, live);	
		}
	}		
	cout <<"=============================================================================" << endl;
	cout <<"-----------------------------------------------------------------------------" << endl;
	cout <<"-----------------------------------------------------------------------------" << endl;
	cout<< "rmvDeath end" << endl;
	cout << "zwraca :" << endl;
	qTree->putToString();
	return 0;	
}



/* to chyba w czasie analizy statycznej, metoda na kazdym wezle
// statically fill dependecies, everything apart from name nodes (dependecies for these nodes
// are already done during static analisation
void DeathRmver::fillDependencyGraph(){
	cout << "drmver.fillDependencyGraph start" << endl;

}
*/

}
