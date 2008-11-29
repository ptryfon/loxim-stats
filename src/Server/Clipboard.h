#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <iostream>
#include <map>

using namespace std;


namespace Server{
	class Clipboard{
    	
		public:
			Clipboard();
			int store(string stmt);
			string load(int id);
			~Clipboard();
		private:
			map<int, string> set;
			int id;
	};
}

#endif
