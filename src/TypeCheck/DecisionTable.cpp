#include <TypeCheck/DecisionTable.h>

#include <Errors/ErrorConsole.h>
#include <QueryExecutor/QueryResult.h>
#include <QueryParser/Stack.h>
#include <QueryParser/TreeNode.h>
#include <TransactionManager/Transaction.h>
#include <TransactionManager/Mutex.h>

#include <TypeCheck/Rule.h>
#include <TypeCheck/TypeChecker.h>
#include <TypeCheck/TypeCheckResult.h>

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>

using namespace Errors;
using namespace QParser;
using namespace std;
using namespace TypeCheckConstants;

namespace TypeCheck
{

	/** Decision tables for the type checking procedure. */

	void DecisionTable::initAlgOpRules(int op) {
		cout << "[TC]: " << "initint AlgOpRules: " << op << " operator." << endl;
		switch (op) {
		case AlgOpNode::bagIntersect: {
			addTcRule(M_B,		"X",		"X",		BS_COPY_L);
			addTcRule(M_ELSE,	"",			"",			TC_RS_ERROR);
			/*CARD*/
			addCdRule(M_B,		"X",		"Y",		CD_INTST);
			/*T-NAME*/
			addTnRule(M_B,		M_0, 		M_0, 		"");
			addTnRule(M_EQ,		"X",		"X",		"");
			addTnRule(M_ELSE, 							TC_RS_ERROR);
			/*C-KIND*/
			addCkRule(M_B,		M_0,		M_0,		"bag");
			addCkRule(M_L,		M_0,		"bag",		"bag");
			addCkRule(M_R,		"bag",		M_0,		"bag");
			addCkRule(			"bag",		"bag",		"bag");
			addCkRule(M_ELSE,							TC_RS_ERROR);
			break;
		}
		case AlgOpNode::bagMinus: {
			addTcRule(M_B,		"X",		"X",		BS_COPY_L);
			addTcRule(M_ELSE,	"",			"",			TC_RS_ERROR);
			/*CARD*/
			addCdRule(M_B,		"X",		"Y",		CD_COPY_L_ZR);	//like for 'where'
			/*T-NAME*/
			addTnRule(M_B,		M_0, 		M_0, 		"");
			addTnRule(M_EQ,		"X",		"X",		"");
			addTnRule(M_ELSE, 							TC_RS_ERROR);
			/*C-KIND*/
			addCkRule(M_B,		M_0,		M_0,		"bag");
			addCkRule(M_L,		M_0,		"bag",		"bag");
			addCkRule(M_R,		"bag",		M_0,		"bag");
			addCkRule(			"bag",		"bag",		"bag");
			addCkRule(M_ELSE,							TC_RS_ERROR);
			break;
		}
		case AlgOpNode::bagUnion: {
/*BASE*/
			addTcRule(M_EQ,		"X",		"X",		BS_COPY_L);
			addTcRule(M_ELSE,	"X",		"Y",		BS_TO_VAR);
/*CARD*/
			addCdRule(M_B,		"X",		"Y",		CD_ADDBANDS);
/*T-NAME*/
			addTnRule(M_B,		M_0, 		M_0, 		"");
			addTnRule(M_EQ,		"X",		"X",		"");
			addTnRule(M_ELSE, 							TC_RS_ERROR);
/*C-KIND*/
			addCkRule(M_B,		M_0,		M_0,		"bag");
			addCkRule(M_L,		M_0,		"bag",		"bag");
			addCkRule(M_R,		"bag",		M_0,		"bag");
			addCkRule(			"bag",		"bag",		"bag");
			addCkRule(M_ELSE,							TC_RS_ERROR);
			break;
		}
		case AlgOpNode::plus: {
/*BASE*/
			addTcRule(			"integer",	"integer",	"integer");
			addTcRule(			"double",	"double",	"double");
			addTcRule(			"string",	"string",	"string");
			addTcRule(			"boolean",	"boolean",	"boolean"); 	//works like...  OR ?
			addTcRule(			"integer",	"double",	"double");
			addTcRule(			"double",	"integer",	"double");
			addTcRule(			"integer",	"string",	"string",	BS_TO_STR, LEFT, STATIC);
			addTcRule(			"string",	"integer",	"string",	BS_TO_STR, RIGHT, STATIC);
			addTcRule(			"double",	"string",	"string",	BS_TO_STR, LEFT, STATIC);
			addTcRule(			"string",	"double",	"string",	BS_TO_STR, RIGHT, STATIC);
			addTcRule(M_ELSE, 							TC_RS_ERROR);
/*CARD*/
			addCdRule(			"1..1",		"1..1",		"1..1");
			addCdRule(M_L,		"",			"1..1", 	"1..1", 	CARD_TO_11, LEFT, DYNAMIC);
			addCdRule(M_R,		"1..1",		"", 		"1..1", 	CARD_TO_11, RIGHT, DYNAMIC);
			addCdRule(M_B,		"", 		"", 		"1..1", 	CARD_TO_11, BOTH, DYNAMIC);
/*T-NAME*/
			addTnRule(M_B,		M_0,		M_0,		"");
			addTnRule(M_ELSE, 							TC_RS_ERROR);
			break;
		}
		case AlgOpNode::minus: {
/*BASE*/
			addTcRule(			"integer",	"integer",	"integer");
			addTcRule(			"double",	"double",	"double");
			addTcRule(			"integer",	"double",	"double");
			addTcRule(			"string",	"integer",	"integer",	BS_TO_INT, LEFT, DYNAMIC);
			addTcRule(			"integer",	"string",	"integer",	BS_TO_INT, RIGHT, DYNAMIC);
			addTcRule(			"string",	"double",	"double",	BS_TO_DBL, LEFT, DYNAMIC);
			addTcRule(			"double",	"string",	"double",	BS_TO_DBL, RIGHT, DYNAMIC);
			addTcRule(M_ELSE, 	"", 		"", 		TC_RS_ERROR);	//so eg. cant subtract strings...
/*CARD*/
			addCdRule(			"1..1", 	"1..1", 	"1..1");
			addCdRule(M_L,		"",			"1..1", 	"1..1", 	CARD_TO_11, LEFT, DYNAMIC);
			addCdRule(M_R,		"1..1",		"", 		"1..1", 	CARD_TO_11, RIGHT, DYNAMIC);
			addCdRule(M_B,		"", 		"", 		"1..1", 	CARD_TO_11, BOTH, DYNAMIC);
/*T-NAME*/
			addTnRule(M_B,		M_0,		M_0,		"");
			addTnRule(M_ELSE, 	"", 		"", 		TC_RS_ERROR);
			break;
		}
		case AlgOpNode::divide: 	// BELOW
		case AlgOpNode::times: {
/*BASE*/
			addTcRule(			"integer",	"integer",	"integer");
			addTcRule(			"double",	"double",	"double");
			addTcRule(			"integer",	"double",	"double");
			addTcRule(			"double",	"integer",	"double");
			addTcRule(			"boolean",	"boolean",	"boolean"); //works like  AND
			addTcRule(			"string",	"double",	"double",	BS_TO_DBL, LEFT, DYNAMIC);
			addTcRule(			"double",	"string",	"double",	BS_TO_DBL, RIGHT, DYNAMIC);
			addTcRule(M_ELSE, 							TC_RS_ERROR);
/*CARD*/
			addCdRule(			"1..1",		"1..1",		"1..1");
			addCdRule(M_L,		"",			"1..1", 	"1..1", 	CARD_TO_11, LEFT, DYNAMIC);
			addCdRule(M_R,		"1..1",		"", 		"1..1", 	CARD_TO_11, RIGHT, DYNAMIC);
			addCdRule(M_B,		"", 		"", 		"1..1", 	CARD_TO_11, BOTH, DYNAMIC);
/*T-NAME*/
			addTnRule(M_B,		M_0,		M_0,		"");
			addTnRule(M_ELSE, TC_RS_ERROR);
			break;
		}
		case AlgOpNode::neq: 	// BELOW
		case AlgOpNode::eq: {
/*BASE*/
			addTcRule(			"double",	"double",	TC_RS_ERROR);	// not allowed to compare doubles!
			addTcRule(M_EX,		"variant",	"",			TC_RS_ERROR);	// not allowed to compare variants.
			addTcRule(			"string",	"integer",	"boolean",	BS_TO_STR, RIGHT, STATIC);
			addTcRule(			"integer",	"string",	"boolean",	BS_TO_STR, LEFT, STATIC);
			addTcRule(			"string",	"double",	"boolean",	BS_TO_STR, RIGHT, STATIC);
			addTcRule(			"double",	"string",	"boolean",	BS_TO_STR, LEFT, STATIC);
			addTcRule(M_EQ,		"X",		"X",		"boolean");
			addTcRule(M_ELSE, TC_RS_ERROR);
/*CARD*/
			addCdRule(			"1..1",		"1..1",		"1..1");
			addCdRule(M_L,		"",			"1..1", 	"1..1", 	CARD_TO_11, LEFT, DYNAMIC);
			addCdRule(M_R,		"1..1",		"", 		"1..1", 	CARD_TO_11, RIGHT, DYNAMIC);
			addCdRule(M_B,		"", 		"", 		"1..1", 	CARD_TO_11, BOTH, DYNAMIC);
/*T-NAME*/
			addTnRule(M_B,		M_0,		M_0,		"");
			addTnRule(M_EQ,		"",			"",			"");
			addTnRule(M_ELSE, TC_RS_ERROR);
			break;
		}
		//all following comparisons ( < , > , <= , >= ) have the same rules :
		case AlgOpNode::lt: 	// BELOW
		case AlgOpNode::gt:		// BELOW
		case AlgOpNode::le:		// BELOW
		case AlgOpNode::ge:{
/*BASE*/
			addTcRule(			"integer",	"integer",	"boolean");
			addTcRule(			"string",	"string",	"boolean");
			addTcRule(			"double",	"double",	"boolean");
			addTcRule(			"string",	"integer",	"boolean",	BS_TO_INT, LEFT, DYNAMIC);
			addTcRule(			"integer",	"string",	"boolean",	BS_TO_INT, RIGHT, DYNAMIC);
			addTcRule(			"string",	"double",	"boolean",	BS_TO_DBL, LEFT, DYNAMIC);
			addTcRule(			"double",	"string",	"boolean",	BS_TO_DBL, RIGHT, DYNAMIC);
			addTcRule(M_ELSE, TC_RS_ERROR);
/*CARD*/
			addCdRule(			"1..1",		"1..1",		"1..1");
			addCdRule(M_L,		"",			"1..1", 	"1..1", 	CARD_TO_11, LEFT, DYNAMIC);
			addCdRule(M_R,		"1..1",		"", 		"1..1", 	CARD_TO_11, RIGHT, DYNAMIC);
			addCdRule(M_B,		"", 		"", 		"1..1", 	CARD_TO_11, BOTH, DYNAMIC);
/*T-NAME*/
			addTnRule(M_B,		M_0,		M_0,		"");
			addTnRule(M_ELSE, TC_RS_ERROR);
			break;
		}
		//TODO: fill the rulesets...:
		case AlgOpNode::boolAnd:	// BELOW
		case AlgOpNode::boolOr: {
/*BASE*/
			addTcRule(			"boolean",	"boolean",	"boolean");
			addTcRule(M_ELSE, TC_RS_ERROR);
/*CARD*/
			addCdRule(			"1..1",		"1..1",		"1..1");
			addCdRule(M_L,		"",			"1..1", 	"1..1", 	CARD_TO_11, LEFT, DYNAMIC);
			addCdRule(M_R,		"1..1",		"", 		"1..1", 	CARD_TO_11, RIGHT, DYNAMIC);
			addCdRule(M_B,		"", 		"", 		"1..1", 	CARD_TO_11, BOTH, DYNAMIC);

			break;
		}
		case AlgOpNode::comma: {
			/*BASE*/
			addTcRule(M_EX,		"variant",	"",			TC_RS_ERROR);
			addTcRule(M_B,		"X",		"Y",		BS_STRUCT);
			/*CARD*/
			addCdRule(M_B,		"X",		"Y",		CD_MULTBANDS);
			/*T-NAME*/
			addTnRule(M_B,		"X",		"Y",		"");//struct always gets an empty distinct typename...
			/*C-KIND*/
			addCkRule(M_B,		M_0,		M_0,		"bag");
			addCkRule(M_L,		M_0,		"bag",		"bag");
			addCkRule(M_R,		"bag",		M_0,		"bag");
			addCkRule(			"bag",		"bag",		"bag");
			addCkRule(M_ELSE, 							TC_RS_ERROR);
			break;
		}
		case AlgOpNode::insert: {	//complex checks under base generators BS_INS_BND, BS_INS_REF
			addTcRule(M_MATCH,	"ref",		"binder",	BS_INS_BND);
			addTcRule(M_MATCH,	"ref",		"ref",		BS_INS_REF);
			addTcRule(M_ELSE,							TC_RS_ERROR);

			addCdRule(M_R,		"1..1",		"x..y",		"1..1");
			addCdRule(M_ELSE,	"",			"",			"1..1",		CARD_TO_11, LEFT, DYNAMIC);

			break;
		}
		case AlgOpNode::assign: {	//complex checks under base generator: BS_ASSIGN
			addTcRule(M_R,		"ref",		"match",	BS_ASSIGN);	//this takes care of sigs' compatibility
			addTcRule(M_ELSE,							TC_RS_ERROR);

			addCdRule(			"1..1",		"1..1",		"1..1");
			addCdRule(M_L,		"",			"1..1", 	"1..1", 	CARD_TO_11, LEFT, DYNAMIC);
			addCdRule(M_R,		"1..1",		"", 		"1..1", 	CARD_TO_11, RIGHT, DYNAMIC);
			addCdRule(M_B,		"", 		"", 		"1..1", 	CARD_TO_11, BOTH, DYNAMIC);

			break;
		}
		case AlgOpNode::semicolon: { // does nthing, but returns right signature.
			addTcRule(M_B,		"X",		"Y",		BS_COPY_R);
			addCdRule(M_B,		"X",		"Y",		ARG_COPY_R);
			addTnRule(M_B,		"X",		"Y",		ARG_COPY_R);
			addCkRule(M_B,		"X",		"Y",		ARG_COPY_R);
			break;
		}
		case AlgOpNode::refeq: {
			/*BASE*/
			addTcRule(			"ref",		"ref",	"boolean");
			addTcRule(M_ELSE,						TC_RS_ERROR);
			/*CARD*/
			addCdRule(			"1..1",		"1..1",		"1..1");
			addCdRule(M_L,		"",			"1..1", 	"1..1", 	CARD_TO_11, LEFT, DYNAMIC);
			addCdRule(M_R,		"1..1",		"", 		"1..1", 	CARD_TO_11, RIGHT, DYNAMIC);
			addCdRule(M_B,		"", 		"", 		"1..1", 	CARD_TO_11, BOTH, DYNAMIC);
			/*T-NAME*/
			addTnRule(M_B,		M_0,		M_0,		"");
			addTnRule(M_EQ,		"",			"",			"");
			addTnRule(M_ELSE, TC_RS_ERROR);
			break;
		}	//arguments must be references, else - error
		default: break;
		}
		provideReturnLinks();
	}

