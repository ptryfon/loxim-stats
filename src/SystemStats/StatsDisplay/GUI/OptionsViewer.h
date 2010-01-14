#ifndef OPTIONS_VIEWER_H_
#define OPTIONS_VIEWER_H_

#include <QGroupBox>

#include <SystemStats/StatsDisplay/Logic/GraphManager.h>

class QWidget;

class OptionsViewer : public QGroupBox
{
	Q_OBJECT
	
	public:
		OptionsViewer(GraphManager *gm, QWidget *parent = 0);

	private:
		GraphManager *gm;

	private slots:
		void new_plot() {return gm->new_plot();}
		void replot() {return gm->replot();}
};

#endif
