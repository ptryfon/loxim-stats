#include <string>
#include <arpa/inet.h>
#include <QtGui>
#include <LoximGui/Authenticator.h>
#include <Client/Client.h>
#include <LoximGui/ConnectDialog.h>

using namespace std;

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
		Authenticator *auth = new Authenticator(this);
		auth->exec();
		new Client::Client(hostname, htons(port), *auth);

	}

}
