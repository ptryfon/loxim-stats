#ifndef LOG_MANAGER_H_
#define LOG_MANAGER_H_

class QString;
class QStringList;

#include <SystemStats/StatsDisplay/GUI/LogViewer.h>

class LogManager
{
	public:
		LogManager(LogViewer *_view) : view(_view) {}

		void debug(const QString &msg);
		void error(const QString &msg);
		void info(const QString &msg);
		void warning(const QString &msg);

		LogViewer * viewer() {return view;}

	private:
		LogViewer *view;
		QStringList msgs;
};

#endif
