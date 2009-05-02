#include <QueryParser/Deb.h>
#include <Config/SBQLConfig.h>
#include <stdarg.h>
#include <stdio.h>
using namespace Config;

namespace QParser {

    bool Deb::writeDebugs;
        
    void Deb::setWriteDebugs() {
        SBQLConfig cnf("QueryParser");
        cnf.getBool("debugMode", writeDebugs);
    }

    bool Deb::ugOn() {
    	return writeDebugs;
    }
    
    void Deb::ug (const char *fmt, ...) {
	if (Deb::ugOn()) {
    	    va_list fmt_args;
	    fprintf(stderr, "[QParser]: ");
	    va_start(fmt_args, fmt);
	    vfprintf(stderr, fmt, fmt_args);
	    va_end (fmt_args);
	    fprintf(stderr, "\n");
	}
    }


}


