#include <Server/Clipboard.h>

using namespace Server;


string Clipboard::load(int id)
{
	return set[id];
}

int Clipboard::store(string stmt)
{
	set[id++] = stmt;
	return id;
}

Clipboard::Clipboard()
{
	id = 0;
}

Clipboard::~Clipboard()
{
}
