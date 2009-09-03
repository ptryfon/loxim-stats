#include <QtGui> 
#include <LoximGui/Authenticator.h>

using namespace std;

namespace LoximGui {

	Authenticator::Authenticator(QWidget *parent)
	{
		setupUi(this);
		connect(okButton, SIGNAL(clicked()), this, SLOT(ok_clicked()));
	}

	void Authenticator::ok_clicked()
	{
		close();
	}

	string Authenticator::get_login()
	{
		return loginEdit->text().toStdString();
	}
	
	string Authenticator::get_password()
	{
		return passwdEdit->text().toStdString();
	}

	void Authenticator::invalid()
	{
	}

	void Authenticator::read()
	{
		this->show();
	}


}
