#include <QAction>
#include <QFileDialog>
#include <QGridLayout>
#include <QMenu>
#include <QMenuBar>
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
	
	setWindowTitle(tr("LoXiM Statistics Display"));
	setMinimumSize(160, 160);
	resize(800, 600);
}

QGridLayout * StatsDisplayWindow::create_layout()
{
	QGridLayout *layout = new QGridLayout;
	layout->addWidget(graph_manager.viewer(), 0, 0, 4, 3);
	layout->addWidget(file_manager.viewer(), 0, 3, 6, 1);
	layout->addWidget(new OptionsViewer(&graph_manager), 4, 0, 1, 3);
	layout->addWidget(log_manager.viewer(), 5, 0, 1, 3);

	for (int i = 0; i < 4; ++i)
	{
		layout->setColumnMinimumWidth(i, 200);
		layout->setColumnStretch(i, 1);
	}

	for (int i = 0; i < 6; ++i)
		layout->setRowStretch(i, 1);

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

void StatsDisplayWindow::help()
{
}

void StatsDisplayWindow::about()
{
}
