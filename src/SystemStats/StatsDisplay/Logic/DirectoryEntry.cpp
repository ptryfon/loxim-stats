#include <QString>

#include <assert.h>

#include <SystemStats/StatsDisplay/Logic/DirectoryEntry.h>

bool DirectoryEntry::add(const QString &file)
{
	if (files.contains(file))
		return false;
	else
	{
		files.insert(file, Qt::Unchecked);
		++_file_count;
		return true;
	}
}

void DirectoryEntry::remove(const QString &file)
{
	assert(files.contains(file));

	if (files[file] == Qt::Checked)
		--checked_files;
	--_file_count;

	_state = checked_files == _file_count ? Qt::Checked :
		checked_files ? Qt::PartiallyChecked : Qt::Unchecked;

	return;
}

void DirectoryEntry::change_state()
{
	switch (_state)
	{
		case Qt::Checked:
			_state = Qt::Unchecked;
			checked_files = 0;
			break;
		default:
			_state = Qt::Checked;
			checked_files = _file_count;
			break;
	}

	for (QHash<QString, Qt::CheckState>::iterator i = files.begin(); i != files.end(); ++i)
		i.value() = _state;

	return;
}

void DirectoryEntry::change_state(const QString &file)
{
	assert(files.contains(file));

	if (files[file] == Qt::Checked)
	{
		--checked_files;
		files[file] = Qt::Unchecked;
	}
	else
	{
		++checked_files;
		files[file] = Qt::Checked;
	}

	_state = checked_files == _file_count ? Qt::Checked :
		checked_files ? Qt::PartiallyChecked : Qt::Unchecked;

	return;
}

void DirectoryEntry::change_state(Qt::CheckState state)
{
	_state = state;

	checked_files = _state == Qt::Checked ? _file_count : 0;

	for (QHash<QString, Qt::CheckState>::iterator i = files.begin(); i != files.end(); ++i)
		i.value() = _state;

	return;
}
