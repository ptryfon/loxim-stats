#include <QImage>
#include <QImageReader>
#include <QVBoxLayout>
#include <QPixmap>
#include <QPainter>
#include <QPaintEvent>
#include <QPushButton>
#include <QString>
#include <QTabWidget>

#include <iostream>

#include <SystemStats/StatsDisplay/GUI/GraphViewer.h>

GraphViewer::GraphViewer(QWidget *parent) : QWidget(parent)
{
	QVBoxLayout *layout = new QVBoxLayout;
	QPushButton *button = new QPushButton("delete");

	button->setMaximumSize(button->sizeHint());
	connect(button, SIGNAL(clicked()), this, SLOT(remove_tab()));

	tabs = new QTabWidget;

	layout->addWidget(tabs);
	layout->addWidget(button);
	setLayout(layout);
}

void GraphViewer::add_tab()
{
	tabs->setCurrentIndex(tabs->addTab(new ImageTab("test.png"), "test"));

	return;
}

void GraphViewer::remove_tab()
{
	int i = tabs->currentIndex();

	if (i >= 0)
		tabs->removeTab(i);

	return;
}

void GraphViewer::update_tab()
{
	int index = tabs->currentIndex();

	if (index >= 0)
	{
		tabs->removeTab(index);
		tabs->insertTab(index, new ImageTab("test.png"), "test");
		tabs->setCurrentIndex(index);
	}

	return;
}

ImageTab::ImageTab(const QString &path) : image(path)
{
}

void ImageTab::paintEvent(QPaintEvent *)
{
	//chech whether image size if ok, if not replot
	QPainter painter(this);
	painter.drawPixmap(0, 0, image);
}

