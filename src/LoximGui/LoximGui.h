#ifndef MYQTAPP_H
#define MYQTAPP_H

#include <LoximGui/ui_LoximGui.h>
#include <LoximGui/Authenticator.h>

namespace LoximGui {

	class LoximGuiWindow: public QMainWindow, private Ui::MainWindow
	{
		Q_OBJECT

		public:
			LoximGuiWindow(QWidget *parent = 0);


		public slots:
			void exit_clicked();
			void show_about();
			void connect_to_server();

		private:
			Authenticator auth;
				
	};

}

#endif
