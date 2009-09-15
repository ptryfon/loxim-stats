#include <iostream>
#include <sstream>
#include <errno.h>

#include <LoximGui/Misc.h>
#include <Errors/Exceptions.h>
#include <Protocol/Enums/Packages.h>
#include <Protocol/Enums/DataPackages.h>
#include <Protocol/Packages/ASCByePackage.h>
#include <Protocol/Packages/ASCErrorPackage.h>
#include <Protocol/Packages/ASCOkPackage.h>
#include <Protocol/Packages/ASCPingPackage.h>
#include <Protocol/Packages/ASCPongPackage.h>
#include <Protocol/Packages/ASCSetoptPackage.h>
#include <Protocol/Packages/QCExecutePackage.h>
#include <Protocol/Packages/QCStatementPackage.h>
#include <Protocol/Packages/VSCAbortPackage.h>
#include <Protocol/Packages/VSCFinishedPackage.h>
#include <Protocol/Packages/VSCSendvaluePackage.h>
#include <Protocol/Packages/VSCSendvaluesPackage.h>
#include <Protocol/Packages/WCHelloPackage.h>
#include <Protocol/Packages/WCLoginPackage.h>
#include <Protocol/Packages/WCPasswordPackage.h>
#include <Protocol/Packages/WSAuthorizedPackage.h>
#include <Protocol/Packages/WSHelloPackage.h>
#include <Protocol/Packages/Data/BagPackage.h>
#include <Protocol/Packages/Data/BindingPackage.h>
#include <Protocol/Packages/Data/BoolPackage.h>
#include <Protocol/Packages/Data/DoublePackage.h>
#include <Protocol/Packages/Data/RefPackage.h>
#include <Protocol/Packages/Data/SequencePackage.h>
#include <Protocol/Packages/Data/Sint32Package.h>
#include <Protocol/Packages/Data/StructPackage.h>
#include <Protocol/Packages/Data/VarcharPackage.h>
#include <Protocol/Packages/Data/VoidPackage.h>
#include <Util/smartptr.h>

using namespace std;
using namespace Protocol;
using namespace _smptr;

namespace LoximGui {

void SimpleCallback::add_callback(TCallback tcallback) {
	callbacks_col.push_back(tcallback);
}

void SimpleCallback::fire() {
	for (CallbacksCol::iterator it = callbacks_col.begin(); it
			!= callbacks_col.end(); ++it) {
		(*it)();
	}
}

void PersonContainter::fill_person_containter(const Package &package,
		int current_person, int current_attr) {
	switch (package.get_type()) {
	case BAG_PACKAGE: {
		persons_containter.clear();
		const BagPackage &bag(dynamic_cast<const BagPackage&> (package));
		const vector<shared_ptr<Package> > &items(bag.get_packages());
		for (vector<shared_ptr<Package> >::const_iterator i = items.begin(); i
				!= items.end(); ++i) {
			if ((*i)->get_type() != STRUCT_PACKAGE)
				throw Errors::LoximException(EINVAL);
			persons_containter.push_back(std::vector<std::pair<std::string,
					std::string> >());
			fill_person_containter(**i, current_person++, current_attr);
		}
		break;
	}
	case STRUCT_PACKAGE: {
		const StructPackage
				&strct(dynamic_cast<const StructPackage&> (package));
		const vector<shared_ptr<Package> > &items(strct.get_packages());
		for (vector<shared_ptr<Package> >::const_iterator i = items.begin(); i
				!= items.end(); ++i) {
			if ((*i)->get_type() != BINDING_PACKAGE)
				throw Errors::LoximException(EINVAL);
			persons_containter[current_person].push_back(
					pair<string, string> ());
			fill_person_containter(**i, current_person, current_attr++);
		}
		break;
	}
	case VARCHAR_PACKAGE: {
		stringstream ss;
		ss
				<< dynamic_cast<const VarcharPackage&> (package).get_val_value().to_string();
		persons_containter[current_person][current_attr].second = ss.str();
		break;
	}
	case VOID_PACKAGE: {
		stringstream ss;
		ss << "void";
		persons_containter[current_person][current_attr].second = ss.str();
		break;
	}
	case SINT32_PACKAGE: {
		stringstream ss;
		ss << dynamic_cast<const Sint32Package&> (package).get_val_value();
		persons_containter[current_person][current_attr].second = ss.str();
		break;
	}
	case DOUBLE_PACKAGE: {
		stringstream ss;
		ss << dynamic_cast<const DoublePackage&> (package).get_val_value();
		persons_containter[current_person][current_attr].second = ss.str();
		break;
	}
	case BINDING_PACKAGE: {
		const BindingPackage &bp(dynamic_cast<const BindingPackage&> (package));
		stringstream ss;
		ss << bp.get_val_binding_name().to_string();
		persons_containter[current_person][current_attr].first = ss.str();
		fill_person_containter(bp.get_val_value(), current_person, current_attr);
		break;
	}
	default:
		throw Errors::LoximException(EINVAL);
	}
}

int PersonContainter::size() {
	return persons_containter.size();
}

string PersonContainter::get_string_at(int i, string name) {
	for(vector<pair<string, string> >::iterator it = persons_containter[i].begin(); it != persons_containter[i].end(); ++it) {
		if (name == it->first) {
			return it->second;
		}
	}

	return string("");
}

bool PersonContainter::has_attribute(int i, std::string attribure) {
	for(vector<pair<string, string> >::iterator it = persons_containter[i].begin(); it != persons_containter[i].end(); ++it) {
		if (attribure == it->first) {
			return true;
		}
	}
	return false;
}

}
