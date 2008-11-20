#ifndef CLIENT_CONSOLE_H
#define CLIENT_CONSOLE_H

#include <string>
#include <regex.h>
#include <LoximClient/StatementProvider.h>
#include <stdio.h>

using namespace std;

#define CC_SLASH 1
#define CC_DOT 2

namespace LoximClient{
	class ClientConsole : public StatementProvider{
		private:
			int mode;
			string read_dot();
			string read_slash();
			bool is_meta_stmt(string stmt);
			bool is_admin_stmt(string stmt);
			void execute_meta_stmt(string stmt);
			char *line_provider (const char *);
			regex_t dot_mode_regex;
			regex_t slash_mode_regex;
			regex_t ext_file_regex;
			regex_t help_regex;
			FILE *file;
			bool line_empty(char *);
			void open_file(const char *);
			void regex_init();
		public:
			ClientConsole();
			ClientConsole(int mode);
			//should be opened
			ClientConsole(int mode, FILE *file);
			string read_stmt();
			~ClientConsole();
	};
}

#endif /* CLIENT CONSOLE */

