#include <QString>
#include <QStringList>

#include <SystemStats/StatsDisplay/Logic/LogManager.h>

void LogManager::debug(const QString &msg)
{
	QString line = "Debug: " + msg;
	msgs.push_back(line);

	view->new_line(line);
}

void LogManager::error(const QString &msg)
{
	QString line = "Error: " + msg;
	msgs.push_back(line);

	view->new_line(line);
}

void LogManager::info(const QString &msg)
{
	QString line = msg;
	msgs.push_back(line);

	view->new_line(line);
}

void LogManager::warning(const QString &msg)
{
	QString line = "Warning: " + msg;
	msgs.push_back(line);

	view->new_line(line);
}
