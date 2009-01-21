#ifndef STATEMENT_PROVIDER_H
#define STATEMENT_PROVIDER_H

#include <string>

namespace Client{
	class StatementProvider{
	    public:
		StatementProvider();
		virtual std::string read_stmt() = 0;
		virtual ~StatementProvider();
		
	};
}

#endif /* STATEMENT_PROVIDER_H */
