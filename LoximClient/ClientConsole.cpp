#include "ClientConsole.h"
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <errno.h>
#include <string.h>

using namespace std;

#define DOT_MODE_REGEX "^(\\$set[[:space:]]+mode[[:space:]]+dot[[:space:]]*)|(\\$dot[[:space:]]*)$"
#define SLASH_MODE_REGEX "^(\\$set[[:space:]]+mode[[:space:]]+slash[[:space:]]*)|(\\$slash[[:space:]]*)$"
#define HELP_REGEX "^\\$help$"
#define FILE_REGEX "^\\$file[[:space:]]+[^[:space:]]+.*"


void LoximClient::ClientConsole::regex_init()
{
	regcomp(&dot_mode_regex, DOT_MODE_REGEX, REG_EXTENDED);
	regcomp(&slash_mode_regex, SLASH_MODE_REGEX, REG_EXTENDED);
	regcomp(&ext_file_regex, FILE_REGEX, REG_EXTENDED);
	regcomp(&help_regex, HELP_REGEX, REG_EXTENDED);
}

LoximClient::ClientConsole::ClientConsole(int mode, FILE *file)
{
	this->mode = mode;
	regex_init();
	this->file = file;
}

LoximClient::ClientConsole::ClientConsole(int mode)
{
	this->mode = mode;
	regex_init();
	file = 0;
}

LoximClient::ClientConsole::ClientConsole()
{
	this->mode = CC_SLASH;
	regex_init();
	file = 0;
}

char *LoximClient::ClientConsole::line_provider(const char *prompt)
{
	char *line = 0;
	size_t len;
	if (file){
		if ((getline(&line, &len, file) == -1) || !line){
			fclose(file);
			file = 0;
			return line_provider(prompt);
		} else {
			if (feof(file)){
				fclose(file);
				file = 0;
			}
			len = strlen(line);
			if (len > 0 && line[len - 1] == '\n')
				line[len - 1] = 0;
			return line;
		}
	}
	else {
		line = readline(prompt);
		if (line)
			add_history(line);
		return line;
	}
}

string LoximClient::ClientConsole::read_slash()
{
	string stmt;
	char *line;
	line = line_provider(" > ");
	if (!line)
		return "quit";
	while (strcmp(line, "/")){
		if (strcmp(stmt.c_str(), ""))
			stmt += "\n";
		//strange, but the old client used to do this
		if (line[0] != '-' || line[1] != '-')
			stmt += string(line);
		if (!strcmp(stmt.c_str(), "quit"))
			return stmt;
		if (is_meta_stmt(stmt))
			return stmt;
		free(line);
		line = line_provider(" \\ ");
		if (!line)
			return "quit";
	}
	free(line);
	return stmt;
}


void LoximClient::ClientConsole::open_file(const char *filename)
{
	int len = strlen(filename);
	int i, j, k;
	bool escaped;
	char *fname = (char*)malloc(len);
	if (!fname){
		printf("Out of memory\n");
		return;
	}
		
	//unescape
	escaped = false;
	j = 0;
	for (i = 0; i < len; i++){
		if (escaped){
			//TODO do some checking
			fname[j] = filename[i];
			j++;
			escaped = false;

		} else {
			switch (filename[i]){
				case '\\': 
					escaped = true;
					break;
				case ' ':
					//it's ok if there are only spaces after it - we ignore them
					for (k = i + 1; k < len; k++)
						if (!isspace(filename[k])){
							printf("File name contains unescaped spaces\n");
							free(fname);
							return;
						}
					break;
				default:
					fname[j] = filename[i];
					j++;
					break;
			}
		}
	}
	fname[j] = 0;
	file = fopen(fname, "r");
	if (!file){
		printf("Error opening file: %s\n", strerror(errno));
	}
	free(fname);
			
}


void LoximClient::ClientConsole::execute_meta_stmt(string stmt)
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
		const char *buf = stmt.c_str();
		while (!isspace(*buf))
			buf++;
		while (isspace(*buf))
			buf++;
		open_file(buf);
		return;
	}
	if (!regexec(&help_regex, stmt.c_str(), 0, 0, 0)){
		printf("Available client commands:\n");
		printf("$set mode (dot|slash)  - change the input mode\n");
		printf("$dot                   - same as $set mode dot\n");
		printf("$slash                 - same as $set mode slash\n");
		printf("$file <file>           - load an external file <file>. It will be interpeted in current mode.\n");
		printf("                         Spaces should be escaped with \\\n");
		printf("quit                   - close the session\n");
		printf("$help                  - this help\n");
		return;
	}
	printf("Invalid meta command\n");
}

bool LoximClient::ClientConsole::is_meta_stmt(string stmt)
{
	return stmt.c_str()[0] == '$';
}

bool LoximClient::ClientConsole::line_empty(char *buf)
{
	while (*buf)
		if (!isspace(*buf))
			return false;
		else
			buf++;
	return true;
}


string LoximClient::ClientConsole::read_dot()
{	
	string stmt;
	char *line;
	line = line_provider(" > ");
	if (!line)
		return "quit";
	if (!strcmp(line, ".")){
		do {
			free(line);
			line = line_provider(" \\ ");
			if (!line)
				return "quit";
			if (strcmp(stmt.c_str(), ""))
				stmt += "\n";
			//strange, but the old client used to do this
			if (strcmp(line, "/") && (line[0] != '-' || line[1] != '-'))
				stmt += string(line);
		} while (strcmp(line, "/"));
		free(line);
	} else {
		while (line_empty(line)){
			free(line);
			line = line_provider(" > ");
		}
		stmt = string(line);
		free(line);
	}
	return stmt;
}

string LoximClient::ClientConsole::read_stmt()
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

LoximClient::ClientConsole::~ClientConsole()
{
}

