#include <iostream>
#include <string>
#include <sstream>
#include <FlexLexer.h>
#include "TreeNode.h"
#include "QueryParser.h"

yyFlexLexer* lexer;
extern QParser::TreeNode *d;
int yyparse();

using namespace std;

namespace QParser {

  TreeNode* QueryParser::parse(string query)
  {
    stringstream ss (stringstream::in | stringstream::out);
    ss << query;
    lexer = new yyFlexLexer(&ss); 
    yyparse();
    delete lexer;
    return d;
  }
  
}
