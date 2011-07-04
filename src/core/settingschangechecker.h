#ifndef SETTINGSCHANGECHECKER_H
#define SETTINGSCHANGECHECKER_H

#include <QtCore/QObject>

#include <memory>

class QSettings;
class SettingsChangeCheckerPrivate;

namespace Core {

class SettingsChangeChecker : public QObject
{
	Q_OBJECT

public:
	explicit SettingsChangeChecker(const QSettings &settings, QObject* parent = 0);

Q_SIGNALS:
	void settingsChanged ();

private:
	SettingsChangeChecker (const SettingsChangeChecker& other);
	SettingsChangeChecker& operator= (const SettingsChangeChecker& other);

private:
	typedef std::auto_ptr <SettingsChangeCheckerPrivate> SettingsChangeCheckerPrivatePtr;
	SettingsChangeCheckerPrivatePtr impl_;
};
}
#endif // SETTINGSCHANGECHECKER_H
