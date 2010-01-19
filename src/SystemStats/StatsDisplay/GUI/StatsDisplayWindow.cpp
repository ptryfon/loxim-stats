#include <QAction>
#include <QFileDialog>
#include <QGridLayout>
#include <QMenu>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QWidget>

#include <SystemStats/StatsDisplay/GUI/StatsDisplayWindow.h>
#include <SystemStats/StatsDisplay/GUI/FileViewer.h>
#include <SystemStats/StatsDisplay/GUI/GraphViewer.h>
#include <SystemStats/StatsDisplay/GUI/LogViewer.h>
#include <SystemStats/StatsDisplay/GUI/OptionsViewer.h>

StatsDisplayWindow::StatsDisplayWindow() : log_manager(new LogViewer),
	file_manager(new FileViewer, &log_manager),
	graph_manager(new GraphViewer, &file_manager)
{
	QWidget *w = new QWidget;
	setCentralWidget(w);

	file_manager.viewer()->connect_model(&file_manager);

	create_actions();
	create_menu();
	w->setLayout(create_layout());

	file_view = new FileView(file_manager.viewer(), this);

	setWindowTitle(tr("LoXiM Statistics Display"));
	setMinimumSize(160, 160);
	resize(800, 300);

	file_view->show();
	file_view->raise();
}

QVBoxLayout * StatsDisplayWindow::create_layout()
{
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(new OptionsViewer(&graph_manager));
	layout->addWidget(log_manager.viewer());

	return layout;
}

void StatsDisplayWindow::create_actions()
{
	open_file_act = new QAction(tr("&Open file"), this);
	open_file_act->setShortcut(tr("Ctrl+O"));
	connect(open_file_act, SIGNAL(triggered()), this, SLOT(open_file()));

	open_dir_act = new QAction(tr("Open &directory"), this);
	open_dir_act->setShortcut(tr("Ctrl+D"));
	connect(open_dir_act, SIGNAL(triggered()), this, SLOT(open_dir()));

	quit_act = new QAction(tr("&Quit"), this);
	quit_act->setShortcut(tr("Ctrl+Q"));
	connect(quit_act, SIGNAL(triggered()), this, SLOT(close()));

	file_window_act = new QAction(tr("&File window"), this);
	file_window_act->setShortcut(tr("Ctrl+F"));
	connect(file_window_act, SIGNAL(triggered()), this, SLOT(file_window()));

	help_act = new QAction(tr("&Help"), this);
	help_act->setShortcut(tr("F1"));
	connect(help_act, SIGNAL(triggered()), this, SLOT(help()));

	about_act = new QAction(tr("&About"), this);
	connect(about_act, SIGNAL(triggered()), this, SLOT(about()));

	return;
}

void StatsDisplayWindow::create_menu()
{
	file_menu = menuBar()->addMenu(tr("&File"));
	file_menu->addAction(open_file_act);
	file_menu->addAction(open_dir_act);
	file_menu->addSeparator();
	file_menu->addAction(quit_act);
	
	options_menu = menuBar()->addMenu(tr("Op&tions"));
	
	window_menu = menuBar()->addMenu(tr("&Window"));
	window_menu->addAction(file_window_act);
	
	help_menu = menuBar()->addMenu(tr("&Help"));
	help_menu->addAction(help_act);
	help_menu->addAction(about_act);
	
	return;
}

void StatsDisplayWindow::open_file()
{
	QFileDialog dialog(this);

	dialog.setFileMode(QFileDialog::ExistingFiles);
	dialog.setNameFilter(tr("Text files (*.txt)"));
	dialog.setViewMode(QFileDialog::Detail);

	QStringList files;

	if (dialog.exec())
		files = dialog.selectedFiles();

	file_manager.open_files(files);

	return;
}

void StatsDisplayWindow::open_dir()
{
	QFileDialog dialog(this);

	dialog.setFileMode(QFileDialog::Directory);
	dialog.setNameFilter(tr("Directories"));
	dialog.setViewMode(QFileDialog::Detail);

	QString dir;

	if (dialog.exec())
		dir = dialog.selectedFiles().first();

	file_manager.open_directory(dir);

	return;
}

void StatsDisplayWindow::file_window()
{
	file_view->show();
	file_view->raise();
	file_view->activateWindow();
	return;
}

void StatsDisplayWindow::help()
{
}

void StatsDisplayWindow::about()
{
}
