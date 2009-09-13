#include <LoximGui/Misc.h>


using namespace std;

namespace LoximGui 
{

	void SimpleCallback::add_callback(TCallback tcallback) 
	{
		callbacks_col.push_back(tcallback);
	}
	
	void SimpleCallback::fire() 
	{
		for(CallbacksCol::iterator it = callbacks_col.begin(); it != callbacks_col.end(); ++it) 
		{
			(*it)();
		}
	}

}
