#include <QtGui> 
#include <LoximGui/Authenticator.h>

using namespace std;

namespace LoximGui {

	Authenticator::Authenticator(QWidget *parent)
	{
		setupUi(this);
		connect(ok_button, SIGNAL(clicked()), this, SLOT(ok_clicked()));
	}

	void Authenticator::ok_clicked()
	{
		close();
	}

	string Authenticator::get_login()
	{
		return login_edit->text().toStdString();
	}
	
	string Authenticator::get_password()
	{
		return passwd_edit->text().toStdString();
	}

	void Authenticator::invalid()
	{
	}

	void Authenticator::read()
	{
		this->show();
	}


}
