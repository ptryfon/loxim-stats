#include <memory>

#include <QApplication>

#include <LoximGui/LoximGui.h>

using namespace std;
using namespace LoximGui;

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setQuitOnLastWindowClosed(true);
	auto_ptr<LoximGuiWindow> window(new LoximGuiWindow);

	window->show();
	return app.exec();
}
