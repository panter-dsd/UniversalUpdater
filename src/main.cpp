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

const QString version = "0.0.5.1";

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


	QSettings settings;
	settings.beginGroup ("PRODUCTS");
	settings.beginGroup ("uu");
	settings.setValue ("Name", QObject::tr ("Universal Updater"));
	settings.setValue ("CurrentVersion", app.applicationVersion());
	settings.setValue ("UpdateProtocol", "Web");
	settings.setValue ("ConfigType", "XML");

	{
		QStringList urlList;
		urlList.push_back ("http://192.168.2.7/version.xml");
		urlList.push_back ("http://192.168.2.189/version.xml");
		settings.setValue ("UpdateConfigUrl", urlList);
	}

	settings.setValue ("CheckOnStartup", true);

#ifdef NDEBUG
	settings.setValue ("CheckPeriod", 1);
#else //NDEBUG
	settings.setValue ("CheckPeriod", 10);
#endif //NDEBUG

#ifdef Q_OS_UNIX
	settings.setValue ("Icon", app.applicationDirPath ()
					   + "/../share/images/tray_main_icon.png");
#else //Q_OS_UNIX
	settings.setValue ("Icon", app.applicationFilePath());
#endif //Q_OS_UNIX
	{
		QStringList parametersList;
		parametersList.push_back ("/S");
		settings.setValue ("InstallerParameters", parametersList);
	}

	settings.endGroup();
	settings.endGroup();
	settings.sync();

	Core::UpdatesChecker updatesChecker;

	Gui::MainWindow win (settings);

	QObject::connect (&updatesChecker, SIGNAL (newUpdatesAvailabel (Core::UpdaterPtr)),
					  &win, SLOT (newUpdateAvailable (Core::UpdaterPtr)));
	QObject::connect (&win, SIGNAL (checkForUpdates()),
					  &updatesChecker, SLOT (checkForUpdates()));


	Core::ConfigLoader configLoader (&settings);
	QObject::connect (&configLoader, SIGNAL (configReaded (Core::UpdaterPtrList)),
					  &win, SLOT (setUpdaterList (Core::UpdaterPtrList)));
	QObject::connect (&configLoader, SIGNAL (configReaded (Core::UpdaterPtrList)),
					  &updatesChecker, SLOT (setUpdaterList (Core::UpdaterPtrList)));
	configLoader.readConfig();

	Core::SettingsChangeChecker settingsChangeChecker (settings);
	QObject::connect (&settingsChangeChecker, SIGNAL (settingsChanged()),
					  &configLoader, SLOT (readConfig()));

	return app.exec ();
}
