#include <QtGui> 
#include <iostream>
#include <LoximGui/LoximGui.h>
#include <LoximGui/ConnectDialog.h>
#include <LoximGui/Misc.h>
#include <Util/smartptr.h>
#include <LoximGui/Application.h>



namespace LoximGui {

	LoximGuiWindow::LoximGuiWindow(QWidget *parent) : auth(this)
	{
		setupUi(this);
		connect(action_close, SIGNAL(triggered()), this, SLOT(exit_clicked()));
		connect(action_about, SIGNAL(triggered()), this, SLOT(show_about()));
		connect(action_connect, SIGNAL(triggered()), this, SLOT(connect_to_server()));
		SimpleCallback::TCallback ttest_callback(_smptr::bind(&LoximGuiWindow::ttest, this));
		Application::get_instance().simpleCallback.add_callback(ttest_callback);
		//Application.get_instance().simpleCallback.add(&LoximGuiWindow::ttest, *this. 
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

	void LoximGuiWindow::ttest() 
	{
		tab_widget->setEnabled(true);
		std::cout << "callback dziala  " << std::endl;
	}

}