	void DecisionTable::initNonAlgOpRules(int op) {
		//cout << "[TC]: " << "initint NONAlgOpRules: " << op << " operator." << endl;
		switch (op) {//TODO: fill the rulesets
		case NonAlgOpNode::dot: {
/*BASE*/
			addTcRule(M_EX,		"variant",	"",			TC_RS_ERROR);
			addTcRule(M_B,		"X",		"Y", 		BS_COPY_R);
/*CARD*/
			addCdRule(M_B,		"X",		"Y",		CD_MULTBANDS);
/*T-NAME*/
			addTnRule(M_B,		"X",		"Y",		ARG_COPY_R);
/*C-KIND*/
			addCkRule(M_B,		M_0,		M_0, 		"bag");
			addCkRule(M_L,		M_0,		"bag",		"bag");
			addCkRule(M_R,		"bag",		M_0,		"bag");
			addCkRule(			"bag", 		"bag",		"bag");
			addCkRule(M_L,		M_0,		"list",		"list");
			addCkRule(M_R,		"list",		M_0,		"list");
			addCkRule(			"list",		"list",		"list");
			addCkRule(M_ELSE, TC_RS_ERROR);
			break;
		}
		case NonAlgOpNode::join: { //same rules as in struct, collKind taken from dot...
/*BASE*/	addTcRule(M_EX,		"variant",	"",			TC_RS_ERROR);
			addTcRule(M_B,		"X",		"Y",		BS_STRUCT);
/*CARD*/
			addCdRule(M_B,		"X",		"Y",		CD_MULTBANDS);
/*T-NAME*/
			addTnRule(M_B,		"X",		"Y",		"");	//empty distinct typename for struct
/*C-KIND*/
			addCkRule(M_B,		M_0,		M_0,		"bag");
			addCkRule(M_L,		M_0,		"bag",		"bag");
			addCkRule(M_R,		"bag",		M_0,		"bag");
			addCkRule(			"bag",		"bag",		"bag");
			addCkRule(M_ELSE, TC_RS_ERROR);
			break;
		}
		case NonAlgOpNode::where: {
/*BASE*/
			addTcRule(M_EX,		"variant",	"",			TC_RS_ERROR);
			addTcRule(M_L,		"X",		"boolean",	BS_COPY_L);
			//One could try and coerce right arg to boolean dynamically. Such coerce node exists and can be used.
			//addTcRule(M_B,	"X",		"Y",		BS_COPY_L,		BS_TO_BOOL, RIGHT, DYNAMIC);
			addTcRule(M_ELSE, TC_RS_ERROR);
/*CARD*/
			addCdRule(M_L,		"X..Y",		"1..1",		CD_COPY_L_ZR);
			addCdRule(M_ELSE, 	"X..Y",		"a..b",		CD_COPY_L_ZR,	CARD_TO_11, RIGHT, DYNAMIC);
/*T-NAME*/
			addTnRule(M_B,		"X",		"Y",		ARG_COPY_L);
/*C-KIND*/
			addCkRule(M_B,		"X",		"Y",		ARG_COPY_L);
			break;
		}
		//Four similar operators, as for TC...
		case NonAlgOpNode::closeBy: 		// BELOW
		case NonAlgOpNode::closeUniqueBy: 	// BELOW
		case NonAlgOpNode::leavesBy: 		// BELOW
		case NonAlgOpNode::leavesUniqueBy: { 	//as in union...
			/*BASE*/
			addTcRule(M_EX,		"variant",	"",			TC_RS_ERROR);
			addTcRule(M_B,		"X",		"Y",		BS_COPY_L);
			/*CARD*/
			addCdRule(M_B,		"X",		"Y",		CD_ADDBANDS);
			/*T-NAME*/
			addTnRule(M_B,		M_0, 		M_0, 		"");
			addTnRule(M_EQ,		"X",		"X",		"");
			addTnRule(M_ELSE, 							TC_RS_ERROR);
			/*C-KIND*/
			addCkRule(M_B,		M_0,		M_0,		"bag");
			addCkRule(M_L,		M_0,		"bag",		"bag");
			addCkRule(M_R,		"bag",		M_0,		"bag");
			addCkRule(			"bag",		"bag",		"bag");
			addCkRule(M_ELSE,							TC_RS_ERROR);
			break;
		}
		case NonAlgOpNode::orderBy: {
			addTcRule(M_EX,		"variant",	"",			TC_RS_ERROR);
			addTcRule(M_B,		"X",		"Y",		BS_COPY_L);
			addCdRule(M_B,		"X",		"Y",		ARG_COPY_L);
			addTnRule(M_B,		"X", 		M_0, 		ARG_COPY_L);
			addTnRule(M_ELSE, TC_RS_ERROR);
			break;
		}
		case NonAlgOpNode::exists: //same as forAll..: BELOW
		case NonAlgOpNode::forAll: {
			/*BASE*/
			addTcRule(M_EX,		"variant",	"",			TC_RS_ERROR);
			addTcRule(M_L,		"X",		"boolean",	"boolean");
			addTcRule(M_ELSE, TC_RS_ERROR);
			/*CARD*/
			addCdRule(M_L,		"X..Y",		"1..1",		"1..1");
			addCdRule(M_ELSE, 	"X..Y",		"a..b",		"1..1",			CARD_TO_11, RIGHT, DYNAMIC);

			break;
		}
		case NonAlgOpNode::forEach: {
			/*BASE*/
			addTcRule(M_EX,		"variant",	"",			TC_RS_ERROR);
			addTcRule(M_B,		"X",		"Y", 		"void");
			/*CARD*/
			addCdRule(M_B,		"X",		"Y",		ARG_COPY_L);
			/*T-NAME*/
			addTnRule(M_B,		"X",		"Y",		"");
			/*C-KIND*/
			addCkRule(M_B,		"",			"", 		ARG_COPY_L);
			break;
		}
		default: break;
		}
		provideReturnLinks();
	}

