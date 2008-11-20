#ifndef _OPTYM_H_
#define _OPTYM_H_

#include <iostream>
#include <vector>
#include <string>
#include "ClassNames.h"


using namespace std;

namespace QParser{

class Optimizer{
    public:
    
    TreeNode * getIndependant(TreeNode *niealg);
    bool isIndependent(TreeNode * tree, int min, int max); // min max - wiazanie nie moze zawierac sie w tym przedziale
    TreeNode * doGetIndep(TreeNode *tree, int min, int max, int &depth);
};

}
#endif


