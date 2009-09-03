#ifndef LOXIM_GUI_AUTHENTICATOR_H
#define LOXIM_GUI_AUTHENRICATOR_H

#include <LoximGui/ui_Authenticator.h>
#include <Client/Authenticator.h>

namespace LoximGui {

	class Authenticator : public QDialog, private Ui::AuthenticatorWindow, public Client::Authenticator
	{
		Q_OBJECT

		public:
			Authenticator(QWidget *parent);
			virtual std::string get_login();
			virtual std::string get_password();
			virtual void invalid();
			virtual void read();


		public slots:
			void ok_clicked();
				
	};

}

#endif
