#include<string>
#include<iostream>
#include<stdio.h>
#include "QueryTree.h"

using namespace std;


class Parser {

    protected:
    int mojeImie;
    public:
    Parser() {};
    virtual ~Parser(){};
    virtual int parseIt (string s, QueryTree *qTrees) {
	printf("Ja, PARSER, wczytalem takie cos...:\n\n %s \n\n",s.c_str());
//	printf("Jestem Parser i parsuje %s stringa!\n", s);
	return 0;    

    };
    //virtual int funkcja_a (int a) = 0;

};




int main() {

    Parser prsr = Parser();
    QueryTree *costam = new QueryTree[10];
//    cout<<"zaraz sparsuje stringa\n";
    prsr.parseIt("JestemZapytanieStringowe", costam);
//    cout<<"juz cos zrobilem\n";
    delete [] costam;
    
}







