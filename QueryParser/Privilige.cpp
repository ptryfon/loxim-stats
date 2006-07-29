#include <string>
#include "Privilige.h"

using namespace std;

namespace QParser {

    Privilige::Privilige(int priv) {
	this->i_priv = priv;
    };
    
    int Privilige::get_priv() {
	return i_priv;
    };

    string Privilige::get_priv_name() {
	switch (i_priv) {
	    case Read:		return string("Read");
	    case Modify:	return string("Modify");
	    case Create:	return string("Create");	
	    case Delete:	return string("Delete");
	    default:		return "Undefined";
	}
    };    
    string Privilige::to_string() {
	switch (i_priv) {
	    case Read:		return string("Privilige: Read");
	    case Modify:	return string("Privilige: Modify");
	    case Create:	return string("Privilige: Create");	
	    case Delete:	return string("Privilige: Delete");
	    default:		return string("Privilige: Undefined");
	}
    };
    
    string Privilige::READ_PRIV		= "Read";
    string Privilige::MODIFY_PRIV 	= "Modify";
    string Privilige::CREATE_PRIV	= "Create";
    string Privilige::DELETE_PRIV	= "Delete";
        
    Privilige* Privilige::clone() {
	return new Privilige(i_priv);
    };
} /* namespace  */
