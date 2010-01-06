#ifndef GRAPH_MANAGER_H_
#define GRAPH_MANAGER_H_

class QProcess;

class GraphManager
{
	public:
		GraphManager();
		~GraphManager();

	private:
		QProcess *plot;
};

#endif
