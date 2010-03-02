#include <QFile>
#include <QProcess>

#include <string>

#include <SystemStats/StatsDisplay/Logic/GraphManager.h>

GraphManager::GraphManager(GraphViewer *_gv, FileManager *_fm) : gv(_gv), fm(_fm)
{
	plot = new QProcess;

	plot->start("gnuplot");

	plot->waitForStarted();
}

GraphManager::~GraphManager()
{
	plot->write("exit\n");
	plot->waitForFinished();
	plot->close();

	delete plot;
}

void GraphManager::file_plot(const QString &path, const QStringList &options)
{
	int pos = path.lastIndexOf('.');
	QString ext = "set terminal " + path.right(path.size() - pos - 1) + '\n';
	QString output = "set output \'" + path + "\'\n";

	plot->write(ext.toStdString().c_str());
	plot->waitForReadyRead(100);
	plot->write(output.toStdString().c_str());
	plot->waitForReadyRead(100);

	plot_graph(options);

	return;
}

bool GraphManager::plot_graph(const QStringList &options)
{
	QString p("plot ");
	QStringList n = fm->get_names();

	if (n.isEmpty())
		return false;

	QString t = options.value(0);
	plot->write(t.toStdString().c_str());
	plot->waitForReadyRead(100);

	for (unsigned int i = 1; i < 4; ++i) {
		plot->write(options.value(i).toStdString().c_str());
		plot->waitForReadyRead(100);
	}

	for (QStringList::iterator i = n.begin(); i != n.end(); ++i) {
		p += i != n.begin() ? ", " : " ";
		if (options.value(4).isEmpty())
			p += *i + options.value(5) + " title " + *i;
		else {
			p += *i + " using 1:2" + options.value(4) + " title " + *i + ", " +
				*i + " using 1:2:3" + options.value(5) + " notitle";
		}
	}

	p += "\n";

	plot->write(p.toStdString().c_str());
	plot->waitForReadyRead(100);

	return true;
}

