#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>
#include <QButtonGroup>
#include <QIntValidator>

#include <iostream>

#include <SystemStats/StatsDisplay/GUI/OptionsViewer.h>

OptionsViewer::OptionsViewer(GraphManager *_gm, QWidget *parent) :
	QGroupBox(parent), gm(_gm)
{
	QHBoxLayout *layout = new QHBoxLayout;
	QVBoxLayout *sublayout = new QVBoxLayout;
	QGridLayout *gridlayout = new QGridLayout;
	QVBoxLayout *sublayout2 = new QVBoxLayout;
	QButtonGroup *buttons = new QButtonGroup(this);

	QPushButton *new_button, *file_button;

	setTitle("Options");

	new_button = new QPushButton("Plot new");
	new_button->setMaximumSize(new_button->sizeHint());
	connect(new_button, SIGNAL(clicked()), this, SLOT(new_plot()));

	file_button = new QPushButton("Plot to file");
	file_button->setMaximumSize(file_button->sizeHint());
	connect(file_button, SIGNAL(clicked()), this, SLOT(file_plot()));

	terminal = new QComboBox;

	terminal->addItem("terminal 1"); terminal->addItem("terminal 2");
	terminal->addItem("terminal 3"); terminal->addItem("terminal 4");
	terminal->addItem("terminal 5"); terminal->addItem("terminal 6");
	terminal->addItem("terminal 7"); terminal->addItem("terminal 8");
	terminal->addItem("terminal 9");

	sublayout->addWidget(terminal);
	sublayout->addWidget(new_button);
	sublayout->addWidget(file_button);

	gridlayout->addWidget(new QLabel("Title"), 0, 0, 1, 1);
	gridlayout->addWidget(new QLabel("X axis"), 1, 0, 1, 1);
	gridlayout->addWidget(new QLabel("Y axis"), 2, 0, 1, 1);
	gridlayout->addWidget(title = new QLineEdit, 0, 1, 1, 1);
	gridlayout->addWidget(x_axis = new QLineEdit, 1, 1, 1, 1);
	gridlayout->addWidget(y_axis = new QLineEdit, 2, 1, 1, 1);

	buttons->addButton(points = new QRadioButton("Draw points"));
	buttons->addButton(lines = new QRadioButton("Draw lines"));
	buttons->setExclusive(true);

	errorbars = new QCheckBox("Show errorbars");
	errorbars->setTristate(false);

	sublayout2->addWidget(points);
	sublayout2->addWidget(lines);
	sublayout2->addWidget(errorbars);

	layout->addLayout(sublayout);
	layout->addLayout(gridlayout);
	layout->addLayout(sublayout2);

	setLayout(layout);
}

const QStringList & OptionsViewer::get_options()
{
	options.clear();

	options << "set terminal x11 " + terminal->currentText().right(1) + "\n";

	options << "set title \'" + title->displayText() + "\'\n";
	options << "set xlabel \'" + x_axis->displayText() + "\'\n";
	options << "set ylabel \'" + y_axis->displayText() + "\'\n";

	if (lines->isChecked())
		options << " smooth bezier";
	else
		options << "";

	if (errorbars->isChecked())
		options << " with errorbars";
	else
		options << "";

	return options;
}

void OptionsViewer::file_plot()
{
	QString path;
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(tr("Images (*.png, *jpg, *eps, *svg)"));
	if (dialog.exec())
		gm->file_plot(dialog.selectedFiles().first(), get_options());

	return;
}
