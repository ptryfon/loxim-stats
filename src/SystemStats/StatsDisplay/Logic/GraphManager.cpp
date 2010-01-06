#include <QProcess>

#include <iostream>

#include <SystemStats/StatsDisplay/Logic/GraphManager.h>

GraphManager::GraphManager()
{
	plot = new QProcess;

	plot->start("gnuplot");

	plot->waitForStarted();

	plot->write("set terminal png\n");
	plot->write("set output \"test.png\"\n");

	plot->waitForReadyRead();
}

GraphManager::~GraphManager()
{
	plot->waitForFinished();
	plot->close();

	delete plot;
}
