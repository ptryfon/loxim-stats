#include <QHBoxLayout>
#include <QPushButton>
#include <QString>
#include <QTextEdit>
#include <QVBoxLayout>

#include <SystemStats/StatsDisplay/GUI/LogViewer.h>

LogViewer::LogViewer(QWidget *parent) : QGroupBox(parent)
{
	setTitle("Log");

	QHBoxLayout *layout = new QHBoxLayout;

	log = new QTextEdit;
	log->setReadOnly(true);

	layout->addWidget(log);
	create_buttons(layout);

	setLayout(layout);
}

void LogViewer::create_buttons(QHBoxLayout *layout)
{
	QPushButton *save, *clear;
	QVBoxLayout *sub_layout = new QVBoxLayout;

	save = new QPushButton("Save log");
	connect(save, SIGNAL(clicked()), this, SLOT(save()));
	sub_layout->addWidget(save);

	clear = new QPushButton("Clear");
	connect(clear, SIGNAL(clicked()), log, SLOT(clear()));
	sub_layout->addWidget(clear);

	layout->addLayout(sub_layout);

	return;
}

void LogViewer::new_line(const QString &line)
{
	return log->append(line);
}

void LogViewer::save()
{
}
