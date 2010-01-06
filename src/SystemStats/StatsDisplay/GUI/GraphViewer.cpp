#include <QImage>
#include <QImageReader>
#include <QVBoxLayout>

#include <SystemStats/StatsDisplay/GUI/GraphViewer.h>

GraphViewer::GraphViewer(QWidget *parent) : QWidget(parent), image("")
{
	QImageReader reader("test.png", QByteArray("png"));

	image = reader.read();

	update();
	updateGeometry();
}