	void DecisionTable::initOtherBinaryRules(int treeType, int op) {
		switch (treeType) {
		case TreeNode::TNCASTTO : {
			/*BASE*/
			addTcRule(M_EQ,		"X",		"X",		BS_COPY_L); //"boolean", "boolean", "boolean");
			addTcRule(M_R,		"variant",	"X",		BS_CAST_VAR);
			addTcRule(M_R,		"struct",	"X",		BS_CAST_VAR);
			addTcRule(M_ELSE, TC_RS_ERROR);
			/*CARD*/
			addCdRule(M_B,		"",			"",			ARG_COPY_L);
			/*T-NAME*/
			addTnRule(M_B, 		"X",		"whatever", ARG_COPY_R);	//may set typename by such a cast...
			/*C-KIND*/
			addCkRule(M_B,		"X",		"whatever", ARG_COPY_L);
		}
		default: break;
		}
		provideReturnLinks();
	}

	void UnOpDecisionTable::initUnOpRules(int treeType, int op) {
		switch (treeType) {
		case TreeNode::TNUNOP :
			switch (op) {
			case UnOpNode::count: { //Alway successful, can even count variant.- returns 1 integer, no t-name, no c-kind
				addTcRule(META,		"x",		"integer");
				addCdRule(META,		"x",		"1..1");
				break;
			}
			case UnOpNode::sum: 	// AS BELOW	 //lets say you can sum all that can have '+' applied
			case UnOpNode::avg:	{
				addTcRule("",		"integer",	"integer");
				addTcRule("",		"double",	"double");
				addTcRule("",		"string",	"string");
				addTcRule(M_ELSE,	"",			TC_RS_ERROR);
				/*CARD*/
				addCdRule(META,		"",			"1..1");
				/*T-NAME*/
				addTnRule(META,		M_0,		"");
				addTnRule(M_ELSE, 	"",			TC_RS_ERROR);
				break;
			}
			case UnOpNode::min: 	// AS BELOW
			case UnOpNode::max: {
				addTcRule("",		"integer",	"integer");
				addTcRule("",		"double",	"double");
				addTcRule("",		"string",	"string");
				addTcRule("",		"boolean",	"boolean"); 	//works like grouped OR for max, AND for min.
				addTcRule(M_ELSE, 	"",			TC_RS_ERROR);
				/*CARD*/
				addCdRule(META,		"",			"1..1");
				/*T-NAME*/
				addTnRule(META,		M_0,		"");
				addTnRule(M_ELSE, 	"",			TC_RS_ERROR);
				break;
			}
			case UnOpNode::deref:{
				addTcRule("",		"ref",		BS_REF_DEREF);	//deprecated - now can be within Else-> BS_DEREF..
				addTcRule("",		"string",	"string");
				addTcRule("",		"integer",	"integer");
				addTcRule("",		"double",	"double");
				addTcRule("",		"boolean",	"boolean");
				addTcRule(M_ELSE, 	"", 		BS_DEREF);
				addCdRule(META, 	"x", 		ARG_COPY);
				//addTnRule(META, 	"x", 		ARG_COPY);
				addCkRule(META, 	"x", 		ARG_COPY);
				break;
			}
			case UnOpNode::ref: {	// copies argument, just setting the 'ref' flag.
				addTcRule("",		"variant",	TC_RS_ERROR);
				addTcRule(META,		"",			BS_COPY_REF);
				addCdRule(META, 	"x", 		ARG_COPY);
				addTnRule(META, 	"x", 		ARG_COPY);
				addCkRule(META, 	"x", 		ARG_COPY);
				break;
			}
			case UnOpNode::distinct: {
				addTcRule("",		"variant",	TC_RS_ERROR);
				addTcRule(META,		"",			BS_COPY_REF);
				addCdRule(META, 	"x", 		ARG_COPY);	//nr of elts may change, but card stays as it is !
				addTnRule(META, 	"x", 		ARG_COPY);	//elements can keep the typename.
				addCkRule(META, 	M_0, 		"");
				break;
			}
			case UnOpNode::unMinus: {
				addTcRule("",		"integer",	"integer");
				addTcRule("",		"double",	"double");
				//addTcRule("",		"boolean",	"boolean");		// could work like NOT
				addTcRule("",		"string",	"double",	BS_TO_DBL, DYNAMIC);
				addTcRule(M_ELSE,	"",			TC_RS_ERROR);
				/*CARD*/
				addCdRule("",		"1..1",		"1..1");
				addCdRule(META,		"",			"1..1",		CARD_TO_11, DYNAMIC);
				/*T-NAME*/
				addTnRule(META,		M_0,		"");
				addTnRule(M_ELSE, 	"",			TC_RS_ERROR);
				break;
			}
			case UnOpNode::boolNot: {
				addTcRule("",		"boolean",	"boolean");
				addTcRule("",		"integer",	"boolean",	BS_TO_BOOL, DYNAMIC);
				addTcRule("",		"double",	"boolean",	BS_TO_BOOL, DYNAMIC);
				addTcRule("",		"string",	"boolean",	BS_TO_BOOL, DYNAMIC);
				addTcRule(M_ELSE,	"",			TC_RS_ERROR);
				/*CARD*/
				addCdRule("",		"1..1",		"1..1");
				addCdRule(META,		"",			"1..1",		CARD_TO_11, DYNAMIC);
				/*T-NAME*/
				addTnRule(META,		M_0,		"");
				addTnRule(M_ELSE, 	"",			TC_RS_ERROR);
				break;
			}
			case UnOpNode::deleteOp: {
				//this could be done this standard way either thanks to A) some
				//pre-processing in TypeChecker::typeCheck() method, or B) a trick in the BS_DELETE generator.
				//B) is chosen here: it actually overrides the card attr. with card of the sig from datascheme!
				addTcRule("",		"ref",		BS_DELETE); 	//only refs may be removed.
				addTcRule(M_ELSE,	"",			TC_RS_ERROR);
				addCdRule("",		"1..1",		TC_RS_ERROR);	//(See comment above) delete forbidden if card == 1..1
				addCdRule("",		"1..*",		"1..1",		CD_CAN_DEL, DYNAMIC);	//check that >= 1 would be left.
				addCdRule(M_ELSE,	"",			"1..1");	//if card is [0..x] than delete allowed.
				break;
			}
			default: break;
			}
			break;
		case TreeNode::TNAS: {
				addTcRule("",		"variant",	TC_RS_ERROR);
				addTcRule(META,		"Base", 	BS_NAMEAS);
				addCdRule(META,		"CD",		CD_NAMEAS);
				addTnRule(META,		"TN",		"");
				addCkRule(META,		"CK",		CK_NAMEAS);
				break;
		}
		case TreeNode::TNCREATE: {
				// TODO: fill in together with AlgOp::insert, most responsibility on base-generator.
				//base : argument has to be a binder, & fulfill other reqs.
				addTcRule("",		"binder",	BS_CREATE);	//returns sigref - to the created object.
				addTcRule(M_ELSE,	"",			TC_RS_ERROR);
				addCdRule(META,		"CD",		ARG_COPY);	//may create a bunch of objects at once.
				addCkRule(META,		"CK",		ARG_COPY);
			break;
		}
		default: break;
		}
		provideReturnLinks();
	}


