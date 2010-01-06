#ifndef DIRECTORY_ENTRY_H_
#define DIRECTORY_ENTRY_H_

#include <QHash>
#include <QPair>

class QString;

class DirectoryEntry
{
	public:
		DirectoryEntry() : _file_count(0), checked_files(0), _state(Qt::Unchecked),
			it(files.end()) {}

		bool add(const QString &file);
		void remove(const QString &file);
		bool contains(const QString &file) {return files.contains(file);}

		void change_state();
		void change_state(const QString &file);
		void change_state(Qt::CheckState state);

		int file_count() {return _file_count;}
		Qt::CheckState state() {return _state;}
		Qt::CheckState state(const QString &file) {return files[file];}

		const QString & entry_file() {return it.key();}
		Qt::CheckState entry_state() {return it.value();}
		bool next_entry() {return ++it != files.end();}
		void first_entry() {it = files.begin(); return;}

	private:
		unsigned int _file_count, checked_files;
		Qt::CheckState _state;
		QHash<QString, Qt::CheckState> files;
		QHash<QString, Qt::CheckState>::iterator it;
};

#endif
