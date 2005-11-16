#ifndef _QUERY_PARSER_H_
#define _QUERY_PARSER_H_

#include <string>
#include "TreeNode.h"

using namespace std;

namespace QParser {

class QueryParser {
 public:
  virtual TreeNode* parse(string query);
  virtual ~QueryParser() {}
};

}

#endif
