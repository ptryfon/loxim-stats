#ifndef __PRIVILIGE_H__
#define __PRIVILIGE_H__

#include <string>

using namespace std;

namespace QParser {

    class Privilige {
	private:
	    int		i_priv;
	    
	public:
	    Privilige(int i_priv);
	    
	    /* possible attributes */
	    enum priv_type { Read, Modify, Create, Delete }; 
	    
	    /* getters */
	    int get_priv();	   	    
	    string get_priv_name();
	    
	    string to_string();	    
	    Privilige* clone();
	    
	    static string READ_PRIV;
	    static string MODIFY_PRIV;
	    static string CREATE_PRIV;
	    static string DELETE_PRIV;
    };

} /* namespace */
#endif
