#ifndef _QUERY_PARSER_H_
#define _QUERY_PARSER_H_

#include <string>
#include <QueryParser/ClassNames.h>

using namespace std;


namespace QParser
{
#define DML_AC_RETURN_ERROR		"return_error"
#define DML_AC_DO_QUERY			"perform_query"
    class QueryParserGen;
    class QueryLexer;
    class TreeNode;

    class QueryParser
    {
    	protected:
	    QueryParserGen *parser;
	    QueryLexer *lexer;
	    TreeNode *d; //for the parser to return values
	    StatQResStack *sQres;
	    StatEnvStack *sEnvs;
	    static int statEvalRun;	// for which time the stat eval is called - some things can be set only once
		bool shouldOptimize;
		bool shouldTypeCheck;
		bool tcTurnedOffTmp;		// to switch typechecking off temporarily.
		string dmlIncompleteAction;

	public:
		QueryParser();
		QueryParser(int sessionId);
		QueryParser(int sessionId, bool readConfig);
		virtual int parseIt (int sessionId, string s, TreeNode *&aTree, string &s2, bool toBeTypeChecked, bool toBeOptimized);
		virtual int parseIt (int sessionId, string s, TreeNode *&aTree);
		virtual bool usesMdn() {return (shouldOptimize || shouldTypeCheck);}
		virtual int testTypeCheck(TreeNode *qTree);
		virtual int testParse (string s, TreeNode *&aTree);
		virtual void testDeath (string zap);
		virtual void setQres(StatQResStack *nq);
		virtual void setEnvs(StatEnvStack *ne);
		virtual void setTcOffTmp(bool tcoff);
		virtual bool isTcOffTmp();
		virtual int statEvaluate(int sessionId, TreeNode *&tn);
		virtual ~QueryParser();
		static int getStatEvalRun();
		static void setStatEvalRun(int n);
		static void incStatEvalRun();
    };

}

#endif


