#include <iostream>
#include <string>
#include <sstream>
#include <QueryParser/QueryLexer.h>
#include <QueryParser/TreeNode.h>
#include <QueryParser/Stack.h>
#include <QueryParser/DataRead.h>
#include <QueryParser/QueryParser.h>
#include <QueryParser/DeathRmver.h>
#include <QueryParser/AuxRmver.h>
#include <QueryParser/JoinRpcer.h>
#include <Errors/ErrorConsole.h>
#include <Errors/Errors.h>
#include <Config/SBQLConfig.h>
#include <Indexes/QueryOptimizer.h>
#include <QueryParser/Deb.h>
//#include <TypeCheck/TCConstants.h>
#include <TypeCheck/TypeChecker.h>


// using namespace std;
using namespace Errors;
using namespace Config;
using namespace TypeCheck;

namespace QParser {

	int  QueryParser::statEvalRun = 0;
	int  QueryParser::getStatEvalRun(){return statEvalRun;};
	void QueryParser::setStatEvalRun(int n){QueryParser::statEvalRun = n;};
	void QueryParser::incStatEvalRun(){statEvalRun++;};
	void QueryParser::setQres(StatQResStack *nq) {this->sQres = nq;}
	void QueryParser::setEnvs(StatEnvStack *nq) {this->sEnvs = nq;}
	QueryParser::~QueryParser() {
		if (sQres != NULL) delete sQres;
		if (sEnvs != NULL) delete sEnvs;
		delete parser;
		delete lexer;
	}
	QueryParser::QueryParser() {
		sQres = NULL;
		sEnvs = NULL;
		SBQLConfig conf("QueryParser");
		conf.getBool("optimisation", shouldOptimize);
		conf.getBool("type_checking", shouldTypeCheck);
		conf.getString("metadata_incomplete_action", dmlIncompleteAction);
		tcTurnedOffTmp = false;
		/* check if this is debug mode or not... */
		Deb::setWriteDebugs();
		Deb::ug("Parser debug mode is ON! you can change it in the configuration file");
		if (shouldOptimize || shouldTypeCheck) {
			DataScheme::reloadDScheme(-1);
		}
		lexer = new QueryLexer();
		parser = new QueryParserGen(lexer, &d);
	}

	QueryParser::QueryParser(int sessionId) {
		sQres = NULL;
		sEnvs = NULL;
		SBQLConfig conf("QueryParser");
		conf.getBool("optimisation", shouldOptimize);
		conf.getBool("type_checking", shouldTypeCheck);
		conf.getString("metadata_incomplete_action", dmlIncompleteAction);
		tcTurnedOffTmp = false;
		/* check if this is debug mode or not... */
		Deb::setWriteDebugs();
		Deb::ug("Parser debug mode is ON! you can change it in the configuration file");
		if (shouldOptimize || shouldTypeCheck) {
			DataScheme::reloadDScheme(sessionId);
		}
		lexer = new QueryLexer();
		parser = new QueryParserGen(lexer, &d);
	}

	//for internal use of parser.
	QueryParser::QueryParser(int sessionId, bool readConfig) {
		sQres = NULL;
		sEnvs = NULL;
		tcTurnedOffTmp = false;
		if (readConfig) {
			SBQLConfig conf("QueryParser");
			conf.getBool("optimisation", shouldOptimize);
			conf.getBool("type_checking", shouldTypeCheck);
			conf.getString("metadata_incomplete_action", dmlIncompleteAction);
			if (shouldOptimize || shouldTypeCheck) {
				DataScheme::reloadDScheme(sessionId);
			}
		} else {
			shouldOptimize = false;
			shouldTypeCheck = false;
		} 
		lexer = new QueryLexer();
		parser = new QueryParserGen(lexer, &d);
	}

	void QueryParser::setTcOffTmp(bool tcoff) {
		tcTurnedOffTmp = tcoff;
	}
	bool QueryParser::isTcOffTmp() {
		return tcTurnedOffTmp;
	}

