#ifndef OPTIONS_VIEWER_H_
#define OPTIONS_VIEWER_H_

#include <QGroupBox>
#include <QLineEdit>
#include <QRadioButton>
#include <QCheckBox>
#include <QComboBox>

#include <SystemStats/StatsDisplay/Logic/GraphManager.h>

class QWidget;

class OptionsViewer : public QGroupBox
{
	Q_OBJECT
	
	public:
		OptionsViewer(GraphManager *gm, QWidget *parent = 0);

	private:
		const QStringList & get_options();

		GraphManager *gm;
		QStringList options;
		QLineEdit *title, *x_axis, *y_axis;
		QRadioButton *points, *lines;
		QCheckBox *errorbars;
		QComboBox *terminal;

	private slots:
		void new_plot() {gm->plot_graph(get_options()); return;}
		void file_plot();
};

#endif
