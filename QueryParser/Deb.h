#ifndef _QP_DEBUG_H_
#define _QP_DEBUG_H_

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include "ClassNames.h"


using namespace std;

namespace QParser {


    class Deb 
    {
    protected:
	static bool writeDebugs;
    public:
	static void ug (const char *fmt, ...);  /* prints out the debug communicate*/
	static bool ugOn();			/* checks if debugMode is on */
	static void setWriteDebugs();
    };


}

#endif

