#include <cassert>
#include <sstream>
#include <iostream>

#include <QtGui>

#include <LoximGui/Application.h>
#include <LoximGui/LoximGui.h>
#include <Client/Client.h>

using namespace std;

namespace LoximGui {


	class ConnectionClosedEvent : public QEvent {
		public:
			const int error;
			QWidget *parent;
			ConnectionClosedEvent(int error, QWidget *parent) : QEvent(QEvent::None), error(error), parent(parent)
			{
			}
	};

	class ConnectionClosedCallback : public Client::OnExit{
		private:
			QWidget *parent;

		public:
			ConnectionClosedCallback(QWidget *parent) : parent(parent)
			{
			}

			void exit(int error)
			{
				ConnectionClosedEvent ev(error, parent);
				QApplication::sendEvent(&Application::get_instance().dummy, &ev);
			}
	};


	bool Application::DummyQObject::event(QEvent *e)
	{
		ConnectionClosedEvent *c = dynamic_cast<ConnectionClosedEvent *>(e);
		if (!c) {
			cout << "ale kupa" << endl;
		} else {
			stringstream ss;
			ss << "Connection closed with code " << c->error << "\n";
			QMessageBox::warning(c->parent, "Connection closed", ss.str().c_str());
			cout << "closed" << endl;
		}
		delete this;
		return true;
	}

	Application *Application::instance = NULL;

	Application &Application::create_instace()
	{
		assert(instance == NULL);
		instance = new Application;
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


	void Application::connect(const std::string &host, int port, Client::Authenticator &auth, QWidget *parent)
	{
		ConnectionClosedCallback *cb = new ConnectionClosedCallback(parent);
		client = auto_ptr<Client::Client>(new Client::Client(host, port, auth, *cb));
	}
}

int main(int argc, char *argv[])
{
	using namespace LoximGui;
	Application::create_instace();
	new QApplication(argc, argv);
	QApplication::quitOnLastWindowClosed();
	LoximGuiWindow w(NULL);
	w.show();
	int res = QApplication::exec();
	Application::destroy_instance();
	return res;
}

