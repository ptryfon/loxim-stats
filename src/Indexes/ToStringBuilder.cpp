#include "ToStringBuilder.h"

namespace Indexes {

	ToStringBuilder::ToStringBuilder(string classname, void* pointer) {
		temp << classname << "@" << pointer << "[";
		empty = true;
	}
	
	ToStringBuilder::~ToStringBuilder(){}

	template <typename T>
	void ToStringBuilder::add(string name, T value) {
		if (!empty) {
			temp << ", ";
		}
		temp << name << "= " << value;
		empty = false;
	}

	string ToStringBuilder::toString() {
		temp << "]";
		return temp.str();
	}
	
	ToStringBuilder& ToStringBuilder::append(string name, string value) {
		add(name, value);
		return *this;
	}
	
	ToStringBuilder& ToStringBuilder::append(string name, int value){
		add(name, value);
		return *this;
	}
	
	ToStringBuilder& ToStringBuilder::append(string name, unsigned int value){
		add(name, value);
		return *this;
	}
	
	ToStringBuilder& ToStringBuilder::append(string name, double value){
		add(name, value);
		return *this;
	}
}
