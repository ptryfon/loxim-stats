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
				std::cout << "creating event" << endl;
				ConnectionClosedEvent *ev = new ConnectionClosedEvent(error, parent);
				std::cout << "sending event" << endl;
				QApplication::postEvent(&Application::get_instance().dummy, ev);
				std::cout << "event dlivered" << endl;
				delete this;
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

	Application::Application()
	{
		destroyer.run();
	}

	void Application::connect(const std::string &host, int port, Client::Authenticator &auth, QWidget *parent)
	{
		ConnectionClosedCallback *cb = new ConnectionClosedCallback(parent);
		if (client.get())
			destroy_client(client);
		client = auto_ptr<Client::Client>(new Client::Client(host, port, auth, *cb));
	}


	void Application::destroy_client(std::auto_ptr<Client::Client> c)
	{
		destroyer.submit(c);
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

