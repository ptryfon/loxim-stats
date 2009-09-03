#include <QtGui> 
//#include <Client/Client.h>
#include <LoximGui/LoximGui.h>

namespace LoximGui {

	LoximGuiWindow::LoximGuiWindow(QWidget *parent) : auth(this)
	{
		setupUi(this);
		connect(actionClose, SIGNAL(triggered()), this, SLOT(exit_clicked()));
		connect(actionAbout, SIGNAL(triggered()), this, SLOT(show_about()));
		connect(actionConnect, SIGNAL(triggered()), this, SLOT(connect_to_server()));
	}

	void LoximGuiWindow::exit_clicked()
	{
		close();
			
	}

	void LoximGuiWindow::show_about()
	{
		QMessageBox::about(this,"About LoXiMGui",
			"LoXiMGui is a simple program for demonstrating LoXiM functionality."
			"\n\n License: LGPL-v3\n\n"
			"Author: Marek Dopiera");
	}

	void LoximGuiWindow::connect_to_server()
	{
/*		auth.read();
		Client *cli = new Client(0, 2000, &auth);
		label.setText(Qstring("connected !"))*/
	}

}
