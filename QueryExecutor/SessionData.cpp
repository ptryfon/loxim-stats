#include <string>
#include <iostream>
#include "SessionData.h"

using namespace std;

namespace SessionDataNms {

    /**
     *	SESSION DATA BEGIN
     */
    SessionData::SessionData() {
	user_data = NULL;
    };
    
    SessionData::~SessionData() {
	if (user_data)
	    delete user_data;
    };
    
    string SessionData::to_string() {
	string message = "Session data object. Attributes: ";
	string user_data_message = user_data->to_string();
	message += user_data_message;
	
	return message;
    };
    
    void SessionData::set_user_data(UserData *user_data) {
	this->user_data = user_data;
    };
    UserData* SessionData::get_user_data() {
	return user_data;
    };
    /**
     *	SESSION DATA END
     */
     
    /**
     *	USER DATA BEGIN
     */
    UserData::UserData(string login, string passwd) {
	this->login	= login;
	this->passwd	= passwd;
    };
    UserData::~UserData() {};
    string UserData::to_string() {
	string message = "User data object: " + login + ", " + passwd;
	return message;
    };
    string UserData::get_login() {
	return login;
    };
    string UserData::get_passwd() {
	return passwd;
    };
    /**
     *	USER DATA END
     */

} /* namespace SessionDataNms */
