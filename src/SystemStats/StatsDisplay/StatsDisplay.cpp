#include <QApplication>
#include <SystemStats/StatsDisplay/GUI/StatsDisplayWindow.h>

int main (int argc, char *argv[])
{
   QApplication app (argc, argv);

   StatsDisplayWindow window;

   window.show ();
   return app.exec ();

   return 0;
}
