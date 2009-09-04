#ifndef LOXIM_GUI_CONNECT_DIALOG_H
#define LOXIM_GUI_CONNECT_DIALOG_H

#include <LoximGui/ui_ConnectDialog.h>

namespace LoximGui {

	class ConnectDialog : public QDialog, private Ui::ConnectDialog
	{
		Q_OBJECT

		public:
			ConnectDialog(QWidget *parent);
			~ConnectDialog();

		public slots:
			void do_connect();
				
	};

}

#endif
