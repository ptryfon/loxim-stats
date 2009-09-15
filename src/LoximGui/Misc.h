#ifndef LOXIM_GUI_MISC
#define LOXIM_GUI_MISC

#include <utility>
#include <vector>
//just a hack to use _smptr instead of std::tr1
#include <Util/smartptr.h>
#include <Protocol/Packages/Package.h>


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


	class PersonContainter
	{
	public:
		void fill_person_containter(const Protocol::Package &package, int current_person, int current_attr);
		int size();
		std::string get_string_at(int i, std::string name);
		int get_int_at(int i, std::string name);
		bool has_attribute(int i, std::string attribure);
	private:
		typedef std::vector<std::vector<std::pair<std::string, std::string> > > PersContainter;
		PersContainter persons_containter;
		int current_person;
		int current_attr;


	};
}

#endif
