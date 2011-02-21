#include <QtCore/QDebug>

#include <QtGui/QApplication>

#include "configloader.h"
#include "updateschecker.h"

#include "mainwindow.h"

const QString version = "0.0.0.0";

int main (int argc, char **argv)
{
	QApplication app (argc, argv);
	app.setOrganizationDomain ("simicon.com");
	app.setOrganizationName ("Simicon");
	app.setApplicationVersion (version);
	app.setApplicationName ("UU");
	app.setQuitOnLastWindowClosed (false);

	Core::UpdatesChecker updatesChecker;
	
	Gui::MainWindow win;

	QObject::connect (&updatesChecker, SIGNAL (newUpdatesAvailabel(Core::UpdaterPtr)),
					  &win, SLOT (newUpdateAvailable(Core::UpdaterPtr)));
	QObject::connect (&win, SIGNAL (checkForUpdates()),
					  &updatesChecker, SLOT (checkForUpdates()));
	//win.show ();


	QSettings settings ("/home/panter/program/UU/share/example/updater.ini",
						QSettings::IniFormat);
	
	Core::ConfigLoader configLoader (&settings);
	updatesChecker.setUpdaterList (configLoader.readConfig());

	return app.exec ();
}
