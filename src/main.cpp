#include <QtCore/QDebug>
#include <QtCore/QStringList>

#include <QtGui/QApplication>

#include "configloader.h"
#include "updateschecker.h"
#include "settingschangechecker.h"

#include "mainwindow.h"

const QString version = "0.0.0.4";

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

	QObject::connect (&updatesChecker, SIGNAL (newUpdatesAvailabel (Core::UpdaterPtr)),
					  &win, SLOT (newUpdateAvailable (Core::UpdaterPtr)));
	QObject::connect (&win, SIGNAL (checkForUpdates()),
					  &updatesChecker, SLOT (checkForUpdates()));
	//win.show ();


	QSettings settings;
	settings.beginGroup ("PRODUCTS");
	settings.beginGroup ("uu");
	settings.setValue ("Name", app.applicationName());
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
	settings.setValue ("CheckPeriod", "1");
#else //NDEBUG
	settings.setValue ("CheckPeriod", "10");
#endif //NDEBUG
	settings.endGroup();
	settings.endGroup();
	settings.sync();

	Core::ConfigLoader configLoader (&settings);
	QObject::connect (&configLoader, SIGNAL (configReaded (UpdaterPtrList)),
					  &updatesChecker, SLOT (setUpdaterList (UpdaterPtrList)));
	configLoader.readConfig();

	Core::SettingsChangeChecker settingsChangeChecker (settings);
	QObject::connect (&settingsChangeChecker, SIGNAL (settingsChanged()),
					  &configLoader, SLOT (readConfig()));

	return app.exec ();
}
