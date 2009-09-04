#include <QtGui> 
#include <LoximGui/LoximGui.h>
#include <LoximGui/ConnectDialog.h>

namespace LoximGui {

	LoximGuiWindow::LoximGuiWindow(QWidget *parent) : auth(this)
	{
		setupUi(this);
		connect(action_close, SIGNAL(triggered()), this, SLOT(exit_clicked()));
		connect(action_about, SIGNAL(triggered()), this, SLOT(show_about()));
		connect(action_connect, SIGNAL(triggered()), this, SLOT(connect_to_server()));
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
		ConnectDialog *cd =  new ConnectDialog(this);
		cd->show();
	}

}
