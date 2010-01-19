#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QFileDialog>

#include <SystemStats/StatsDisplay/GUI/OptionsViewer.h>

OptionsViewer::OptionsViewer(GraphManager *_gm, QWidget *parent) :
	QGroupBox(parent), gm(_gm)
{
	QHBoxLayout *layout = new QHBoxLayout;
	QVBoxLayout *sublayout = new QVBoxLayout;
	QPushButton *new_button, *replot_button, *file_button;

	setTitle("Options");

	new_button = new QPushButton("Plot new");
	new_button->setMaximumSize(new_button->sizeHint());
	connect(new_button, SIGNAL(clicked()), this, SLOT(new_plot()));

	replot_button = new QPushButton("Replot");
	replot_button->setMaximumSize(replot_button->sizeHint());
	connect(replot_button, SIGNAL(clicked()), this, SLOT(replot()));

	file_button = new QPushButton("Plot to file");
	file_button->setMaximumSize(file_button->sizeHint());
	connect(file_button, SIGNAL(clicked()), this, SLOT(file_plot()));

	sublayout->addWidget(new_button);
	sublayout->addWidget(replot_button);
	sublayout->addWidget(file_button);

	layout->addLayout(sublayout);

	setLayout(layout);
}

void OptionsViewer::file_plot()
{
	QString path;
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(tr("Images (*.png, *jpg, *eps, *svg)"));
	if (dialog.exec())
		gm->file_plot(dialog.selectedFiles().first());

	return;
}
