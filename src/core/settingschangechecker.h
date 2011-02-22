#ifndef SETTINGSCHANGECHECKER_H
#define SETTINGSCHANGECHECKER_H

#include <QtCore/QObject>
#include <QtCore/QSettings>

namespace Core {
namespace Private {
class AbstractSettingsChangeChecker : public QObject
{
	Q_OBJECT

public:
	AbstractSettingsChangeChecker (QSettings* settings, QObject *parent = 0)
	: QObject (parent), settings_ (settings)
	{ Q_ASSERT (settings_);}
	virtual ~AbstractSettingsChangeChecker ()
	{}

Q_SIGNALS:
	void settingsChanged ();

private:
	AbstractSettingsChangeChecker (const AbstractSettingsChangeChecker& other);
	AbstractSettingsChangeChecker& operator= (const AbstractSettingsChangeChecker& other);

protected:
	QSettings *settings_;
};

class RegistrySettingsChangeChecker : public AbstractSettingsChangeChecker
{
	Q_OBJECT
	
public:
	RegistrySettingsChangeChecker (QSettings* settings, QObject *parent = 0);

protected:
	void timerEvent (QTimerEvent*);

private:
	QSettings::SettingsMap readSettings () const;

private:
	QSettings::SettingsMap lastSettins_;
};

class IniSettingsChangeChecker : public AbstractSettingsChangeChecker
{
	Q_OBJECT
	
public:
	IniSettingsChangeChecker (QSettings* settings, QObject *parent = 0);
};
}

class SettingsChangeChecker : public QObject
{
	Q_OBJECT

public:
	SettingsChangeChecker(const QSettings &settings, QObject* parent = 0);

Q_SIGNALS:
	void settingsChanged ();
	
private:
	SettingsChangeChecker (const SettingsChangeChecker& other);
	SettingsChangeChecker& operator= (const SettingsChangeChecker& other);

private:
	QSettings settings_;
	Private::AbstractSettingsChangeChecker *checker_;
};
}
#endif // SETTINGSCHANGECHECKER_H