	/** *********************************************************** */

	/** ***********		Result generator dispatchers. 	*********** */

	Signature *DecisionTable::doSig(int method, Signature *lSig, Signature *rSig, TypeCheckResult &retResult) {
		// << "doing sig based on lSig and rSig: " << endl;	lSig->putToString();	rSig->putToString();
		switch(method) {
			case DecisionTable::BS_COPY_L : return leftSig(lSig, rSig);
			case DecisionTable::BS_COPY_R : return rightSig(lSig, rSig);
			case DecisionTable::BS_STRUCT : return doStruct(lSig, rSig);
			case DecisionTable::BS_CAST_VAR: return castVar(lSig, rSig);
			case DecisionTable::BS_TO_VAR: return createVar(lSig, rSig);
			case DecisionTable::BS_INS_REF: return insertRefCheck(lSig, rSig, retResult);
			case DecisionTable::BS_INS_BND: return insertBndCheck(lSig, rSig, retResult);
			case DecisionTable::BS_ASSIGN : return assignCheck(lSig, rSig, retResult);

			default: return NULL;
		}
	}

	string DecisionTable::doAttr(int method, string lArg, string rArg) {
		switch(method) {
			case DecisionTable::CD_ADDBANDS : return cdAddBands(lArg, rArg);
			case DecisionTable::CD_MULTBANDS : return cdMultiplyBands(lArg, rArg);
			case DecisionTable::ARG_COPY_L : return copyLeftArg(lArg, rArg);
			case DecisionTable::CD_COPY_L_ZR : return cdCopyLeftWithZero(lArg, rArg);
			case DecisionTable::ARG_COPY_R : return copyRightArg(lArg, rArg);
			case DecisionTable::CD_INTST : return cdIntersect(lArg, rArg);

			default: return "";
		}
	}

	Signature *UnOpDecisionTable::doSig(int method, Signature *sig, string param, int option, TypeCheckResult &retResult) {
		switch(method) {
			case UnOpDecisionTable::BS_COPY : return copySig(sig);
			case UnOpDecisionTable::BS_REF_DEREF : return refDerefSig(sig);
			case UnOpDecisionTable::BS_DEREF : return derefSig(sig);
			case UnOpDecisionTable::BS_COPY_REF : return copyAndRefSig(sig);
			case UnOpDecisionTable::BS_NAMEAS : return nameAsSig(sig, param, option);
			case UnOpDecisionTable::BS_DELETE : return deleteCheck(sig, retResult);
			case UnOpDecisionTable::BS_CREATE : return createCheck(sig, retResult);

			default: return NULL;
		}
	}

	string UnOpDecisionTable::doAttr(int method, string arg, string param, int option) {
		switch(method) {
			case UnOpDecisionTable::CD_NAMEAS : return cdNameAs(arg, option);
			case UnOpDecisionTable::CK_NAMEAS : return ckNameAs(arg, option);
			case UnOpDecisionTable::ARG_COPY : return arg;
			default: return "";
		}
	}

	/** ***************		Result and attribute generators.. 	****************/
// DecisionTables
	Signature *DecisionTable::leftSig(Signature *lSig, Signature *rSig) {return lSig->clone();}

	Signature *DecisionTable::rightSig(Signature *lSig, Signature *rSig) { return rSig->clone();}

	Signature *DecisionTable::doStruct(Signature *lSig, Signature *rSig) {
		//RETURN: STRUCT( lSig[card=1..1], rSig[card=1..1] )...
		//Could flatten the structure here, if lSig or rSig were structs..
		Signature *sig = new SigColl(Signature::SSTRUCT);
		lSig->setCard("1..1");
		rSig->setCard("1..1");
		((SigColl *)sig)->setElts(lSig);
		((SigColl *)sig)->addToMyList(rSig);
		return sig;
	}

	//to be used for casting variant subSignatures...
	Signature *DecisionTable::castVar(Signature *lSig, Signature *rSig) {
		if (lSig->isStructurallyEqualTo(rSig)) {
			//cout << "Structurally equal !!! " << endl;
			return lSig->clone();
		} else {
			//cout << "lSig isnt struct. equal to rSig. lets check if its a variant and has at least 1 matching sig\n";
			//if (lSig->type() != Signature::SVAR) {cout << "NOT a var sig\n";}
			if (lSig->type() == Signature::SSTRUCT || lSig->type() == Signature::SVAR) {
				//cout << "Struct or var sig - works the same, will check out on it\n";
				bool foundMatch = false;
				Signature *ptr = ((SigColl *)lSig)->getMyList();
				if (ptr != NULL) {
					foundMatch = ptr->isSolelyEqualTo(rSig);
				}
				while ((ptr->getNext() != NULL) && (not foundMatch)) {
					ptr = ptr->getNext();
					foundMatch = ptr->isSolelyEqualTo(rSig);
				}
				if (foundMatch) return rSig->clone();
				else return NULL;
			}
		}
		cout << "castVar seems not allowed, will return NULL as result signature..." << endl;
		return NULL;
	}

