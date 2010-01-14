#ifndef GRAPH_VIEWER_H_
#define GRAPH_VIEWER_H_

#include <QWidget>

class QImage;
class QPixmap;
class QPaintEvent;
class QString;
class QTabWidget;

class GraphViewer : public QWidget
{
	Q_OBJECT

	public:
		GraphViewer(QWidget *parent = 0);

	public slots:
		void add_tab();
		void remove_tab();
		void update_tab();

	private:
		QTabWidget *tabs;
};

class ImageTab : public QWidget
{
	Q_OBJECT

	public:
		ImageTab(const QString &path);
		void paintEvent(QPaintEvent *event);

	private:
		QPixmap image;
};

#endif
