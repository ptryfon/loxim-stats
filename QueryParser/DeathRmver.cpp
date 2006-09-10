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
	qParser->statEvaluate(qTree);
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
