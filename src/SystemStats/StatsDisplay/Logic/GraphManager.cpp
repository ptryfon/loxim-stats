#include <QFile>
#include <QProcess>

#include <string>
#include <iostream>

#include <SystemStats/StatsDisplay/Logic/GraphManager.h>

GraphManager::GraphManager(GraphViewer *_gv, FileManager *_fm) : gv(_gv), fm(_fm)
{
	plot = new QProcess;

	plot->start("gnuplot");

	plot->waitForStarted();

	plot->write("set terminal svg\n");

	plot->waitForReadyRead(500);
}

GraphManager::~GraphManager()
{
	QFile f("test.svg");
	f.remove();

	plot->write("exit\n");
	plot->waitForFinished();
	plot->close();

	delete plot;
}

void GraphManager::new_plot()
{
	if (plot_graph())
		gv->add_tab();

	return;
}

void GraphManager::replot()
{
	if (plot_graph())
		gv->update_tab();

	return;
}

bool GraphManager::plot_graph()
{
	QString p("plot ");
	std::string s;

	p += *(fm->get_names());

	if (p.size() == 5)
		return false;

	s = p.toStdString() + '\n';

	plot->write("set output \"test.svg\"\n");
	plot->waitForReadyRead(100);
	plot->write(s.c_str());
	plot->waitForReadyRead(100);

	return true;
}
