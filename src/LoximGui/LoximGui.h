#ifndef MYQTAPP_H
#define MYQTAPP_H

#include <LoximGui/ui_LoximGui.h>
#include <LoximGui/Authenticator.h>
#include <Protocol/Packages/Package.h>
#include <LoximGui/Misc.h>


namespace LoximGui {

	class LoximGuiWindow: public QMainWindow, private Ui::MainWindow
	{
		Q_OBJECT

		public:
			LoximGuiWindow(QWidget *parent = 0);
			void ttest();
			void fill_people();
			void fill_people_table();


		public slots:
			void exit_clicked();
			void show_about();
			void connect_to_server();
			void sbql_submit();
			void clear_command_edit();
			void test_par(QTableWidgetItem* item);
			void expand_clicked();
			void collapse_clicked();


		private:
			void executeHelper(std::string statement);
			void init_people_table();
			Authenticator auth;
			LoximGui::PersonContainter person_container;
			void print_result(const Protocol::Package &package, QTreeWidgetItem *parent, QTreeWidget *result_view);
			void sbql_submit(QTreeWidget *result_view, const std::string &stmt);
			void collapse(QTreeWidget *result_view);
			void expand(QTreeWidget *result_view);
			void refresh_university_tab();
			void university_views_sbql_submit(QTreeWidget *result_view, const std::string &stmt);

	};

}

#endif
