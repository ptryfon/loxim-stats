#include "JoinRpcer.h"

namespace QParser{

JoinRpcer::JoinRpcer(QueryParser *qParser)
{
	this->qParser = qParser;
}

JoinRpcer::~JoinRpcer()
{
}

int JoinRpcer::replaceJoin(TreeNode *&qTree){
	cout << "replaceJoin start\n";
	cout <<"=============================================================================" << endl;
	cout <<"-----------------------------------------------------------------------------" << endl;
	cout <<"-----------------------------------------------------------------------------" << endl;
	
	this->qTree = qTree;
	cout << "dostal :" << endl;
	qTree->serialize();
	// qTree->putToString();
	cout << endl;
	
	cout << "rj before setStatEvalRun\n";
	QueryParser::setStatEvalRun(0);
	cout << "rj after setStatEvalRun\n";
	
	cout << "rj before resetUsedNeeded\n";
	TreeNode::resetUsedNeeded(qTree);
	cout << "rj after resetUsedNeeded\n";
	
	cout << "rj before statEvaluate\n";
	qParser->statEvaluate(qTree);
	cout << "rj after statEvaluate\n";
	
	bool replaced = true;
	while(replaced){
		replaced = false;
		
		if (Deb::ugOn()){
		    cout << "\n JoinRpcer szuka w \n";
		    qTree->putToString();
		    cout << endl;
		}
		vector<NonAlgOpNode*> *toReplaceVec =  new vector<NonAlgOpNode*>();
		getSupposed(toReplaceVec, qTree);
		cout << "\n JoinRpcer szuka w size:  " << toReplaceVec->size() << endl;
		// tu robie zastepowanie joina kropka, jezeli sie uda, ustawiam replaced = true;
		for(vector<NonAlgOpNode*>::iterator iter = toReplaceVec->begin(); iter != toReplaceVec->end(); iter++){
			if (canReplace(*iter)){
				replaceJoinWithDot((NonAlgOpNode*) (*iter)->getLArg());	
				replaced = true;
			}
		}
	}		
	cout <<"=============================================================================" << endl;
	cout <<"-----------------------------------------------------------------------------" << endl;
	cout <<"-----------------------------------------------------------------------------" << endl;
	cout<< "replaceJoin end, zwraca" << endl;
//	qTree->putToString();
//	cout << endl;
//	cout << "\n serialize: \n";
	qTree->serialize();
	cout << endl;
	return 0;	
}

	void JoinRpcer::getSupposed(vector<NonAlgOpNode*> *toReplaceVec, TreeNode *&qTree){
		vector<TreeNode*> *listVec =  new vector<TreeNode*>();
		qTree->getInfixList(listVec);
		for(vector<TreeNode*>::iterator iter = listVec->begin(); iter != listVec->end(); iter++){	
			if ((*iter)->type()==TreeNode::TNNONALGOP){
				NonAlgOpNode * dot = (NonAlgOpNode*)(*iter);
				if (dot->getOp() == NonAlgOpNode::dot){
					if (dot->getLArg()->type() == TreeNode::TNNONALGOP){
						if (((NonAlgOpNode*)dot->getLArg())->getOp() == NonAlgOpNode::join){
							toReplaceVec->push_back(dot);
						}	
					}
				}
			}
		}
	}
	/*					 .
	  				    / \
	 				 join  q3
					  /\
	 				 q1 q2
	*/
	bool JoinRpcer::canReplace(NonAlgOpNode* dotNode){
		// wiem ze lewym drzewem jest join
		TreeNode* q1 = ((NonAlgOpNode*)dotNode->getLArg())->getLArg();
		TreeNode* q2 = ((NonAlgOpNode*)dotNode->getLArg())->getRArg();
		TreeNode* q3 = (NonAlgOpNode*)dotNode->getRArg();
		
		vector<NameNode*> *q3NamesVec = new vector<NameNode*>();
		vector<TreeNode*> *q3NodesVec =  new vector<TreeNode*>();
		q3->getInfixList(q3NodesVec);
		
		for(vector<TreeNode*>::iterator iter = q3NodesVec->begin(); iter != q3NodesVec->end(); iter++){	
			if ((*iter)->type()==TreeNode::TNNAME){
				q3NamesVec->push_back((NameNode*)(*iter));
			}
		}
		
		for(vector<NameNode*>::iterator iter = q3NamesVec->begin(); iter != q3NamesVec->end(); iter++){
			vector<TreeNode*> *boundIn = (*iter)->getBoundIn();
			for(vector<TreeNode*>::iterator q1NodeIter = boundIn->begin(); q1NodeIter != boundIn->end(); q1NodeIter++){
				long oid = (*q1NodeIter)->getOid();
				if (q1->containsOid(oid)){
					return false;	
				}
			}
			
		}
		return true;
	}
	
	void JoinRpcer::replaceJoinWithDot(NonAlgOpNode* joinNode){
		joinNode->setOp(NonAlgOpNode::dot);
	}

}
