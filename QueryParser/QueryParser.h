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
    	protected:
	    StatQResStack *sQres;
	    StatEnvStack *sEnvs;
	public:
	QueryParser() {sQres = NULL; sEnvs = NULL;}
	virtual int parseIt (string s, TreeNode *&aTree);
	
	virtual int testParse (string s, TreeNode *&aTree);
	virtual void testDeath ();
	virtual void setQres(StatQResStack *nq);
	virtual void setEnvs(StatEnvStack *ne);
        virtual int statEvaluate(TreeNode *&tn);	
	virtual ~QueryParser();
    };
		
}

#endif


