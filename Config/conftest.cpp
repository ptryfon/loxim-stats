#include <iostream>
#include "SBQLConfig.h"

using namespace Config;

int main()
{
	bool b;
	int i;
	double d;
	string s;
	
	SBQLConfig c("Server");

	cerr << c.init("example.conf") << endl;
	if (c.getString("syslog", s) == 0)
		cout << s << endl;
	else
		cout << "error s" << endl;
	if (c.getBool("syslog", b) == 0)
		cout << b << endl;
	else
		cout << "error b" << endl;
	if (c.getInt("syslog", i) == 0)
		cout << i << endl;
	else
		cout << "error i" << endl;
	if (c.getDouble("syslog", d) == 0)
		cout << d << endl;
	else
		cout << "error d" << endl;
	c.dumpConfig();
}
