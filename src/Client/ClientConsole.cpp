#include <errno.h>
#include <string.h>
#include <cstdlib>
#include <iostream>

#include <readline/readline.h>
#include <readline/history.h>

#include <Client/ClientConsole.h>

using namespace std;

#define DOT_MODE_REGEX "^(\\$set[[:space:]]+mode[[:space:]]+dot[[:space:]]*)|(\\$dot[[:space:]]*)$"
#define SLASH_MODE_REGEX "^(\\$set[[:space:]]+mode[[:space:]]+slash[[:space:]]*)|(\\$slash[[:space:]]*)$"
#define HELP_REGEX "^\\$help$"
#define FILE_REGEX "^\\$file[[:space:]]+[^[:space:]]+.*"

namespace Client{


	void ClientConsole::regex_init()
	{
		::regcomp(&dot_mode_regex, DOT_MODE_REGEX, REG_EXTENDED);
		::regcomp(&slash_mode_regex, SLASH_MODE_REGEX, REG_EXTENDED);
		::regcomp(&ext_file_regex, FILE_REGEX, REG_EXTENDED);
		::regcomp(&help_regex, HELP_REGEX, REG_EXTENDED);
	}

	ClientConsole::ClientConsole(int mode, auto_ptr<ifstream> file) :
		mode(mode), file(file)
	{
		regex_init();
	}

	ClientConsole::ClientConsole(int mode) :
		mode(mode)
	{
		regex_init();
	}

	ClientConsole::ClientConsole() :
		mode(CC_SLASH)
	{
		regex_init();
	}

	string ClientConsole::line_provider(const string &prompt)
	{
		if (file.get()){
			string res;
			getline(*file, res);
			if (file->fail()){
				file.reset();
				return line_provider(prompt);
			}
			if (file->eof())
				file.reset();
			return res;
		}
		else {
			char *line = readline(prompt.c_str());
			if (line)
				add_history(line);
			if (!line)
				return "quit";
			string res(line);
			free(line);
			return res;
		}
	}

	string ClientConsole::read_slash()
	{
		string stmt;
		string line = line_provider(" > ");
		while (line.compare("/")){
			if (stmt.size() != 0)
				stmt += "\n";
			//strange, but the old client used to do this
			if ((line.size() > 0 && line[0] != '-') || (line.size() > 1 && line[1] != '-'))
				stmt += line;
			if (!stmt.compare("quit"))
				return stmt;
			if (is_meta_stmt(stmt))
				return stmt;
			if (is_admin_stmt(stmt))
				return stmt;
			line = line_provider(" \\ ");
		}
		return stmt;
	}


	void ClientConsole::open_file(const string &filename)
	{
		//unescape
		bool escaped = false;
		string fname;
		for (size_t i = 0; i < filename.length(); i++){
			if (escaped){
				//TODO do some checking
				fname += filename[i];
				escaped = false;
			} else {
				switch (filename[i]){
					case '\\': 
						escaped = true;
						break;
					case ' ':
						//it's ok if there are only spaces after it - we ignore them
						for (size_t k = i + 1; k < filename.length(); k++)
							if (!isspace(filename[k])){
								cout << "File name contains unescaped spaces, not opened." << endl;
								return;
							}
						file = auto_ptr<ifstream>(new ifstream(fname.c_str()));
						if (!file->is_open()){
							cout << "Error opening file" << endl;
							file.reset();
						}
						return;
					default:
						fname += filename[i];
						break;
				}
			}
		}
		file = auto_ptr<ifstream>(new ifstream(fname.c_str()));
		if (!file->is_open()){
			cout << "Error opening file" << endl;
			file.reset();
		}
	}


	void ClientConsole::execute_meta_stmt(const string &stmt)
	{
		if (!regexec(&dot_mode_regex, stmt.c_str(), 0, 0, 0)){
			printf("Switching to dot mode\n");
			mode = CC_DOT;
			return;
		}
		if (!regexec(&slash_mode_regex, stmt.c_str(), 0, 0, 0)){
			printf("Switching to slash mode\n");
			mode = CC_SLASH;
			return;
		}
		if (!regexec(&ext_file_regex, stmt.c_str(), 0, 0, 0)){
			
			string::const_iterator i = stmt.begin();
			while (!isspace(*i))
				i++;
			while (isspace(*i))
				i++;
			open_file(string(i, stmt.end()));
			return;
		}
		if (!regexec(&help_regex, stmt.c_str(), 0, 0, 0)){
			cout << "Available client commands:" << endl;
			cout << "$set mode (dot|slash)  - change the input mode" << endl;
			cout << "$dot                   - same as $set mode dot" << endl;
			cout << "$slash                 - same as $set mode slash" << endl;
			cout << "$file <file>           - load an external file <file>. It will be interpeted in current mode." << endl;
			cout << "                         Spaces should be escaped with \\" << endl;
			cout << "quit                   - close the session" << endl;
			cout << "$help                  - this help" << endl;
			return;
		}
		cout << "Invalid meta command" << endl;
	}

	bool ClientConsole::is_meta_stmt(const string &stmt)
	{
		return stmt.size() && (stmt[0] == '$');
	}

	bool ClientConsole::is_admin_stmt(const string &stmt)
	{
		return stmt.size() && (stmt[0] == '#');
	}

	bool ClientConsole::line_empty(const string &buf)
	{
		string::const_iterator i = buf.begin();
		while (i != buf.end())
			if (!isspace(*i))
				return false;
			else
				++i;
		return true;
	}


	string ClientConsole::read_dot()
	{	
		string stmt;
		string line = line_provider(" > ");
		if (!line.compare(".")){
			do {
				line = line_provider(" \\ ");
				if (stmt.length() != 0)
					stmt += "\n";
				//strange, but the old client used to do this
				if (line.compare("/") && (((line.length() > 0) && (line[0] != '-')) || ((line.length() > 1) && (line[1] != '-'))))
					stmt += line;
			} while (line.compare("/"));
		} else {
			while (line_empty(line)){
				line = line_provider(" > ");
			}
			stmt = line;
		}
		return stmt;
	}

	string ClientConsole::read_stmt()
	{
		string cmd;
		do {
			if (mode == CC_DOT)
				cmd = read_dot();
			else
				cmd = read_slash();
			if (is_meta_stmt(cmd))
				execute_meta_stmt(cmd);
		} while (is_meta_stmt(cmd));
		return cmd;
	}

	ClientConsole::~ClientConsole()
	{
	}

}
