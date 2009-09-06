#ifndef LOXIM_GUI_APPLICATION_H
#define LOXIM_GUI_APPLICATION_H

#include <memory>
#include <QApplication>

namespace Client {
	class Authenticator;
	class Client;
}

namespace LoximGui {
	class Application : private QApplication {
		public:
			static Application &create_instace(int argc, char *argv[]);
			static void destroy_instance();
			static Application &get_instance();

			int start();
			void connect(
				const std::string &host,
				int port,
				Client::Authenticator &auth,
				QWidget *widhet
			);

		private:
			Application(int argc, char *argv[]);
			std::auto_ptr<Client::Client> client;
			static Application *instance;
	};
}

#endif /* LOXIM_GUI_APPLICATION_H */
