#include "Optimizer.h"
#include "TreeNode.h"
#include "Deb.h"

namespace QParser{


    TreeNode * Optimizer::getIndependant(TreeNode *niealg){
	Deb::ug("GETINDEPENDANT START");
	int min = ((NonAlgOpNode* ) niealg)->getFirstOpenSect();
	int max = ((NonAlgOpNode* ) niealg)->getLastOpenSect();
	int depth = 0;	
	Deb::ug("nonalg oper tree:");
	niealg->putToString();
	Deb::ug("------------------------");
	
	cout << "it is searching for independant in" << endl;
	(((TwoArgsNode *) niealg)->getRArg())->putToString();
	cout << "------------------------" << endl;
	
	TreeNode * tree = doGetIndep(((TwoArgsNode *) niealg)->getRArg(), min, max, depth);
	
	printf(" drzewko: %d, na glebokosci %d\n", (int) tree, depth);
	cout << "getIndependant returns: " << endl;
	if (tree != NULL)
		tree->putToString();
	else 
	    cout << "no independant subquery found" << endl;
	cout << "GETINDEPENDANT END" << endl;
	return tree;
    }

    
    bool Optimizer::isIndependent(TreeNode * tree, int min, int max){
	cout << "ISINDEPENDENT START" << endl;
	if (tree->type() == TreeNode::TNNAME){		// jezeli wezel z nazwa to sprawdzam w ktorej sekcji wiarze
	    NameNode *nnode = (NameNode *) tree;
	    if (nnode->getBindSect() < min || nnode->getBindSect() > max)
		return true;
	    else return false;
	} else if ((tree->type() == TreeNode::TNALGOP) || (tree->type() == TreeNode::TNNONALGOP)){
	    TreeNode *larg = ((TwoArgsNode *)tree)->getLArg();
	    TreeNode *rarg = ((TwoArgsNode *)tree)->getRArg();
	    if (isIndependent(larg, min, max) && isIndependent(rarg, min, max)) return true;
	    else return false;
	} else if ((tree->type() == TreeNode::TNUNOP) || (tree->type() == TreeNode::TNAS)){	// operator unarny
	    TreeNode *arg = ((UnOpNode *)tree)->getArg();
	    return isIndependent(arg, min, max);
	} else // ten wezel nie jest nazwa ani operatorem dwuargumentowym ani operatorem jednoargumentowym
	    return true;
    }
    
    TreeNode * Optimizer::doGetIndep(TreeNode * tree, int min, int max, int &depth){ 
	if ((tree->type() == TreeNode::TNNONALGOP) && this->isIndependent(tree, min, max)){
	    return tree;
	} else if (tree->type() == TreeNode::TNALGOP){
	    TreeNode *larg = ((TwoArgsNode *) tree)->getLArg();
	    TreeNode *rarg = ((TwoArgsNode *) tree)->getRArg();
	    int dl = depth + 1;
	    int dr = depth + 1;
	    TreeNode *lind = doGetIndep(larg, min, max, dl);
	    TreeNode *rind = doGetIndep(rarg, min, max, dr);
	    if (lind == NULL){ 
		depth = dr;
		return rind;
	    } else if (rind == NULL){
		depth = dl;
		return lind;
	    } else if(dl < dr){			// w obu poddrzewach znalazl niezalezne podzapytanie, zwracam blizsze
		depth = dl;
		return lind;
	    } else {
		depth = dr;
		return rind;
	    }
	} else if(tree->type() == TreeNode::TNUNOP) {
	    TreeNode *arg = ((UnOpNode *)tree)->getArg();
	    depth++;
	    return doGetIndep(arg, min, max, depth);
	} else {
	    Deb::ug("jsi doGetIndep: ELSE ZWRACA NULL !!!!\n");
	    return NULL;
	}    
    }
}

