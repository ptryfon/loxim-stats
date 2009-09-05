#include <cassert>
#include <sstream>
#include <iostream>

#include <QtGui>

#include <LoximGui/Application.h>
#include <LoximGui/LoximGui.h>
#include <Client/Client.h>

using namespace std;

namespace LoximGui {

	class ConnectionClosedCallback : public Client::OnExit{
		private:
			QWidget *parent;

		public:
			ConnectionClosedCallback(QWidget *parent) : parent(parent)
			{
			}

			void exit(int error)
			{
				stringstream ss;
				ss << "Connection closed with code " << error << "\n";
				QMessageBox::warning(parent, "Connection closed", ss.str().c_str());
				cout << "closed" << endl;
				delete this;
			}
	};

	Application *Application::instance = NULL;

	Application &Application::create_instace(int argc, char *argv[])
	{
		assert(instance == NULL);
		instance = new Application(argc, argv);
		return *instance;
	}

	Application &Application::get_instance()
	{
		assert(instance != NULL);
		return *instance;
	}

	void Application::destroy_instance()
	{
		assert(instance != NULL);
		delete instance;
		instance = NULL;
	}

	Application::Application(int argc, char *argv[]) : QApplication(argc, argv)
	{
	}

	int Application::start()
	{
		setQuitOnLastWindowClosed(true);
		LoximGuiWindow window;
		window.show();
		return exec();
	}

	void Application::connect(const std::string &host, int port, Client::Authenticator &auth, QWidget *parent)
	{
		ConnectionClosedCallback *cb = new ConnectionClosedCallback(parent);
		client = auto_ptr<Client::Client>(new Client::Client(host, port, auth, *cb));
	}
}

int main(int argc, char *argv[])
{
	using namespace LoximGui;
	Application &app = Application::create_instace(argc, argv);
	int res = app.start();
	Application::destroy_instance();
	return res;
}

