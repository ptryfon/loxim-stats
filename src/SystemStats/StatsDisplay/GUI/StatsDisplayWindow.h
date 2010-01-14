#ifndef STATS_DISPLAY_WINDOW_H_
#define STATS_DISPLAY_WINDOW_H_

#include <QMainWindow>

#include <SystemStats/StatsDisplay/Logic/FileManager.h>
#include <SystemStats/StatsDisplay/Logic/GraphManager.h>

class QActions;
class QGridLayout;
class QMenu;

class StatsDisplayWindow : public QMainWindow
{
	Q_OBJECT
	
	public:
		StatsDisplayWindow();
	
	private:
		QGridLayout * create_layout();
		void create_actions();
		void create_menu();

		QMenu *file_menu, *options_menu, *help_menu;
		QAction *open_file_act, *open_dir_act, *quit_act;
		QAction *help_act, *about_act;

		LogManager log_manager;
		FileManager file_manager;
		GraphManager graph_manager;

		private slots:
			void open_file();
			void open_dir();
			void help();
			void about();
};

#endif
