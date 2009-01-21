#ifndef CLIENT_CONSOLE_H
#define CLIENT_CONSOLE_H

#include <string>
#include <fstream>
#include <regex.h>
#include <stdio.h>
#include <memory>
#include <Client/StatementProvider.h>

namespace Client{
	enum Modes{
		CC_SLASH = 1,
		CC_DOT
	};

	class ClientConsole : public StatementProvider{
		private:
			int mode;
			std::auto_ptr<std::ifstream> file;
			regex_t dot_mode_regex;
			regex_t slash_mode_regex;
			regex_t ext_file_regex;
			regex_t help_regex;
			
			std::string read_dot();
			std::string read_slash();
			bool is_meta_stmt(const std::string &stmt);
			bool is_admin_stmt(const std::string &stmt);
			void execute_meta_stmt(const std::string &stmt);
			std::string line_provider (const std::string &);
			bool line_empty(const std::string &);
			void open_file(const std::string &);
			void regex_init();
		public:
			ClientConsole();
			ClientConsole(int mode);
			//should be opened
			ClientConsole(int mode, std::auto_ptr<std::ifstream> file);
			std::string read_stmt();
			~ClientConsole();
	};
}

#endif /* CLIENT CONSOLE */

