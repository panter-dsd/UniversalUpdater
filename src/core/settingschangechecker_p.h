#ifndef SETTINGSCHANGECHECKER_P_H
#define SETTINGSCHANGECHECKER_P_H

#include <QtCore/QObject>
#include <QtCore/QSettings>

#include <assert.h>

class AbstractSettingsChangeChecker : public QObject
{
	Q_OBJECT

public:
	explicit AbstractSettingsChangeChecker (QSettings *settings, QObject *parent = 0)
		: QObject (parent), settings_ (settings) {
		assert (settings_);
	}
	virtual ~AbstractSettingsChangeChecker ()
	{}

Q_SIGNALS:
	void settingsChanged ();

private:
	AbstractSettingsChangeChecker (const AbstractSettingsChangeChecker &other);
	AbstractSettingsChangeChecker &operator= (const AbstractSettingsChangeChecker &other);

protected:
	QSettings *settings_;
};

class RegistrySettingsChangeChecker : public AbstractSettingsChangeChecker
{
	Q_OBJECT

public:
	explicit RegistrySettingsChangeChecker (QSettings *settings, QObject *parent = 0);

protected:
	void timerEvent (QTimerEvent *);

private:
	QSettings::SettingsMap readSettings () const;

private:
	QSettings::SettingsMap lastSettins_;
};

class IniSettingsChangeChecker : public AbstractSettingsChangeChecker
{
	Q_OBJECT

public:
	explicit IniSettingsChangeChecker (QSettings *settings, QObject *parent = 0);

private Q_SLOTS:
	void fileChanged (const QString &) {
		emit settingsChanged ();
	}
};

class SettingsChangeCheckerPrivate : public QObject
{
	Q_OBJECT

public:
	explicit SettingsChangeCheckerPrivate (const QSettings &settings, QObject *parent = 0);

Q_SIGNALS:
	void settingsChanged ();

private:
	SettingsChangeCheckerPrivate (const SettingsChangeCheckerPrivate &other);
	SettingsChangeCheckerPrivate &operator= (const SettingsChangeCheckerPrivate &other);

private:
	QSettings settings_;
	AbstractSettingsChangeChecker *checker_;
};

#endif // SETTINGSCHANGECHECKER_P_H
