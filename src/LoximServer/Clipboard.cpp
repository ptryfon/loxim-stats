#include <LoximServer/Clipboard.h>

using namespace LoximServer;


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