	Signature *DecisionTable::createVar(Signature *lSig, Signature *rSig) {
		if (lSig == NULL || rSig == NULL) return NULL;
		SigColl *ret = new SigColl(Signature::SVAR);
		if (lSig->type() != Signature::SVAR) ret->addToMyList(lSig->clone());
		else addVariants(ret, (SigColl *)lSig);
		if (rSig->type() != Signature::SVAR) ret->addToMyList(rSig->clone());
		else addVariants(ret, (SigColl *)rSig);
		return ret;
	}

	void DecisionTable::addVariants(SigColl *sig, SigColl *tbAdded) {
		Signature *ptr = tbAdded->getMyList();
		//now - clone also clones all the nexts! so no need for a 'while' loop.
		if (ptr != NULL) {
			sig->addToMyList(ptr->clone());
			sig->setMyLast(sig->getMyList());
		}
	}

	string DecisionTable::cdAddBands(string lCard, string rCard) {
		stringstream conv;
		//in the code below zeros are subtracted in order to cast (convert) chars to integers.
		if (((lCard[0]-'0')+ (rCard[0]-'0')) > 1) conv << "1";
		else conv << ((lCard[0]-'0') + (rCard[0]-'0'));
		conv << "..";
		if (lCard[3] == '*' || rCard[3] == '*') conv << "*";
		else {
			int rb = ((lCard[3]-'0') + (rCard[3]-'0'));
			if (rb > 1) conv << "*";
			else conv << rb;
		}
		return conv.str();
	}

	//Takes left card, replaces left bound with zero, and copies right bound...
	string DecisionTable::cdCopyLeftWithZero(string lCard, string rCard) {
		stringstream conv;
		conv << "0.." << lCard[3];
		return conv.str();
	}

	string DecisionTable::cdMultiplyBands(string lCard, string rCard) {
		stringstream conv;
		if (lCard[0] == '0' || rCard[0] == '0') conv << "0";
		else if (lCard[0] == '*' || rCard[0] == '*') conv << "*";
		else conv << ((lCard[0] - '0') * (rCard[0] - '0'));
		conv << "..";
		if (lCard[3] == '0' || rCard[3] == '0') conv << "0";
		else if (lCard[3] == '*' || rCard[3] == '*') conv << "*";
		else conv << ((lCard[3] - '0') * (rCard[3] - '0'));
		return conv.str();
	}

	string DecisionTable::cdIntersect(string lCard, string rCard) {
		stringstream conv;
		conv << "0..";
		if (lCard[3] == '*' && rCard[3] == '*') conv << "*";
		else if (lCard[3] == '0' || rCard[3] == '0') conv << "0";
		else conv << "1";
		return conv.str();
	}

	string DecisionTable::copyLeftArg(string lArg, string rArg) {return lArg;}

	string DecisionTable::copyRightArg(string lArg, string rArg) {return rArg;}

//UnOpDecisionTables

	Signature *UnOpDecisionTable::nameAsSig(Signature *sig, string name, int grouped) {
		if (grouped == 0) sig->setCard("1..1");
		Signature *resultSig = new StatBinder(name, sig);
		return resultSig;
	}

	Signature *UnOpDecisionTable::refDerefSig(Signature *sig) {
		if (!sig->isRefed()) return DataScheme::dScheme(-1)->signatureOfRef(((SigRef *)sig)->getRefNo());
		else return sig->cloneSole();
	}

	Signature *UnOpDecisionTable::derefSig(Signature *sig) {
		if (sig == NULL) return NULL;
		if (sig->isRefed()) return sig->cloneSole();
		return sig->deref();
	}

	string UnOpDecisionTable::cdNameAs(string arg, int grouped) {
		return (grouped == 1 ? "1..1" : arg);
	}

	string UnOpDecisionTable::ckNameAs(string arg, int grouped) {
		return (grouped == 1 ? "" : arg);
	}

	Signature *UnOpDecisionTable::copySig(Signature *sig) {return sig->clone();}

	Signature *UnOpDecisionTable::copyAndRefSig(Signature *sig) {Signature *s = sig->clone(); s->setRefed(); return s;}


	Signature *UnOpDecisionTable::deleteCheck(Signature *sig, TypeCheckResult &retResult) {
		//just set sig's card attribute to what you find in datascheme - we really know sig is a SigRef.
		if (sig->type() != Signature::SREF) return NULL;
		DataObjectDef *obj = DataScheme::dScheme(-1)->getObjById(((SigRef *)sig)->getRefNo());
		if (obj == NULL) return NULL;
		//trick below - its the Objects cardinality that matters, not sigs real card.
		sig->setCard(obj->getCard());
		return new SigVoid();	//delete just returns void.
	}

	Signature *UnOpDecisionTable::createCheck(Signature *sig, TypeCheckResult &retResult) {
		int newObjId = 0;
		if (sig->type() != Signature::SBINDER) return NULL;
		//Signature *valSig = ((StatBinder *) sig)->getValue();
		string name = ((StatBinder *) sig)->getName();

		BinderWrap *bwObj = DataScheme::dScheme(-1)->bindBaseObjects();
		cout << "[TC] DTable::createCheck(): bound base objects" << endl;
		if (bwObj == NULL) {cout << "no base objects\n";
			retResult.setEffect(TC_RS_ERROR);
			retResult.addErrorPart("Name '" + name + "' could not be bound\n	");
			return NULL;
		}
		vector<BinderWrap*> *vec = new vector<BinderWrap*>();
		bwObj->bindName2(name, vec);
		if (DataScheme::bindError(vec)) {//no object with this name...
			cout << "[TC] DTable::createCheck(): name:  "<< name << " not found in roots, can't create such object\n";
			retResult.setEffect(TC_RS_ERROR);
			retResult.addErrorPart("Name '" + name + "' not found\n	");
			return NULL;
		}
		Signature *bSig = DataScheme::extractSigFromBindVector(vec);
		if (bSig == NULL || bSig->type() != Signature::SREF) {
			cout << "[TC] DTable::createCheck(): name not bound properly: sig is null or not a ref.\n";
			retResult.setEffect(TC_RS_ERROR);
			retResult.addErrorPart("Name '"+ name + "' not bound properly\n	");
			return NULL;
		}
		//check if we're not creating to many objects..
		char maxAlwd = (bSig->getCard())[3];
		char maxCrtd = (sig->getCard())[3];
		//such condition as below would not suffice: we don't know how many such objects already exist...
		//if (maxAlwd != '*' && (maxCrtd == '*' || (maxAlwd-'0') < (maxCrtd-'0'))) {
		if (maxAlwd != '*' && maxCrtd != '0') {
			retResult.setEffect(TC_RS_COERCE);
			retResult.addActionId(CD_EXT_CRT, SINGLE);
		}
		newObjId = ((SigRef *) bSig)->getRefNo();
		Signature *fullBSig = DataScheme::dScheme(-1)->namedSignatureOfRef(newObjId);
		Signature *flatSig = sig->cloneFlat();
		if (fullBSig == NULL || flatSig == NULL || ((StatBinder *) flatSig)->getValue() == NULL) {
			return NULL; //bad arg for create..
		}

		cout << "[TC] comparing " << flatSig->toString() << " \nto sig from datascheme: \n" << fullBSig->toString() << ".\n";
		int comparison = fullBSig->compareNamedSigCrt(flatSig, false);
		ErrorConsole ec("TypeChecker");
		ec << "[TC] result of comparing the 2 signatures:";
		ec << comparison;
		if (comparison == 0) {
			cout << "CREATE comparison worked out, 0 result !" << endl;
			return bSig; //reference to created object
		} else if (comparison == (ErrTypeChecker | ESigCdDynamic)) {
			cout << "DYNAMIC CHECKING !!! have to return it somehow, so that it can be performed...\n";
			cout <<"OK, now we've got retResult, so may set coerce (with coerce action and MARK_NODE parm)\n";
			retResult.setEffect(TC_RS_COERCE);
			retResult.addActionId(CD_CAN_CRT, MARK_NODE);
			retResult.setCoerceSig(fullBSig);
			return bSig;
		}
		cout <<"[TC] - create check failed, so will return null, which will trigger TCError\n";
		retResult.setEffect(TC_RS_ERROR);
		retResult.addErrorPart(SBQLstrerror(comparison));
		return NULL;
	}

