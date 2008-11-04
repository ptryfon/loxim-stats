#include <stdarg.h>
#include <fstream>
#include <iostream>
#include <ios>
#include "ErrorConsole.h"
#include "Errors.h"
#include "Config/SBQLConfig.h"

using namespace std;
using namespace Config;

namespace Errors {

    extern struct ErrorMessages {
        int e;
        char *s;
    } ErrorMsg[];


    bool ErrorConsole::useLogFile = true;

    ofstream* ErrorConsole::consoleFile = NULL;
    int ErrorConsole::serr = 0;

    ErrorConsole::ErrorConsole() {
    };

    ErrorConsole::ErrorConsole(string module) {
        owner = module;
    };
    //XXX  CHANGED TO BE READ FROM CONF FILE

    int ErrorConsole::init(void)
    //	int ErrorConsole::init(int tostderr)
    {
        SBQLConfig c("Errors");
        string f;
        serr = 0;
        if (c.getInt("stderr", serr) != 0)
            serr = 0;
        if (c.getString("logfile", f) != 0)
            useLogFile = false;
        if (useLogFile) {
            if (consoleFile == NULL)
                consoleFile = new ofstream(f.c_str(), ios::app);
            if (!consoleFile->is_open())
                return ENoFile | ErrErrors;
        }
        return 0;
    };

    ErrorConsole& ErrorConsole::operator<<(int error) {
        string src_mod, dest_mod, str;

            switch (error & ErrAllModules) {
                case ErrBackup:
                    src_mod = "Backup";
                    break;
                case ErrConfig:
                    src_mod = "Config";
                    break;
                case ErrDriver:
                    src_mod = "Driver";
                    break;
                case ErrErrors:
                    src_mod = "Errors";
                    break;
                case ErrLockMgr:
                    src_mod = "Lock Manager";
                    break;
                case ErrLogs:
                    src_mod = "Logs";
                    break;
                case ErrQExecutor:
                    src_mod = "Query Executor";
                    break;
                case ErrQParser:
                    src_mod = "Query Parser";
                    break;
                case ErrSBQLCli:
                    src_mod = "SBQLCli";
                    break;
                case ErrServer:
                    src_mod = "Server";
                    break;
                case ErrStore:
                    src_mod = "Store";
                    break;
                case ErrTManager:
                    src_mod = "Transaction Manager";
                    break;
                case ErrTypeChecker:
                    src_mod = "Type Checker";
                    break;
                case ErrUserProgram:
                    src_mod = "User Program";
                    break;
                default:
                    src_mod = "Unknown";
                    break;
            }
            if ((error & ~ErrAllModules) < 0x100) {
                str = strerror(error & ~ErrAllModules);
            } else {
                int erridx = (error & ~ErrAllModules);
                int i = 0;
                while (ErrorMsg[i].e != EUnknown) {
                    if (ErrorMsg[i].e == erridx)
                        break;
                    i++;
                }
                str = ErrorMsg[i].s;
            }
            if (owner.length() == 0)
                dest_mod = "Unknown: ";
            else
                dest_mod = owner + ": ";
        if (useLogFile) {
            *consoleFile << dest_mod << src_mod << " said: " << str << " (errno: " << (error & ~ErrAllModules) << ")\n";
            consoleFile->flush();
        }
        if (serr) {
            cerr << dest_mod << src_mod << " said: " << str << " (errno: " << (error & ~ErrAllModules) << ")\n";
            cerr.flush();
        }

        return *this;
    };

    ErrorConsole& ErrorConsole::operator<<(const string &errorMsg) {
        string dest_mod;

        if (owner.length() == 0)
            dest_mod = "Unknown: ";
        else
            dest_mod = owner + ": ";
        if (useLogFile) {
            *consoleFile << dest_mod << errorMsg << "\n";
            consoleFile->flush();
        }
        if (serr) {
            cerr << dest_mod << errorMsg << "\n";
            cerr.flush();
        }
        return *this;
    };

    ErrorConsole& ErrorConsole::printf(const char *format, ...) {
        char str[1024];
        va_list ap;
        string dest_mod;

        va_start(ap, format);
        vsnprintf(str, 1024, format, ap);
        va_end(ap);

        if (owner.length() == 0)
            dest_mod = "Unknown: ";
        else
            dest_mod = owner + ": ";

        *consoleFile << dest_mod << str;
        consoleFile->flush();
        if (serr) {
            cerr << dest_mod << str;
            cerr.flush();
        }
        return *this;
    };

    void ErrorConsole::free(void) {
        if (consoleFile != NULL) {
            delete consoleFile;
            consoleFile = NULL;
        }
    };

    ErrorConsole::~ErrorConsole() {
    };
}
