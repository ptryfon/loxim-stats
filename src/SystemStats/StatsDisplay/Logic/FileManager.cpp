#include <QDir>
#include <QString>
#include <QStringList>

#include <assert.h>

#include <iostream>

#include <SystemStats/StatsDisplay/Logic/FileManager.h>

void FileManager::open_files(const QStringList &files)
{
	for (QStringList::const_iterator i = files.constBegin(); i != files.constEnd(); ++i)
	{
		QDir dir(*i);

		assert(dir.cdUp());

		QString dir_path(dir.canonicalPath());
		QString file_path(dir.relativeFilePath(*i));

		QHash<QString, DirectoryEntry>::iterator it = entries.find(dir_path);

		if (it == entries.end())
		{
			it = entries.insert(dir_path, DirectoryEntry());
			view->add_directory(dir_path);
		}

		if (it.value().add(file_path))
			view->add_file(dir_path, file_path);
	}

	return;
}

void FileManager::open_directory(const QString &dir)
{
	QDir directory(dir);
	QString dir_path(directory.canonicalPath());
	QStringList filters, files;
	QHash<QString, DirectoryEntry>::iterator it = entries.find(dir_path);

	if (it == entries.end())
	{
		it = entries.insert(dir_path, DirectoryEntry());
		view->add_directory(dir_path);
	}

	filters << "*.txt";
	directory.setNameFilters(filters);
	files = directory.entryList();

	for (QStringList::const_iterator i = files.constBegin(); i != files.constEnd(); ++i)
	{
		QString file_path(directory.relativeFilePath(*i));

		if (it.value().add(file_path))
			view->add_file(dir_path, file_path);
	}

	clean_directory(dir_path);

	return;
}

void FileManager::clean_directory(const QString &dir)
{
	assert(entries.contains(dir));

	if (!entries[dir].file_count())
	{
		view->remove_directory(dir);
		entries.remove(dir);
		log->info("Removing empty directory: " + dir);
	}

	return;
}

void FileManager::remove(const QString &dir)
{
	assert(entries.contains(dir));

	entries.remove(dir);
	view->remove_directory(dir);
	return;
}

void FileManager::remove(const QString &dir, const QString &file)
{
	assert(entries.contains(dir));

	entries[dir].remove(file);

	view->remove_file(dir, file);
	view->change_state(dir, entries[dir].state());

	clean_directory(dir);

	return;
}

void FileManager::clear()
{
	return entries.clear();
}

void FileManager::change_all_state(Qt::CheckState state)
{
	for (QHash<QString, DirectoryEntry>::iterator i = entries.begin(); i != entries.end(); ++i)
		i.value().change_state(state);

	view->change_all_state(state);

	return;
}

void FileManager::change_state(const QString &dir)
{
	assert(entries.contains(dir));

	entries[dir].change_state();

	view->change_state(dir, entries[dir].state());

	if (entries[dir].file_count())
	{
		entries[dir].first_entry();
		do
		{
			view->change_state(dir, entries[dir].entry_file(), entries[dir].entry_state());
		} while (entries[dir].next_entry());
	}

	return;
}

void FileManager::change_state(const QString &dir, const QString &file)
{
	assert(entries.contains(dir));

	entries[dir].change_state(file);

	view->change_state(dir, file, entries[dir].state(file));
	view->change_state(dir, entries[dir].state());

	return;
}

QString * FileManager::get_names()
{
	QString *ret = new QString;
	uint files = 0;
	QHash<QString, DirectoryEntry>::iterator i;

	for (i = entries.begin(); i != entries.end(); ++i)
	{
		DirectoryEntry d = i.value();

		if (d.file_count())
		{
			d.first_entry();
			do
			{
				if (d.entry_state() == Qt::Checked)
				{
					if (files)
						ret->append(", ");
					ret->append("\'");
					ret->append(i.key() + "/" + d.entry_file() + "\' ");
					++files;
				}
			} while (d.next_entry());
		}
	}

	return ret;
}