	Signature *DecisionTable::assignCheck(Signature *lSig, Signature *rSig, TypeCheckResult &retResult) {
		if (lSig == NULL || rSig == NULL) return NULL;
		if (lSig->type() != Signature::SREF) {cout << "[TC] DTable::assignCheck(): base sig not a ref\n"; return NULL;}
		int leftId = ((SigRef *) lSig)->getRefNo();
		Signature *namedLSig = DataScheme::dScheme(-1)->namedSignatureOfRef(leftId);
		if (namedLSig == NULL || namedLSig->type() != Signature::SBINDER || ((StatBinder *)namedLSig)->getValue() == NULL)
			return NULL;
		//now make rSig a binder with fRSigs name..
		Signature *fRSig = NULL;
		if (rSig->type() == Signature::SREF && !rSig->isRefed()) {
			fRSig = DataScheme::dScheme(-1)->namedSignatureOfRef(((SigRef *)rSig)->getRefNo());
			if (fRSig == NULL) return NULL;
			((StatBinder *)fRSig)->setName(((StatBinder *)namedLSig)->getName());
		} else { //atom or struct or binder or (sigRef which was refed)
			Signature *rVal = rSig->cloneFlat();
			if (rVal == NULL || (rVal->type() == Signature::SBINDER && ((StatBinder *)rVal)->getValue() == NULL)) {
				return NULL;
			}
			if (rVal->type() == Signature::SBINDER) {
				fRSig = rVal;
			} else {
				fRSig = new StatBinder(((StatBinder *)namedLSig)->getName(), rVal);
			}
		}
		//now check that fRSig suits namedLSig well... (exactly like in createCheck..)
		cout << "[TC]SDDIGN:  comparing base sig:" << namedLSig->toString() << " \nto assigned: \n" << fRSig->toString() << ".\n";
		int comparison = namedLSig->compareNamedSigCrt(fRSig, true);
		ErrorConsole ec("TypeChecker");
		ec << "[TC] result of comparing the 2 signatures for assignment:";
		ec << comparison;
		if (comparison == 0) {	//SUCCESS
			cout << "ASSIGN comparison worked out, 0 result !" << endl;
			return new SigVoid();
		} else if (comparison == (ErrTypeChecker | ESigCdDynamic)) {
			cout << "ASSIGN needs DYNAMIC CHECKING !!! for sub cardinalities.\n";
			retResult.setEffect(TC_RS_COERCE);
			retResult.addActionId(CD_CAN_ASGN, MARK_NODE);
			retResult.setCoerceSig(namedLSig);
			return new SigVoid();
		}
		cout <<"[TC] - assign check failed, so will return null, and trigger TCError\n";
		retResult.setEffect(TC_RS_ERROR);
		retResult.addErrorPart(SBQLstrerror(comparison) + "\n	bases");
		return NULL;	//on error
	}


	Signature *DecisionTable::insertCheck(Signature *lSig, Signature *flatRSig, TypeCheckResult &retResult) {
		//flatRSig is a not-null, flattened, named binder signature.
		if (lSig == NULL || lSig->type() != Signature::SREF) return NULL;

		int leftId = ((SigRef *) lSig)->getRefNo();
		Signature *namedLSig = DataScheme::dScheme(-1)->namedSignatureOfRef(leftId);

		if (namedLSig == NULL || namedLSig->type() != Signature::SBINDER || ((StatBinder *)namedLSig)->getValue() == NULL)
			return NULL;
		string outerName = ((StatBinder *)namedLSig)->getName();
		Signature *outerSig = ((StatBinder *)namedLSig)->getValue();
		if (outerSig->type() != Signature::SSTRUCT) {
			retResult.setEffect(TC_RS_ERROR);
			retResult.addErrorPart("Left argument does not point to a complex object definition\n	bases");
			return NULL;
		}
		SigColl *bSig = (SigColl *) outerSig;
		Signature *bPtr = bSig->getMyList();
		string rName = ((StatBinder *) flatRSig)->getName();
		bool foundName = false;
		Signature *match = NULL;
		while (bPtr != NULL && not foundName) {
			if (((StatBinder *) bPtr)->getName() == rName) {
				foundName = true;
				match = bPtr;
			}
			bPtr = bPtr->getNext();
		}
		if (not foundName || match == NULL) {
			retResult.setEffect(TC_RS_ERROR);
			retResult.addErrorPart("No '"+rName+"' subobjects in declaration of "+outerName+".\n	bases");
			return NULL;
		}
		cout << "[TC] comparing chosen sub from DSCHEME sig: \n" << match->toString() << " \nto sig to be inserted: \n" << flatRSig->toString() << ".\n";
		int comparison = match->compareNamedSigCrt(flatRSig, true);
		ErrorConsole ec("TypeChecker");
		ec << "[TC] result of comparing a chosen sub of left sig, with the right sig, for insert:";
		ec << comparison;
		if (comparison == 0) {//SUCCESS - just check the cardinalities.
			cout << "INSERT comparison worked out, 0 result ! Will just check ext cards." << endl;
			if (match->getCard() == "1..1") {
				retResult.setEffect(TC_RS_ERROR);
				retResult.addErrorPart("Only one '"+rName+"' subobject allowed in "+outerName+"\n	bases");
				return NULL;
			}
			if (match->getCard() == "0..1") {
				cout <<"Marking insert node to be checked dynamically for presence of '"+rName+"' subobjects.\n";
				retResult.setEffect(TC_RS_COERCE);
				retResult.addActionId(CD_EXT_INS, MARK_NODE);
			}
			return new SigVoid();
		} else if (comparison == (ErrTypeChecker | ESigCdDynamic)) {
			cout << "ASSIGN needs DYNAMIC CHECKING !!! for sub cardinalities.\n";
			retResult.setEffect(TC_RS_COERCE);
			retResult.addActionId(CD_CAN_INS, MARK_NODE);
			retResult.setCoerceSig(match);
			return new SigVoid();
		}
		cout <<"[TC] - insert check on chosen sub failed, so will return null, and trigger TCError\n";
		retResult.setEffect(TC_RS_ERROR);
		retResult.addErrorPart(SBQLstrerror(comparison) + "\n	bases");
		return NULL;
	}

	Signature *DecisionTable::insertRefCheck(Signature *lSig, Signature *rSig, TypeCheckResult &retResult) {
		if (rSig == NULL || rSig->type() != Signature::SREF) return NULL;
		Signature *flatSig = NULL;
		int refNo = ((SigRef *)rSig)->getRefNo();
		//check its a root
		DataObjectDef *obd = DataScheme::dScheme(-1)->getObjById(refNo);
		if (obd == NULL || not (obd->getIsBase()) || obd->getIsTypedef()) {
			retResult.setEffect(TC_RS_ERROR);
			retResult.addErrorPart("Right argument does not point to a root object\n	bases");
			return NULL;
		}
		//check card
		if (obd->getCard() == "1..1") {
			retResult.setEffect(TC_RS_ERROR);
			retResult.addErrorPart("Cannot delete single root pointed by right arg.\n	cards");
			return NULL;
		}
		if (obd->getCard() == "1..*") {
			retResult.setEffect(TC_RS_COERCE);
			retResult.addActionId(CD_CAN_DEL, RIGHT);
		}

		flatSig = obd->createNamedSignature();
		if (flatSig == NULL || flatSig->type() != Signature::SBINDER || ((StatBinder *) flatSig)->getValue() == NULL)
			return NULL; //bad right arg for insert..
		if (Deb::ugOn()) cout << "Created NAMED signature from dScheme: \n" << flatSig->toString() << endl;

		return insertCheck(lSig, flatSig, retResult);
	}

	Signature *DecisionTable::insertBndCheck(Signature *lSig, Signature *rSig, TypeCheckResult &retResult) {
		if (rSig == NULL || rSig->type() != Signature::SBINDER) return NULL;
		Signature *flatSig = rSig->cloneFlat();
		if (flatSig == NULL || flatSig->type() != Signature::SBINDER || ((StatBinder *) flatSig)->getValue() == NULL)
			return NULL; //bad right arg for insert..

		return insertCheck(lSig, flatSig, retResult);
	}



	/** end of result generating methods..*/


	/** ***************		DecisionTable (bin) methods	   ******************** */

	DecisionTable::DecisionTable(int opType, int oper) {
		create(opType, oper);
	}

	void DecisionTable::create(int opType, int oper) {
		this->op = oper;
		this->algOrNonAlg = opType;
		switch (opType) {
			case TreeNode::TNALGOP : initAlgOpRules(op); break;
			case TreeNode::TNNONALGOP : initNonAlgOpRules(op); break;
			default: initOtherBinaryRules(opType, op); break;
		}
	}

