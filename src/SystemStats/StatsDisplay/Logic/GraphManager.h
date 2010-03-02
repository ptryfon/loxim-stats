#ifndef GRAPH_MANAGER_H_
#define GRAPH_MANAGER_H_

#include <QStringList>

#include <SystemStats/StatsDisplay/GUI/GraphViewer.h>
#include <SystemStats/StatsDisplay/Logic/FileManager.h>

class QProcess;
class QString;

class GraphManager
{
	public:
		GraphManager(GraphViewer *gv, FileManager *fm);
		~GraphManager();

		GraphViewer *viewer() {return gv;}

		bool plot_graph(const QStringList &options);
		void file_plot(const QString &path, const QStringList &options);

	private:
		QProcess *plot;
		GraphViewer *gv;
		FileManager *fm;
};

#endif
