#ifndef FILE_MANAGER_H_
#define _FILE_MANAGER_H_

#include <QHash>

#include <SystemStats/StatsDisplay/GUI/FileViewer.h>
#include <SystemStats/StatsDisplay/Logic/DirectoryEntry.h>
#include <SystemStats/StatsDisplay/Logic/LogManager.h>

class QString;
class QStringList;

class FileManager
{
	public:
		FileManager(FileViewer *_view, LogManager *_log) : view(_view), log(_log) {}

		FileViewer * viewer() {return view;}

		void open_files(const QStringList &files);
		void open_directory(const QString &dir);
		void close_files(const QStringList &files);
		void close_directory(const QString &dir);
		void clean_directory(const QString &dir);

		void remove(const QString &dir);
		void remove(const QString &dir, const QString &file);
		void clear();

		void change_all_state(Qt::CheckState state);
		void change_state(const QString &dir);
		void change_state(const QString &dir, const QString &file);
		Qt::CheckState get_state(const QString &dir);
		Qt::CheckState get_state(const QString &dir, const QString &file);

	private:
		FileViewer *view;
		LogManager *log;
		QHash<QString, DirectoryEntry> entries;
};

#endif
