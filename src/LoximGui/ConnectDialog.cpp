#include <string>
#include <arpa/inet.h>
#include <QtGui>
#include <Client/Client.h>
#include <Errors/Exceptions.h>
#include <LoximGui/Authenticator.h>
#include <LoximGui/ConnectDialog.h>
#include <LoximGui/Application.h>

using namespace std;
using namespace Errors;

namespace LoximGui {

	ConnectDialog::ConnectDialog(QWidget *parent)
	{
		setupUi(this);
		connect(buttons, SIGNAL(accepted()), this, SLOT(do_connect()));
	}
	
	ConnectDialog::~ConnectDialog()
	{
		std::cout << "dtor" << std::endl;
	}

	void ConnectDialog::do_connect()
	{
		const string hostname =	server_input->text().toStdString();
		const int port = port_spin->value();
		Authenticator *auth = new Authenticator(parentWidget());
		auth->exec();
		try {
			Application::get_instance().connect(hostname, htons(port), *auth, parentWidget());
		} catch ( LoximException &ex) {
			QMessageBox::warning(
				parentWidget(),
				"Unable to connect",
				("Connection failed with message:\n" + ex.to_string()).c_str());
		} catch ( ... ) {
			QMessageBox::warning(
				parentWidget(),
				"Unable to connect",
				"Connection failed with unknown error");
		}
	}

}
