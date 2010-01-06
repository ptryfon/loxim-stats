#ifndef OPTIONS_VIEWER_H_
#define OPTIONS_VIEWER_H_

#include <QGroupBox>

class QWidget;

class OptionsViewer : public QGroupBox
{
	Q_OBJECT
	
	public:
		OptionsViewer(QWidget *parent = 0);
};

#endif
