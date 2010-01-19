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

	//plot->write("set terminal png\n");

	//plot->waitForReadyRead(500);
}

GraphManager::~GraphManager()
{
	//QFile f("test.png");
	//f.remove();

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

void GraphManager::file_plot(const QString &path)
{
	int pos = path.lastIndexOf('.');
	QString ext = "set terminal " + path.right(path.size() - pos - 1) + '\n';
	QString output = "set output \'" + path + "\'\n";

	std::cout << ext.toStdString().c_str() << std::endl;
	std::cout << output.toStdString().c_str() << std::endl;

	plot->write(ext.toStdString().c_str());
	plot->waitForReadyRead(100);
	plot->write(output.toStdString().c_str());
	plot->waitForReadyRead(100);

	plot_graph();

	return;
}

bool GraphManager::plot_graph()
{
	QString p("plot ");
	QString *n = fm->get_names();

	p += *n;
	delete n;

	if (p.size() == 5)
		return false;

	p += '\n';

	plot->write(p.toStdString().c_str());
	plot->waitForReadyRead(100);

	return true;
}

