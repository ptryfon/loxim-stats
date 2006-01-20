#include "Optymalizator.h"
#include "TreeNode.h"

namespace QParser{


    TreeNode * Optymalizator::getIndependant(TreeNode *niealg){
	int min = ((NonAlgOpNode* ) niealg)->getFirstOpenSect();
	int max = ((NonAlgOpNode* ) niealg)->getLastOpenSect();
	int depth = 0;	
	TreeNode * tree = doGetIndep(((TwoArgsNode *) niealg)->getRArg(), min, max, depth);
	printf(" drzewko: %d, na glebokosci %d\n", (int) tree, depth);
	return tree;
    }

    
    bool Optymalizator::isIndependent(TreeNode * tree, int min, int max){
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
	} else if (tree->type() == TreeNode::TNUNOP){	// operator unarny
	    TreeNode *arg = ((UnOpNode *)tree)->getArg();
	    return isIndependent(arg, min, max);
	} else // ten wezel nie jest nazwa ani operatorem dwuargumentowym ani operatorem jednoargumentowym
	    return true;
    }
    
    TreeNode * Optymalizator::doGetIndep(TreeNode * tree, int min, int max, int &depth){
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
	} else return NULL;
    }
}

