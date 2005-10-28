#ifndef _QPARSER
#define _QPARSER
#include <string>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

class QueryTree
{

private:
    int nodesNum;
    QueryTree *lSon;
    QueryTree *rSon;
    
public:
    QueryTree() {};
    virtual ~QueryTree(){};
    int toString(){
	return 0;
    };

};

class QueryParser 
{

    protected:
    string myName;
    public:
    QueryParser() {};
    virtual ~QueryParser() {};
    int parseIt (string s, QueryTree *&qTrees);

};

#endif