	int DecisionTable::getResult(TypeCheckResult &finalResult, Signature *lSig, Signature *rSig) {
		int result = 0;
		result = applyRuleset(rules, SIG_BASE, lSig, rSig, lSig->textType(), rSig->textType(), finalResult);
		if (result == 0 && !finalResult.isError()) { //don't apply rules for attributes when the base is wrong.
			//return 'ETCNotApplicable' if no rules were given.(finalRes is SUCCESS, but has null signature);
			if (finalResult.getSig() == NULL) return (ErrTypeChecker | ETCNotApplicable);
			result += applyRuleset(cardRules, SIG_CARD, lSig, rSig, lSig->getCard(), rSig->getCard(), finalResult);
			result += applyRuleset(typeNameRules, SIG_TN, lSig, rSig, lSig->getTypeName(), rSig->getTypeName(), finalResult);
			result += applyRuleset(collKindRules, SIG_CK, lSig, rSig, lSig->getCollKind(), rSig->getCollKind(), finalResult);
		}
		return result;
	}


	int DecisionTable::applyRuleset (vector<TCRule> ruleSet, string atr, Signature *lSig, Signature *rSig, string leftArg, string rightArg, TypeCheckResult &finalResult) {
		cout << "[TC]: " << "APPLYING RULESET WITH 	" << ruleSet.size() << " rules..." << endl;
		for (unsigned int i = 0; i < ruleSet.size(); i++) {
			if (ruleSet.at(i).appliesTo(leftArg, rightArg) &&
			   (atr != SIG_BASE || ruleSet.at(i).appliesToBase(lSig, rSig))) {

				cout << "[TC]: " << " applied rule: nr "<< i << endl;
				ruleSet.at(i).coutMe();
				cout << "[TC]: gonna getResult of this rule" << endl;
				return ruleSet.at(i).getResult(atr, lSig, rSig, finalResult);
			} else {
				cout << "[TC]: " << " ....... REJECTED RULE :"; ruleSet.at(i).coutMe();
			}
		}
		if (ruleSet.size() == 0) return 0;
		return -1;
	}

	/** ***************		UnOpDecisionTable methods	   ******************** */

	void UnOpDecisionTable::create(int treeType, int oper) {
		op = oper;
		initUnOpRules(treeType, op);
	}

	int UnOpDecisionTable::applyRuleset (vector<UnOpRule> ruleSet, string atr, Signature *sig, string textArg, TypeCheckResult &finalResult, string param, int option) {
		cout << "[TC]: " << "APPLYING RULESET WITH 	" << ruleSet.size() << " rules..." << endl;
		for (unsigned int i = 0; i < ruleSet.size(); i++) {
			if (ruleSet.at(i).appliesTo(textArg)) {
				cout << "[TC]: " << " applied UNOP rule: nr "<< i << endl;
				ruleSet.at(i).coutMe();
				return ruleSet.at(i).getResult(atr, sig, finalResult, param, option);
			} else {
				cout << "[TC]: " << " ....... REJECTED RULE :"; ruleSet.at(i).coutMe();
			}
		}
		if (ruleSet.size() == 0) return 0;	//Empty ruleset - leaving default values for signature attributes.
		return -1;
	}

	int UnOpDecisionTable::getResult(TypeCheckResult &finalResult, Signature *sig) {
		return getResult(finalResult, sig, "", 0);
	}

	int UnOpDecisionTable::getResult(TypeCheckResult &finalResult, Signature *sig, string param, int option) {
		cout << "unOpT::getResult(finResult, sig,...);" << endl;
		int result = 0;
		result += applyRuleset(rules, SIG_BASE, sig, sig->textType(), finalResult, param, option);

		if (result == 0 && !finalResult.isError()) {
			if (finalResult.getSig() == NULL) return (ErrTypeChecker | ETCNotApplicable);
			result += applyRuleset(cardRules, SIG_CARD, sig, sig->getCard(), finalResult, param, option);
			result += applyRuleset(typeNameRules, SIG_TN, sig, sig->getTypeName(), finalResult, param, option);
			result += applyRuleset(collKindRules, SIG_CK, sig, sig->getCollKind(),finalResult, param, option);
		}
// 		cout << "applied all rulesets" << endl;
// 		cout << result;
// 		cout << "unOpT::getResult: ret int: <-- \n";
// 		cout << ", tcRes:\n" << finalResult.printAllInfo() << endl;
		return result;
	}


	/** ***************		DTable methods	   ******************** */
	/* :/ :\ :/ ugly design ... with class A : B- can't put A into vector< B >.. */
	void DTable::provideReturnLinks() {
		cout << "DOING UPPER CLASS PROV. RETURN LINKS, IS THIS CORRECT ? " << endl;
		provideReturnLinksForVector(getRules());
		provideReturnLinksForVector(getCardRules());
		provideReturnLinksForVector(getTypeNameRules());
		provideReturnLinksForVector(getCollKindRules());
	}

	void DTable::provideReturnLinksForVector(vector<Rule> v) {
		for (unsigned int i = 0; i < v.size(); i++) {
			v.at(i).setDTable(this);
		}
	}

	void DecisionTable::provideReturnLinks() {
		cout << "############################################" << endl;
		provideReturnLinksForVector(rules);
		provideReturnLinksForVector(cardRules);
		provideReturnLinksForVector(typeNameRules);
		provideReturnLinksForVector(collKindRules);
	}

	void UnOpDecisionTable::provideReturnLinks() {
		provideReturnLinksForVector(rules);
		provideReturnLinksForVector(cardRules);
		provideReturnLinksForVector(typeNameRules);
		provideReturnLinksForVector(collKindRules);
	}

	void DecisionTable::provideReturnLinksForVector(vector<TCRule> &v) {
		cout << "Providing links for vector: ";
		for (unsigned int i = 0; i < v.size(); i++) {
			cout << " " << i;
			v.at(i).setDTable(this);
		}
		cout << endl;
	}

	void UnOpDecisionTable::provideReturnLinksForVector(vector<UnOpRule> &v) {
		for (unsigned int i = 0; i < v.size(); i++) {
			v.at(i).setDTable(this);
		}
	}

	/** ***************	  DecisionTableHandler methods	******************** */

	DecisionTableHandler *DecisionTableHandler::dTableHandler = new DecisionTableHandler();
	DecisionTableHandler *DecisionTableHandler::getHandle() {
		return dTableHandler;
	}

	DecisionTable *DecisionTableHandler::getDTable(int algOrNonAlg, int op) {
		map<int, DecisionTable*> *tableMap;
		int place = op;
		switch (algOrNonAlg) {
			case TreeNode::TNALGOP : tableMap = &algOpDTables; break;
			case TreeNode::TNNONALGOP : tableMap = &nonAlgOpDTables; break;
			//a new tree node with many operators may need new tableMap
			default : tableMap = &otherBinaryDTables; place = (_TN_MAX_OP * algOrNonAlg) + op; break;
		}
		if ((*tableMap).find(place) == (*tableMap).end()) {
			(*tableMap)[place] = new DecisionTable(algOrNonAlg, op);
		}
		return (*tableMap)[place];
	}

	UnOpDecisionTable *DecisionTableHandler::getUnOpDTable(int treeType, int op) {
		map<int, UnOpDecisionTable*> *tableMap;
		int place = op;
		//cout << "place: " << place << endl;
		if (treeType == TreeNode::TNUNOP) tableMap = &unOpDTables;
		else {
			tableMap = &otherUnaryDTables;
			//cout << "place changed: " << place << endl;
			place = (_TN_MAX_OP * treeType) + op;
		}

		if ((*tableMap).find(place) == (*tableMap).end()) {
			//cout << "creating unopDTable(" << treeType << ", " << op << ") at " << place << "\n";
			(*tableMap)[place] = new UnOpDecisionTable(treeType, op);
		}
		return (*tableMap)[place];
	}

	DecisionTableHandler::~DecisionTableHandler(){
	//do I have to clear these maps by hand?
		unOpDTables.clear();
		algOpDTables.clear();
		nonAlgOpDTables.clear();
		otherBinaryDTables.clear();
		otherUnaryDTables.clear();
	}

	/** *********** 		Methods adding rules to decision tables 	***************************** */

