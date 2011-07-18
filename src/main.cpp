#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QTranslator>

#include <QtGui/QApplication>

#ifdef NDEBUG
#include "qtsingleapplication.h"
#endif

#include "configloader.h"
#include "updateschecker.h"
#include "settingschangechecker.h"
#include "core.h"

#include "mainwindow.h"

const QString version = "0.0.7.3";

int main (int argc, char **argv)
{
#ifndef NDEBUG
	QApplication app (argc, argv);
#else
	QtSingleApplication app (argc, argv);
	app.initialize (true);

	if (app.isRunning ()) {
		return 0;
	}

#endif

	app.setOrganizationDomain ("simicon.com");
	app.setOrganizationName ("Simicon");
	app.setApplicationVersion (version);
	app.setApplicationName ("UniversalUpdater");
	app.setQuitOnLastWindowClosed (false);

	//Translations
	QTranslator progTranslator;
	progTranslator.load (":/share/translations/uu_" + Core::currentLocale () + ".qm");
	app.installTranslator (&progTranslator);

	//Qt translation
	QTranslator qtTranslator;
	qtTranslator.load (":/share/translations/qt_" + Core::currentLocale () + ".qm");
	app.installTranslator (&qtTranslator);


	QSettings settings (QSettings::SystemScope,
						app.organizationName(),
						app.applicationName());

	Core::UpdatesChecker updatesChecker;

	Gui::MainWindow win (settings);

	QObject::connect (&win, SIGNAL (checkForUpdates()),
					  &updatesChecker, SLOT (checkForUpdates()));

	Core::ConfigLoader configLoader (&settings);
	QObject::connect (&configLoader, SIGNAL (configReaded (Core::UpdatersList)),
					  &updatesChecker, SLOT (setUpdaterList (Core::UpdatersList)));
	QObject::connect (&configLoader, SIGNAL (configReaded (Core::UpdatersList)),
					  &win, SLOT (setUpdaterList (Core::UpdatersList)));

	configLoader.readConfig();

	Core::SettingsChangeChecker settingsChangeChecker (settings);
	QObject::connect (&settingsChangeChecker, SIGNAL (settingsChanged()),
					  &configLoader, SLOT (readConfig()));

	return app.exec ();
}
