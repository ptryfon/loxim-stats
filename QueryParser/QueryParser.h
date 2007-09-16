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
	    static int statEvalRun;	// for which time the stat eval is called - some things can be set only once
		bool shouldOptimize;
	public:
	QueryParser();
	virtual int parseIt (string s, TreeNode *&aTree);
	
	virtual int testParse (string s, TreeNode *&aTree);
	virtual void testDeath (string zap);
	virtual void setQres(StatQResStack *nq);
	virtual void setEnvs(StatEnvStack *ne);
        virtual int statEvaluate(TreeNode *&tn);	
	virtual ~QueryParser();
	static int getStatEvalRun();
	static void setStatEvalRun(int n);
	static void incStatEvalRun();
    };
		
}

#endif