	int QueryParser::statEvaluate(int sessionId, TreeNode *&tn) {
		QueryParser::incStatEvalRun();
		/* init the both static stacks. The data model is available as	 *
		* DataRead::dScheme()						 */
		setEnvs(new StatEnvStack());
		setQres(new StatQResStack());

		/* set the first section of the ENVS stack with binders to *
		* definitions of base (i.e. root) objects ...             */
		sEnvs->pushBinders(DataScheme::dScheme(sessionId)->bindBaseObjects());
		if (Deb::ugOn()) {sEnvs->putToString();	cout << endl; }
		Deb::ug("OK, the ENVS is ready with root objects on the bottom.");
		/* evoke static_eval, which will fill in special info in nameNodes and NonAlgOpNodes */
		int res = tn->staticEval (sQres, sEnvs);
		if (Deb::ugOn()) {tn->putToString(); cout << endl; }
		return res;
	}
	/** TypeChecker testing... */
	int QueryParser::testTypeCheck(TreeNode *qTree) {
		Signature *lSig = new SigAtomType("string");
		Signature *rSig = new SigAtomType("string");
		TypeChecker *tc = new TypeCheck::TypeChecker(qTree);
		DecisionTable *dt = tc->getDTable(DTable::ALG, AlgOpNode::plus);

		cout << "gotDTable(alg, plus);" << endl;
		TypeCheckResult tcRes;
		int intTCRes = dt->getResult(tcRes, lSig, rSig);
		cout << "getResult int res: " << intTCRes << endl;
		cout << "tcres:\n" << tcRes.printAllInfo() << endl;

		return 0;
	}
	/** end of TypeChecker testing */

	int QueryParser::parseIt(int sessionId, string query, TreeNode *&qTree) {
		string tcResultString = "";
		int errcode = parseIt(sessionId, query, qTree, tcResultString, false, false);
		return errcode;
	}

