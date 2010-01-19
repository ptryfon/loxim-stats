#ifndef GRAPH_MANAGER_H_
#define GRAPH_MANAGER_H_

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

		void new_plot();
		void replot();
		void file_plot(const QString &path);

	private:
		bool plot_graph();

		QProcess *plot;
		GraphViewer *gv;
		FileManager *fm;
};

#endif
