#ifndef LOG_VIEWER_H_
#define LOG_VIEWER_H_

#include <QGroupBox>

class QHBoxLayout;
class QString;
class QTextEdit;

class LogViewer : public QGroupBox
{
	Q_OBJECT
	
	public:
		LogViewer(QWidget *parent = 0);

		void new_line(const QString &line);

	private:
		void create_buttons(QHBoxLayout *layout);

		QTextEdit *log;

	private slots:
		void save();
};

#endif
