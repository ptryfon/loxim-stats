/**
 * AdminTreeValueNode.h
 * @author Pawel Tryfon (pt248444)
 *
 * The purpose of this file is to define a class that can store a value obtained
 * during parsing of admin command
 */

#ifndef ADMINTREEVALUENODE_H
#define ADMINTREEVALUENODE_H

#include <AdminParser/AdminTreeNode.h>

namespace AdminParser {

	/**
	 * AdminTreeValueNode is a template class allowing only to set and get a value that
	 * can be passed during the process of parsing
	 */
	template <class ValueType> class AdminTreeValueNode: public AdminTreeNode {
		
		private:
			ValueType val;
		public:
			AdminTreeValueNode(ValueType _val): val(_val) {}
			const ValueType& get_value() const {return val;}
			void set_value(ValueType _val) {val = _val;}
			virtual bool is_executable() {return false;}
			virtual ~AdminTreeValueNode() {}

	};

}

#endif

