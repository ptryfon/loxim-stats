#ifndef LOXIM_GUI_APPLICATION_H
#define LOXIM_GUI_APPLICATION_H

#include <memory>
#include <QApplication>
#include <LoximGui/ClientDestroyer.h>
#include <LoximGui/Misc.h>
#include <Protocol/Packages/Package.h>


namespace Client {
	class Authenticator;
	class Client;
}

namespace LoximGui {
	class Application {
		public:
			class DummyQObject : public QObject {
				bool event(QEvent *e);
			};

			static Application &create_instace();
			static void destroy_instance();
			static Application &get_instance();

			Application();
			int start();
			void connect(
				const std::string &host,
				int port,
				Client::Authenticator &auth,
				QWidget *widhet
			);
			void destroy_client(std::auto_ptr<Client::Client>);
			DummyQObject dummy;
			SimpleCallback simpleCallback;
			std::auto_ptr<Protocol::Package> execute_stmt(string s);

		private:
			static Application *instance;
			ClientDestroyer destroyer;
			std::auto_ptr<Client::Client> client;
	};
}

#endif /* LOXIM_GUI_APPLICATION_H */
