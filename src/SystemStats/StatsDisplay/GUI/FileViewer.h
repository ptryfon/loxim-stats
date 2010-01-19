#ifndef FILE_VIEWER_H_
#define FILE_VIEWER_H_

#include <QDialog>
#include <QWidget>

class FileManager;
class QPushButton;
class QString;
class QStringList;
class QTreeWidget;
class QTreeWidgetItem;
class QVBoxLayout;

class FileViewer : public QWidget
{
	Q_OBJECT

	public:
		FileViewer(QWidget *parent = 0);

		void connect_model(FileManager *m) {model = m; return;}

		void add_directory(const QString &dir);
		void remove_directory(const QString &dir);
		void add_file(const QString &dir, const QString &file);
		void remove_file(const QString &dir, const QString &file);

		void change_all_state(Qt::CheckState state);
		void change_state(const QString &dir, Qt::CheckState state);
		void change_state(const QString &dir, const QString &file, Qt::CheckState state);

	private:
		void create_buttons(QVBoxLayout *layout);

		FileManager *model;
		QTreeWidget *tree;

	private slots:
		void select_all();
		void diselect_all();
		void remove();
		void clear();
		void item_clicked(QTreeWidgetItem *item, int column);
};

class FileView : public QDialog
{
	Q_OBJECT

	public:
		FileView(FileViewer *f, QWidget *parent = 0);
};

#endif