	void DecisionTable::addTcRule (string lArg, string rArg, string result) {
		this->rules.push_back(TCRule(Rule::BASE, lArg, rArg, result));
	}
	void DecisionTable::addTcRule (string specArg, string lArg, string rArg, string result) {
		this->rules.push_back(TCRule(Rule::BASE, specArg, lArg, rArg, result, -1, -1, -1, STATIC));
	}
	void DecisionTable::addTcRule (string specArg, string lArg, string rArg, int resultGen) {
		this->rules.push_back(TCRule(Rule::BASE, specArg, lArg, rArg, "", resultGen, -1, -1, STATIC));
	}
	void DecisionTable::addTcRule (string specArg, string result) {
		this->rules.push_back(TCRule(Rule::BASE, specArg, "", "", result, -1, -1, -1, STATIC));
	}
	void DecisionTable::addTcRule (string lArg, string rArg, string result, int act, int actArg, int dynStat) {
		this->rules.push_back(TCRule(Rule::BASE, M_MATCH, lArg, rArg, result, -1, act, actArg, dynStat));
	}
	void DecisionTable::addTcRule (string specArg, string lArg, string rArg, string result, int act, int actArg, int dynStat) {
		this->rules.push_back(TCRule(Rule::BASE, specArg, lArg, rArg, result, -1, act, actArg, dynStat));
	}
	void DecisionTable::addTcRule (string specArg, string lArg, string rArg, int resultGen, int act, int actArg, int dynStat) {
		this->rules.push_back(TCRule(Rule::BASE, specArg, lArg, rArg, "", resultGen, act, actArg, dynStat));
	}
	/* ---------------------  CARD rules --------------------- */
	void DecisionTable::addCdRule (string lArg, string rArg, string result) {
		this->cardRules.push_back(TCRule(Rule::CARD, lArg, rArg, result));
	}
	void DecisionTable::addCdRule (string specArg, string lArg, string rArg, string result) {
		this->cardRules.push_back(TCRule(Rule::CARD, specArg, lArg, rArg, result, -1, -1, -1, STATIC));
	}
	void DecisionTable::addCdRule (string specArg, string lArg, string rArg, int resultGen) {
		this->cardRules.push_back(TCRule(Rule::CARD, specArg, lArg, rArg, "", resultGen, -1, -1, STATIC));
	}
	void DecisionTable::addCdRule (string specArg, string result){
		this->cardRules.push_back(TCRule(Rule::CARD, specArg, "", "", result, -1, -1, -1, STATIC));
	}
	void DecisionTable::addCdRule (string lArg, string rArg, string result, int act, int actArg, int dynStat) {
		this->cardRules.push_back(TCRule(Rule::CARD, M_MATCH, lArg, rArg, result, -1, act, actArg, dynStat));
	}
	void DecisionTable::addCdRule (string specArg, string lArg, string rArg, string reslt, int act, int actArg, int dynStat) {
		this->cardRules.push_back(TCRule(Rule::CARD, specArg, lArg, rArg, reslt, -1, act, actArg, dynStat));
	}
	void DecisionTable::addCdRule (string specArg, string lArg, string rArg, int resultGen, int act, int actArg, int dynStat) {
		this->cardRules.push_back(TCRule(Rule::CARD, specArg, lArg, rArg, "", resultGen, act, actArg, dynStat));
	}
	/* ---------------------  TYPENAME rules --------------------- */
	void DecisionTable::addTnRule (string lArg, string rArg, string result) {
		this->typeNameRules.push_back(TCRule(Rule::TYPENAME, lArg, rArg, result));
	}
	void DecisionTable::addTnRule (string specArg, string lArg, string rArg, string result) {
		this->typeNameRules.push_back(TCRule(Rule::TYPENAME, specArg, lArg, rArg, result, -1, -1, -1, STATIC));
	}
	void DecisionTable::addTnRule (string specArg, string lArg, string rArg, int resultGen) {
		this->typeNameRules.push_back(TCRule(Rule::TYPENAME, specArg, lArg, rArg, "", resultGen, -1, -1, STATIC));
	}
	void DecisionTable::addTnRule (string specArg, string result) {
		this->typeNameRules.push_back(TCRule(Rule::TYPENAME, specArg, "", "", result, -1, -1, -1, STATIC));
	}
	/* ---------------------  COLLECTION KIND  rules --------------------- */
	void DecisionTable::addCkRule (string lArg, string rArg, string result) {
		this->collKindRules.push_back(TCRule(Rule::COLLKIND, lArg, rArg, result));
	}
	void DecisionTable::addCkRule (string specArg, string lArg, string rArg, string result) {
		this->collKindRules.push_back(TCRule(Rule::COLLKIND, specArg, lArg, rArg, result, -1, -1, -1, STATIC));
	}
	void DecisionTable::addCkRule (string specArg, string lArg, string rArg, int resultGen) {
		this->collKindRules.push_back(TCRule(Rule::COLLKIND, specArg, lArg, rArg, "", resultGen, -1, -1, STATIC));
	}
	void DecisionTable::addCkRule (string specArg, string result) {
		this->collKindRules.push_back(TCRule(Rule::COLLKIND, specArg, "", "", result, -1, -1, -1, STATIC));
	}



//		void addTcRule (string arg, string result);
	void UnOpDecisionTable::addTcRule (string specArg, string arg, string result) {
		this->rules.push_back(UnOpRule(Rule::BASE, specArg, arg, result, -1, -1, STATIC));
	}
	void UnOpDecisionTable::addTcRule (string specArg, string arg, int resultGen) {
		this->rules.push_back(UnOpRule(Rule::BASE, specArg, arg, "", resultGen, -1, STATIC));
	}
	void UnOpDecisionTable::addTcRule (string specArg, string arg, string result, int act, int dynStat) {
		this->rules.push_back(UnOpRule(Rule::BASE, specArg, arg, result, -1, act, dynStat));
	}
	void UnOpDecisionTable::addTcRule (string specArg, string arg, int resultGen, int act, int dynStat) {
		this->rules.push_back(UnOpRule(Rule::BASE, specArg, arg, "", resultGen, act, dynStat));
	}



//		void addTcRule (string arg, string result, int action, string dynStat);
//		void addTcRule (string specArg, string arg, string result, int action, string dynStat);
//		void addTcRule (string specArg, string arg, int resultGen, int action, string dynStat);
//		void addCdRule (string arg, string result);
	void UnOpDecisionTable::addCdRule (string specArg, string arg, string result) {
		this->cardRules.push_back(UnOpRule(Rule::CARD, specArg, arg, result, -1, -1, STATIC));
	}
	void UnOpDecisionTable::addCdRule (string specArg, string arg, int resultGen) {
		this->cardRules.push_back(UnOpRule(Rule::CARD, specArg, arg, "", resultGen, -1, STATIC));
	}

	void UnOpDecisionTable::addCdRule (string specArg, string arg, string result, int act, int dynStat) {
		this->cardRules.push_back(UnOpRule(Rule::CARD, specArg, arg, result, -1, act, dynStat));
	}
	void UnOpDecisionTable::addCdRule (string specArg, string arg, int resultGen, int act, int dynStat) {
		this->cardRules.push_back(UnOpRule(Rule::CARD, specArg, arg, "", resultGen, act, dynStat));
	}
//		void addCdRule (string arg, string result, int action, string dynStat);
//		void addCdRule (string specArg, string arg, string result, int action, string dynStat);
//		void addCdRule (string specArg, string arg, int resultGen, int action, string dynStat);

//		void addTnRule (string arg, string result);
	void UnOpDecisionTable::addTnRule (string specArg, string arg, string result) {
		this->typeNameRules.push_back(UnOpRule(Rule::TYPENAME, specArg, arg, result, -1, -1, STATIC));
	}
	void UnOpDecisionTable::addTnRule (string specArg, string arg, int resultGen) {
		this->typeNameRules.push_back(UnOpRule(Rule::TYPENAME, specArg, arg, "", resultGen, -1, STATIC));
	}

//		void addCkRule (string arg, string result);
	void UnOpDecisionTable::addCkRule (string specArg, string arg, string result) {
		this->collKindRules.push_back(UnOpRule(Rule::COLLKIND, specArg, arg, result, -1, -1, STATIC));
	}
	void UnOpDecisionTable::addCkRule (string specArg, string arg, int resultGen) {
		this->collKindRules.push_back(UnOpRule(Rule::COLLKIND, specArg, arg, "", resultGen, -1, STATIC));
	}

	/** ************************************************************************ */

}

