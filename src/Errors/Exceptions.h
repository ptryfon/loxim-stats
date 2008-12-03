#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>
#include <iostream>
#include <vector>
#include <Errors/Errors.h>

namespace Errors{
	class ErrorConsole;

	class LoximException : public std::runtime_error {
		friend std::ostream &operator<<(std::ostream&, const LoximException&);

		public:
			LoximException(int err);
			int get_error() const;
			const std::vector<std::string> &get_bt() const;
			virtual ~LoximException() throw();
			string to_string() const;
		protected:
			int err;
			std::string construct_message(int err);
			std::vector<std::string> bt;

	};
	std::ostream &operator<<(std::ostream&, const LoximException&);
}

#endif
