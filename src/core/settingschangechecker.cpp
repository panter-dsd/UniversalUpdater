#include <QtCore/QFileSystemWatcher>
#include <QtCore/QStringList>
#include <QtCore/QDebug>

#include "settingschangechecker.h"

const int registryCheckInterval = 1 * 1000 * 30; //30 seconds

namespace Core
{
namespace Private {

RegistrySettingsChangeChecker::RegistrySettingsChangeChecker (QSettings* settings,
															  QObject *parent)
: AbstractSettingsChangeChecker (settings, parent)
{
	lastSettins_ = readSettings ();
	startTimer (registryCheckInterval);
}

void RegistrySettingsChangeChecker::timerEvent (QTimerEvent*)
{
	const QSettings::SettingsMap &currentSettings = readSettings ();

	if (lastSettins_ != currentSettings) {
		lastSettins_ = currentSettings;
		emit settingsChanged ();
	}
}

QSettings::SettingsMap RegistrySettingsChangeChecker::readSettings () const
{
	QSettings::SettingsMap result;

	foreach (const QString &key, settings_->allKeys()) {
		result [key] = settings_->value (key);
	}
	
	return result;
}

IniSettingsChangeChecker::IniSettingsChangeChecker (QSettings* settings,
													QObject *parent)
: AbstractSettingsChangeChecker (settings, parent)
{
	QFileSystemWatcher *watcher = new QFileSystemWatcher (this);
	connect (watcher, SIGNAL (fileChanged(QString)),
			 this, SLOT (fileChanged(QString)));
	
	watcher->addPath (settings_->fileName ());
	qDebug () << settings_->fileName ();
}
}

SettingsChangeChecker::SettingsChangeChecker (const QSettings& settings, QObject* parent)
: QObject (parent), settings_ (settings.fileName(), settings.format()), checker_ (0)
{
	switch (settings_.format ()) {

		case QSettings::NativeFormat:
			#ifdef Q_OS_WIN
			checker_ = new Private::RegistrySettingsChangeChecker (&settings_, this);
			#else //Q_OS_WIN
			checker_ = new Private::IniSettingsChangeChecker (&settings_, this);
			#endif //Q_OS_WIN
			break;

		case QSettings::IniFormat:
			checker_ = new Private::IniSettingsChangeChecker (&settings_, this);
			break;

		default:
			break;
	}
	
	if (checker_) {
		connect (checker_, SIGNAL (settingsChanged()),
				 this, SIGNAL (settingsChanged()));
	}
}

}
