#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>

#include <SystemStats/StatsDisplay/GUI/OptionsViewer.h>

OptionsViewer::OptionsViewer(GraphManager *_gm, QWidget *parent) :
	QGroupBox(parent), gm(_gm)
{
	QVBoxLayout *layout = new QVBoxLayout;
	QPushButton *new_button, *replot_button;

	setTitle("Options");

	new_button = new QPushButton("Plot new");
	new_button->setMaximumSize(new_button->sizeHint());
	connect(new_button, SIGNAL(clicked()), this, SLOT(new_plot()));

	replot_button = new QPushButton("Replot");
	replot_button->setMaximumSize(replot_button->sizeHint());
	connect(replot_button, SIGNAL(clicked()), this, SLOT(replot()));

	layout->addWidget(new_button);
	layout->addWidget(replot_button);

	setLayout(layout);
}
