#include <QHBoxLayout>
#include <QHeaderView>
#include <QList>
#include <QPushButton>
#include <QString>
#include <QStringList>
#include <QTreeView>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

#include <assert.h>

#include <SystemStats/StatsDisplay/GUI/FileViewer.h>
#include <SystemStats/StatsDisplay/Logic/FileManager.h>

FileView::FileView(FileViewer *f, QWidget *parent) : QDialog(parent)
{
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(f);
	setLayout(layout);

	setWindowTitle("File Display");

	setMinimumSize(160, 160);
	resize(300, 700);
}

FileViewer::FileViewer(QWidget *parent) : QWidget (parent)
{
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(tree = new QTreeWidget);

	create_buttons(layout);

	setLayout(layout);

	tree->setColumnCount(2);
	tree->setHeaderHidden(true);
	connect(tree, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this,
		SLOT(item_clicked(QTreeWidgetItem *, int)));

	tree->header()->setStretchLastSection(false);
	tree->header()->setResizeMode(0, QHeaderView::Stretch);
	tree->header()->setResizeMode(1, QHeaderView::ResizeToContents);
}

void FileViewer::create_buttons(QVBoxLayout *layout)
{
	QPushButton *b1, *b2;
	QHBoxLayout *sub_layout;

	sub_layout = new QHBoxLayout;
	b1 = new QPushButton("Expand all");
	connect(b1, SIGNAL(clicked()), tree, SLOT(expandAll()));
	b2 = new QPushButton("Collapse all");
	connect(b2, SIGNAL(clicked()), tree, SLOT(collapseAll()));
	sub_layout->addWidget(b1);
	sub_layout->addWidget(b2);
	layout->addLayout(sub_layout);

	sub_layout = new QHBoxLayout;
	b1 = new QPushButton("Select all");
	connect(b1, SIGNAL(clicked()), this, SLOT(select_all()));
	b2 = new QPushButton("Diselect all");
	connect(b2, SIGNAL(clicked()), this, SLOT(diselect_all()));
	sub_layout->addWidget(b1);
	sub_layout->addWidget(b2);
	layout->addLayout(sub_layout);

	sub_layout = new QHBoxLayout;
	b1 = new QPushButton("Remove");
	connect(b1, SIGNAL(clicked()), this, SLOT(remove()));
	b2 = new QPushButton("Clear");
	connect(b2, SIGNAL(clicked()), this, SLOT(clear()));
	sub_layout->addWidget(b1);
	sub_layout->addWidget(b2);
	layout->addLayout(sub_layout);

	return;
}

void FileViewer::add_directory(const QString &dir)
{
	QStringList list(dir);
	QTreeWidgetItem *i = new QTreeWidgetItem(list);

	i->setFlags(Qt::ItemIsSelectable);
	i->setCheckState(1, Qt::Unchecked);

	tree->addTopLevelItem(i);

	return;
}

void FileViewer::remove_directory(const QString &dir)
{
	QTreeWidgetItem *i = tree->findItems(dir, Qt::MatchExactly).first();

	delete tree->takeTopLevelItem(tree->indexOfTopLevelItem(i));

	return;
}

void FileViewer::add_file(const QString &dir, const QString &file)
{
	QStringList list(file);
	QTreeWidgetItem *i = new QTreeWidgetItem(list);

	i->setFlags(Qt::ItemIsSelectable);
	i->setCheckState(1, Qt::Unchecked);

	tree->findItems(dir, Qt::MatchExactly).first()->addChild(i);

	return;
}

void FileViewer::remove_file(const QString &dir, const QString &file)
{
	QList<QTreeWidgetItem *> list = tree->findItems(file, Qt::MatchExactly | Qt::MatchRecursive);
	QTreeWidgetItem *parent = tree->findItems(dir, Qt::MatchExactly).first();

	for (QList<QTreeWidgetItem *>::iterator i = list.begin(); i != list.end(); ++i)
	{
		if ((*i)->parent() == parent)
		{
			parent->removeChild(*i);
			delete *i;
		}
	}

	return;
}

void FileViewer::change_all_state(Qt::CheckState state)
{
	for (int i = 0; i < tree->topLevelItemCount(); ++i)
	{
		QTreeWidgetItem *w = tree->topLevelItem(i);
		w->setCheckState(1, state);

		for (int j = 0; j < w->childCount(); ++j)
			w->child(j)->setCheckState(1, state);
	}

	return;
}

void FileViewer::change_state(const QString &dir, Qt::CheckState state)
{
	tree->findItems(dir, Qt::MatchExactly).first()->setCheckState(1, state);

	return;
}

void FileViewer::change_state(const QString &dir, const QString &file, Qt::CheckState state)
{
	QList<QTreeWidgetItem *> list = tree->findItems(file, Qt::MatchExactly | Qt::MatchRecursive);
	QTreeWidgetItem *parent = tree->findItems(dir, Qt::MatchExactly).first();

	for (QList<QTreeWidgetItem *>::iterator i = list.begin(); i != list.end(); ++i)
	{
		if ((*i)->parent() == parent)
		{
			(*i)->setCheckState(1, state);
			break;
		}
	}

	return;
}

void FileViewer::select_all()
{
	return model->change_all_state(Qt::Checked);
}

void FileViewer::diselect_all()
{
	return model->change_all_state(Qt::Unchecked);
}

void FileViewer::remove()
{
	QString dir, file;
	QTreeWidgetItem *item;
	QList<QTreeWidgetItem *> list = tree->selectedItems();

	if (list.size())
	{
		item = list.first();

		if (tree->indexOfTopLevelItem(item->parent()) < 0)
		{
			dir = item->text(0);
			model->remove(dir);
		}
		else
		{
			dir = item->parent()->text(0);
			file = item->text(0);
			model->remove(dir, file);
		}
	}

	return;
}

void FileViewer::clear()
{
	tree->clear();
	model->clear();
	return;
}

void FileViewer::item_clicked(QTreeWidgetItem *item, int column)
{
	if (column)
	{
		QString dir, file;

		if (tree->indexOfTopLevelItem(item->parent()) < 0)
		{
			dir = item->text(0);
			model->change_state(dir);
		}
		else
		{
			dir = item->parent()->text(0);
			file = item->text(0);
			model->change_state(dir, file);
		}
	}

	return;
}
