#include <QtCore/QDebug>

#include <QtGui/QApplication>

#include "configloader.h"
#include "updateschecker.h"

#include "mainwindow.h"

int main (int argc, char **argv)
{
	QApplication app (argc, argv);
	app.setQuitOnLastWindowClosed (false);

	Core::UpdatesChecker updatesChecker;
	
	Gui::MainWindow win;
	QObject::connect (&updatesChecker, SIGNAL (newUpdatesAvailabel(Core::UpdaterPtr)),
		&win, SLOT (newUpdateAvailable(Core::UpdaterPtr)));
	//win.show ();


	QSettings settings ("/home/panter/program/UU/share/example/updater.ini",
						QSettings::IniFormat);
	
	Core::ConfigLoader configLoader (&settings);
	updatesChecker.setUpdaterList (configLoader.readConfig());

	return app.exec ();
}
