#include <QueryParser/AuxRmver.h>
#include <assert.h>

namespace QParser{

AuxRmver::AuxRmver(QueryParser *qParser)
{
	this->qParser = qParser;
}

AuxRmver::~AuxRmver()
{
}

int AuxRmver::rmvAux(TreeNode *&qTree){
	cout << "rmvAux start\n";
	cout <<"=============================================================================" << endl;
	cout <<"-----------------------------------------------------------------------------" << endl;
	cout <<"-----------------------------------------------------------------------------" << endl;

	this->qTree = qTree;
	cout << "dostal :" << endl;
	qTree->serialize();
	// qTree->putToString();
	cout << endl;
	bool removed = true;
	while(removed){
		removed = false;
		QueryParser::setStatEvalRun(0);
		TreeNode::resetUsedNeeded(qTree);
		qParser->statEvaluate(-1, qTree);
		if (Deb::ugOn()){
		    cout << "\n it is looking in\n";
		    qTree->putToString();
		    cout << endl;
		}
		vector<NameAsNode*> * auxVec = new vector<NameAsNode*>();	// all group as nodes
		vector<TreeNode*> * listVec = new vector<TreeNode*>();	//  all nodes in a tree
		qTree->getInfixList(listVec);
		for(vector<TreeNode*>::iterator iter = listVec->begin(); iter != listVec->end(); iter++){
			if ((*iter)->type() == TreeNode::TNAS){
				auxVec->push_back((NameAsNode*)*iter);
			}
		}

		for(vector<NameAsNode*>::iterator auxIter = auxVec->begin(); auxIter != auxVec->end(); auxIter++){

			vector<NameNode*> * usedBy =  (vector<NameNode*>*)(*auxIter)->getUsedBy();

			bool canTryToRemove = true;

			if (usedBy->size() == 0) {
				canTryToRemove = false;
			}
			for(vector<NameNode*>::iterator usedIter =usedBy->begin(); usedIter != usedBy->end(); usedIter++){
/*
				// here i check if *usedIter is bound in section with size 1 and if its father is dot
				// if it is so, than i copy the orig and delete from it all decl. and occurences and
				// evaluate it and check if all names are bound in the same node
				// tu sprawdzam czy (*usedIter) jest wiazany w sekcji o rozmiarze 1 i czy jego ojcem jest kropka
				// jezeli tak to kopije sobie znowu oryginal i usuwam z niego dekl i wszystkie wywolania i ewaluuje
				// potem sprawdzam czy wszystkie z used by sa wiazane w tym samym wezle

*/
				if (!canTryToRemoveAux(*auxIter,  usedBy)){
					canTryToRemove = false;
				} else {

				}
			}


			if (canTryToRemove){
				if (removedAux(*auxIter, usedBy, qTree)){
					removed = true;
					break;
				}
			} else {

			}

			(*auxIter)->putToString();
			cout << endl;
		}
	}
	cout <<"=============================================================================" << endl;
	cout <<"-----------------------------------------------------------------------------" << endl;
	cout <<"-----------------------------------------------------------------------------" << endl;
	cout<< "rmvAux end, zwraca" << endl;
//	qTree->putToString();
//	cout << endl;
//	cout << "\n serialize: \n";
	qTree->serialize();
	cout << endl;
	return 0;
}

bool AuxRmver::removedAux(NameAsNode * _auxNode, vector<NameNode*> *_toRemoveVec, TreeNode *&qTree){
	// jsi_kom cout<< "removedAux start\n";
	TreeNode *klon = qTree->clone();


	NameAsNode * auxNode;
	vector<NameNode*> *toRemoveVec = new vector<NameNode*>;

	vector<TreeNode*> *listVec2 = new vector<TreeNode*>();	// wszystkie wezly w drzewie
	klon->getInfixList(listVec2);
	auxNode = (NameAsNode*) TreeNode::getNodeByOid(listVec2, _auxNode->getOid());

	for (vector<NameNode*>::iterator rIter = _toRemoveVec->begin(); rIter != _toRemoveVec->end(); rIter++){
		toRemoveVec->push_back((NameNode*) TreeNode::getNodeByOid(listVec2, (*rIter)->getOid()));
	}

	QueryParser::setStatEvalRun(0);
	TreeNode::resetUsedNeeded(klon);

	// now i delete from the orig auxNode and all related with it (from toRemoveVec)

	// i delete name as node
	if (klon == auxNode){
		klon = auxNode->getArg();
		auxNode->getArg()->setParent(NULL);
	} else {
		TreeNode * parent = auxNode->getParent();
		TreeNode * son = auxNode->getArg();
		parent->swapSon(auxNode, son);
	}

	for(vector<NameNode*>::iterator rIter = toRemoveVec->begin(); rIter != toRemoveVec->end(); rIter++){
		// its father is a dot node
		NonAlgOpNode* parentDot = (NonAlgOpNode*) (*rIter)->getParent();
		TreeNode* left = parentDot->getLArg();
		TreeNode* right = parentDot->getRArg();
		if ((*rIter) == left){
			parentDot->getParent()->swapSon(parentDot, right);
		} else {// its right son of dot node
			if (NULL != parentDot->getParent()){
				parentDot->getParent()->swapSon(parentDot, left);
			} else {
				left->setParent(NULL);
				klon = left;
			}
		}
	}


	printf("wykonuje statyczna ewaluacje na klonie\n");
	qParser->statEvaluate(-1, klon);

	// sprawdzam czy wszystkie name nody z klona sa wiazane w tych samych wezlach co odpowiadajace im namenody z oryginalu
	vector<TreeNode*> * listVec = new vector<TreeNode*>();	// wszystkie wezly w drzewie
	klon->getInfixList(listVec);
	vector<NameNode*> * nameVec = new vector<NameNode*>();
	for(vector<TreeNode*>::iterator iter = listVec->begin(); iter != listVec->end(); iter++){
		if ((*iter)->type() == TreeNode::TNNAME){
			nameVec->push_back((NameNode*)(*iter));
		}
	}
	vector<TreeNode*> * oldlistVec = new vector<TreeNode*>();	// wszystkie wezly w drzewie
	qTree->getInfixList(oldlistVec);

	// jsi_kom cout<< "// sprawdzam czy wszystkie name nody z klona sa wiazane w tych samych wezlach co odpowiadajace im namenody z oryginalu \n";
	for(vector<NameNode*>::iterator nameIter = nameVec->begin(); nameIter != nameVec->end(); nameIter++){
		NameNode * newNode = *nameIter;
		assert(newNode != NULL);
		NameNode * oldNode = (NameNode*) TreeNode::getNodeByOid(oldlistVec, newNode->getOid());

		if (!oldNode)
			cout << "NULL !!!! " << endl;
		assert(oldNode != NULL);

		// jsi_kom cout<< "newNode->getBoundIn()->size(): " << newNode->getBoundIn()->size() << endl;

		bool ok = false;

		// dziala gdy jest wiazany tylko w jednym wezle? wtedy wlasciwie niepotrzebna ta petla, poprawic warunki
		for (vector<TreeNode*>::iterator niter = newNode->getBoundIn()->begin(); niter != newNode->getBoundIn()->end(); niter++){
			// jsi_kom cout<< "loop start" << endl;
			for (vector<TreeNode*>::iterator oiter = oldNode->getBoundIn()->begin(); oiter != oldNode->getBoundIn()->end(); oiter++){
				// jsi_kom cout<< "loop2 start" << endl;
				if ((*niter == NULL) && (*oiter == NULL)){		// zalezy od null - czyli jest wiazany w sekcji bazowej,
					ok = true;
				} else {
					long noid = (*niter)->getOid();		// ten wezel z nowego drzewa jest wiazany w wezle o takim id
					if ((*oiter)->getOid() == noid){

						ok = true;
					}
				}
			}
		}

		if (not ok){
			// jsi_kom cout<< "porownanie sie nie udalo, zwraca false\n";
			return false;
		} else {
			// jsi_kom cout<< "porownanie sie udalo\n";
		}

	}
	qTree = klon;
	// jsi_kom cout<< "porownanie sie udalo,  zwraca true ustawil qTree = klon\n";
	return true;
}

bool AuxRmver::canTryToRemoveAux(NameAsNode * auxNode, vector<NameNode*> *nameVec){
	// jsi_kom cout<< "AuxRmver::canTryToRemoveAux start " << nameVec->size() << endl;

	if (nameVec->size() == 0){

		return false;
	}
	for (vector<NameNode*>::iterator nameIter = nameVec->begin(); nameIter != nameVec->end(); nameIter++){
		if ((*nameIter)->getStatEnvSecSize() > 1){
			// jsi_kom cout<< "nie mozna usuwac bo jest wiazny w sekcji z o rozmiarze > 1 (rozmiar: " << (*nameIter)->getStatEnvSecSize() << ")" << endl;
			return false;
		}
		if ((*nameIter)->getParent()->type() != TreeNode::TNNONALGOP){
			// jsi_kom cout<< "nie mozna usuwac bo ojciec nie jest operatorem niealgebraicznym\n";
			return false;
		}
		if (((NonAlgOpNode*)(*nameIter)->getParent())->getOp() != NonAlgOpNode::dot){
			// jsi_kom cout<< "nie mozna usuwac bo ojciec nie jest operatorem kropki\n";
			return false;
		}
	}
	return true;
}


}
