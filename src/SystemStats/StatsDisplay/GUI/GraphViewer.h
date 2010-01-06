#ifndef GRAPH_VIEWER_H_
#define GRAPH_VIEWER_H_

#include <QWidget>

class QImage;

class GraphViewer : public QWidget
{
	Q_OBJECT

	public:
		GraphViewer(QWidget *parent = 0);

	private:
		QImage image;
};

#endif
