#ifndef _QUERY_PARSER_H_
#define _QUERY_PARSER_H_

#include <string>
//#include "TreeNode.h"
#include "ClassNames.h"

using namespace std;

namespace QParser 
{

    class QueryParser 
    {
	public:

	virtual int parseIt (string s, TreeNode *&aTree);
	virtual ~QueryParser() {}
    };
	
	
}

#endif
