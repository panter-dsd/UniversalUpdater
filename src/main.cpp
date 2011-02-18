#include <QtCore/QDebug>

#include <QtGui/QApplication>
#include <QtGui/QStyle>
#include <QtGui/QSystemTrayIcon>
#include <QtGui/QAction>
#include <QtGui/QMenu>

#include "configloader.h"
#include "updateschecker.h"

#include "mainwindow.h"

int main (int argc, char **argv)
{
	QApplication app (argc, argv);

	QSystemTrayIcon trayIcon (QIcon (":/share/images/tray_main_icon.png"));
	trayIcon.show ();
	QObject::connect (&app, SIGNAL (destroyed ()), &trayIcon, SLOT (hide ()));

	QMenu trayContextMenu;
	
	QAction exitAction (app.style ()->standardIcon (QStyle::SP_DialogCloseButton),
						QObject::tr ("Exit"), 0);
	QObject::connect (&exitAction, SIGNAL (triggered ()), &app, SLOT (quit ()));
	trayContextMenu.addAction (&exitAction);
	
	trayIcon.setContextMenu(&trayContextMenu);
	

	Core::UpdatesChecker updatesChecker;

	QSettings settings ("/home/panter/program/UU/share/example/updater.ini",
						QSettings::IniFormat);

	Core::ConfigLoader configLoader (&settings);
	updatesChecker.setUpdaterList (configLoader.readConfig());

	Gui::MainWindow win;
	win.show ();

	return app.exec ();
}
