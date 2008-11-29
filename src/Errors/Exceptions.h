#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

namespace Errors{
	class LoximException : public std::runtime_error {
		public:
			LoximException(int err);
			int get_error() const;

		protected:
			int err;
			std::string construct_message(int err);
	};
}

#endif
