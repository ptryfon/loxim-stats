#include <memory>

#include <QApplication>

#include <LoximGui/LoximGui.h>

using namespace std;
using namespace LoximGui;

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setQuitOnLastWindowClosed(true);
	LoximGuiWindow window;

	window.show();
	return app.exec();
}
