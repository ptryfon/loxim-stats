#ifndef MYQTAPP_H
#define MYQTAPP_H

#include <LoximGui/ui_LoximGui.h>
#include <LoximGui/Authenticator.h>
#include <Protocol/Packages/Package.h>


namespace LoximGui {

	class LoximGuiWindow: public QMainWindow, private Ui::MainWindow
	{
		Q_OBJECT

		public:
			LoximGuiWindow(QWidget *parent = 0);
			void ttest();


		public slots:
			void exit_clicked();
			void show_about();
			void connect_to_server();
			void sbql_submit();
			void clear_command_edit();

		private:
			Authenticator auth;
			void print_result(const Protocol::Package &package, QTreeWidgetItem *parent);
				
	};

}

#endif
