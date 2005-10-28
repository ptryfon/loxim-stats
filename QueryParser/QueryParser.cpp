#include<string>
#include<iostream>
#include<stdio.h>
#include "QueryParser.h"

using namespace std;



    int QueryParser::parseIt (string s, QueryTree * &qTree) {
	printf("Ja, PARSER, wczytalem takie cos...:\n\n %s \n\n",s.c_str());
//	printf("Jestem Parser i parsuje %s stringa!\n", s);
	return 0;    

    };


int main() {

    QueryParser prsr = QueryParser();
    QueryTree *costam;
    cout<<"zaraz sparsuje stringa\n";
    prsr.parseIt("JestemZapytanieStringowe", costam);
//    cout<<"juz cos zrobilem\n";
    delete [] costam;
    // jakis wpis do wcommitowania js209298    
}







