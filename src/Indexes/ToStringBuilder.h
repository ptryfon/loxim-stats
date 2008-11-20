#ifndef TOSTRINGBUILDER_H_
#define TOSTRINGBUILDER_H_

#include <string>
#include <sstream>
using namespace std;
namespace Indexes {

	class ToStringBuilder {
		private:
			stringstream temp;
			bool empty;
			
			template <typename T>
			void add(string name, T value);
		public:
			ToStringBuilder(string classname, void* pointer);
			virtual ~ToStringBuilder();
			
			virtual ToStringBuilder& append(string name, string value);
			virtual ToStringBuilder& append(string name, int value);
			virtual ToStringBuilder& append(string name, unsigned int value);
			virtual ToStringBuilder& append(string name, double value);
			
			virtual string toString();
	};

}

#endif /*TOSTRINGBUILDER_H_*/