	int QueryParser::parseIt(int sessionId, string query, TreeNode *&qTree, string &tcResultString, bool toBeTypeChecked, bool toBeOptimized) {
		QueryParser::setStatEvalRun(0);
		ErrorConsole &ec(ErrorConsole::get_instance(EC_QUERY_PARSER));


		stringstream ss (stringstream::in | stringstream::out);
		ss << query;
		lexer->switch_streams(&ss, 0);
		int res = parser->parse();
		if (res != 0){
			debug_print(ec,  "Query not parsed properly...\n");
			debug_print(ec,  (ErrQParser | ENotParsed));
			return (ErrQParser | ENotParsed);
		} else {
			Deb::ug("Query parsed OK.");
		}
		qTree = d;

		Indexes::QueryOptimizer::optimizeWithIndexes(qTree);

		//Nothing more needs to be done, if this is an internal query. (no screen logging, typechecking, optimization)
		if (!toBeTypeChecked && !toBeOptimized) {
			return 0;
		}

		if (Deb::ugOn()) {
			cout << " Tree before optimization:";
			cout << "\n--------------------------------------\n";
			qTree->putToString();
			cout << "\n--------------------------------------\n";
		}
		if ((shouldTypeCheck || shouldOptimize) && !qTree->skipPreprocessing()) {
			TreeNode *nt = d->clone();
			bool metadataCorrect = DataScheme::dScheme(sessionId)->getIsComplete();
			bool metadataUpToDate = DataScheme::dScheme(sessionId)->getIsUpToDate();
			if (!metadataCorrect || !metadataUpToDate) {
				debug_print(ec,  "Data scheme is not complete or not up to date); optimization and typechecking blocked.");
			}

			int typeCheckErrCode = 0;
			if (shouldTypeCheck && toBeTypeChecked && metadataCorrect && metadataUpToDate) {
				if (isTcOffTmp()) { debug_print(ec,  "Typechecking turned off temporarily"); tcResultString = "";}
				else {
					Deb::ug(" \n \n TYPECHECK BEGIN !!! \n \n");
					TypeChecker *tc = new TypeCheck::TypeChecker(nt);
					typeCheckErrCode = tc->doTypeChecking(tcResultString);
					Deb::ug(" \n \n TYPECHECK DONE... \n ");
					if ((typeCheckErrCode != 0) && (typeCheckErrCode != (ErrTypeChecker | ETCNotApplicable))) {
						debug_print(ec,  "TC, Parser: typeCheckErrorCode says general TC error, should return string to user.\n");
						return typeCheckErrCode;
					}
					if (typeCheckErrCode == 0) {
						delete qTree;
						qTree = nt;
					}
				}
			} else {
				if (shouldTypeCheck && toBeTypeChecked && dmlIncompleteAction == DML_AC_RETURN_ERROR) {
					if (!metadataCorrect) return (ErrTypeChecker | EIncompleteMetadata);
					else return (ErrTypeChecker | EMetadataOutdated);
				}
				tcResultString = "";
				Deb::ug("TypeChecking is disabled.");
			}

			if (shouldOptimize && toBeOptimized && metadataCorrect && metadataUpToDate) {
				Deb::ug(" optimisation is set ON !");
				int optres = 0;
				int stat_ev_res;
				nt = qTree->clone();
				if ((stat_ev_res = this->statEvaluate(sessionId, nt)) != 0) {
					Deb::ug("static evaluation did not work out ...");
					optres = -1;
				} else {
					int optres = -2;
					DeathRmver *rmver = new DeathRmver(this);
					rmver->rmvDeath(nt);
					if (this->statEvaluate(sessionId, nt) != 0) optres = -1;
	/* The main optimisation loop - factor out single independent subqueries *
	* as long as ... such exist !                                           */
					while (optres == -2) {
						optres = nt->optimizeTree();
						while (nt->getParent() != NULL) nt = nt->getParent();
						/*one more static eval, to make sure nodes have the right info.. */
						fprintf (stderr, "one more stat eval..\n");
						if (this->statEvaluate(sessionId, nt) != 0) optres = -1;
					}
					AuxRmver *auxRmver = new AuxRmver(this);
					auxRmver->rmvAux(nt);
					JoinRpcer *joinRpcer = new JoinRpcer(this);
					joinRpcer->replaceJoin(nt);
				}
				if (optres != -1) {
					Deb::ug("I'll return optimized tree\n");
					qTree = nt;
					if (Deb::ugOn()) {
						cout << "Tree after optimization:\n--------------------------------------\n";
						qTree->putToString(); cout << "\n--------------------------------------\n";
					}
				} else { Deb::ug("I'll return the tree from before static eval..");}
			} else {
				if (shouldTypeCheck && toBeTypeChecked && dmlIncompleteAction == DML_AC_RETURN_ERROR) {
					if (!metadataCorrect) return (ErrTypeChecker | EIncompleteMetadata);
					else return (ErrTypeChecker | EMetadataOutdated);
				}
				Deb::ug(" optimisation is disabled. \n");
			}
		} else {
			qTree->qpLocalAction(this);
		}
		Deb::ug(" ParseIt() ends successfully!");
		/*			cout << "td1------------------------------------------------------------\n";
					testDeath("EMP.NAME");
					cout << "td2------------------------------------------------------------\n";
					testDeath("(EMP join (WORKS_IN.DEPT)).NAME;");
					cout << "td3------------------------------------------------------------\n";
		*/
		if (Deb::ugOn()) {
			cout << "===================================================================" << endl;
			cout << "P    A    R    S    E    R        R    E    T    U    R    N    S :" << endl;
			qTree->serialize();
			cout << "\n===================================================================" << endl;
		}
		return 0;
	}

