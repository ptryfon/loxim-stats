#ifndef __SESSION_DATA_H__
#define __SESSION_DATA_H__

#include <string>
using namespace std;

namespace SessionDataNms {
    
    /* declared classes */
    class SessionData;
    class UserData;
    
    /**
     *	Class SessionData holds data of particular session,
     *	f.e. it holds user login and password, and may hold some
     *	session parameters
     */
    class SessionData {
	protected:
	    UserData *user_data;
	public:
	    SessionData();
	    virtual ~SessionData();	    
	    
	    virtual string to_string();
	
	    /* setters */
	    virtual void set_user_data(UserData *user_data);
	    
	    /* getters */
	    virtual UserData* get_user_data();
    };
    
    class UserData {
	protected:
	    string login;
	    string passwd;
	public:
	    UserData(string login, string passwd);
	    virtual ~UserData();

	    virtual string to_string();	    
	    
	    /* getters */
	    virtual string get_login();
	    virtual string get_passwd();
    };

} /* namespace SessionDataNms */
#endif
