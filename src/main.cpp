#include <QtGui/QApplication>

#include "mainwindow.h"

int main (int argc, char **argv)
{
	QApplication app (argc, argv);

	Gui::MainWindow win;
	win.show ();

	return app.exec ();
}