    void QueryParser::testDeath(string _zap){
//    	string zap = "(EMP join (WORKS_IN.DEPT)).NAME;";
	    cout << "TESTDEATH START---------------------------------------------------------------------------------" << endl;
	    stringstream ss (stringstream::in | stringstream::out);
	    ss << _zap;
	    lexer->switch_streams(&ss, 0);
	    int res = parser->parse();
			cout << "parse result: " << res << endl;
	    TreeNode *tree = d;

	    cout << "Odczyt z drzewka, ktore przekazuje:" << endl;
	    cout << "--------------------------------------" << endl;
	    tree->putToString();
	    cout << "\n--------------------------------------" << endl;
    	    QParser::DeathRmver *rmver = new QParser::DeathRmver(this);

		cout << "now the following tree will be evaluated statically.." << endl;
	    TreeNode *nt = d->clone();

		rmver->rmvDeath(nt);

	    cout << "TEST END__---------------------------------------------------------------------------------" << endl;
		cout << "koniec testDeath\n";
    }

    int QueryParser::testParse (string query, TreeNode *&qTree) {
	string zap = "EMP where SAL = (EMP where NAME=\"KUBA\").SAL;";

	if (false && (query == zap)){
	    cout << "TEST START---------------------------------------------------------------------------------" << endl;
	    stringstream ss (stringstream::in | stringstream::out);
	    ss << zap;
	    lexer->switch_streams(&ss, 0);
	    int res = parser->parse();

	cout << "parse result: " << res << endl;
	    TreeNode *tree = d;

	    //printf( "po parsowaniu treeNode: %d. \n", tree);
	    cout << "Odczyt z drzewka, ktore przekazuje:" << endl;
	    cout << "--------------------------------------" << endl;
	    tree->putToString();
	    cout << "\n--------------------------------------" << endl;
    	    QParser::Optimiser *opt = new QParser::Optimiser();
	    int reslt;
	    cout << "now the following tree will be evaluated statically.." << endl;
	    TreeNode *nt = d->clone();

	    if ((reslt = opt->stEvalTest(nt)) != 0)
		fprintf (stderr, "static evaluation did not work out...\n");
	    else {
		cout << "static evaluatioin OK, result: "<< reslt << endl;


		fprintf (stderr, "now I will try to optimise the tree..\n");

		cout << "OPTYMALIZUJE--------------------------------------------------------"<< endl;
		//cout << "przed optymalizacjia" << endl;
		//nt->putToString();
		//cout << "-----------" << endl;

		int ooptres = nt->optimizeTree();
		cout << "KONIEC OPTYMALIZACJI-------------------------------------------------------"<< endl;
		/*the nt tree needs to be 'rolled' up: (in case the nonalgopnode
		  we were operating on was the root... */
		while (nt->getParent() != NULL) nt = nt->getParent();

		fprintf (stderr, "__%d__", ooptres);

		cout << "po JEDNYM PRZEBIEGU optymalizacji" << endl;
		nt->putToString();
		cout << "-----------" << endl;
		fprintf (stderr, "end of optimisation.\n");
	    }
	    cout << "TEST END__---------------------------------------------------------------------------------" << endl;
	}

		cout << "koniec parseIt\n";
		return 0;
	}

}

/*
int main (int argc, char* argv[]) {

    string theStr = argv[1];

    cout << "zaraz zaczne maina \n";
	cout << "dostalem stringa z parametru: " << theStr <<"\n" ;
    QParser::QueryParser *qp = new QParser::QueryParser();
    QParser::TreeNode *tNode;
    qp->parseIt(theStr, tNode);

//	cout << "teraz bede testowal stosy... " << endl;
//	QParser::Optimiser *opt = new QParser::Optimiser();
//	int res = opt->simpleTest();

//	cout << "to tyle z testow stosow. wynik testow: "<< res << endl;
//    cout << "zaraz zrobie factorSub" << endl;
//    QParser::TreeNode *modNode = tNode->factorSubQuery( tNode->getRArg()->getRArg(), "sik");
//    cout << "\n ~~~~~~~~~~~~~~~~~~" << endl;
//    modNode->putToString();
//    cout << "\n ~~~~~~~~~~~~~~~~~~" << endl;
  //  cout << "skonczylem maina \n";
    return 0;
}

*/





