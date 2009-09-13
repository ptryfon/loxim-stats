#ifndef LOXIM_GUI_MISC
#define LOXIM_GUI_MISC

#include <vector>
//just a hack to use _smptr instead of std::tr1
#include <Util/smartptr.h>


namespace LoximGui {

	class SimpleCallback 
	{
	public:
		typedef _smptr::function< void () > TCallback;
		typedef std::vector <TCallback> CallbacksCol;
		void add_callback(TCallback tcallback);
		void fire();
	private:
		CallbacksCol callbacks_col;		
	};

}

#endif
