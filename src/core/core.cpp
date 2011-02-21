#include <QtCore/QCoreApplication>
#include <QtCore/QLocale>
#include <QtCore/QDir>

#include <core.h>

namespace Core
{
QString currentLocale ()
{
	const QString &locale = QLocale::system ().name ().section (QLatin1Char ('_'), 0, 0);
	return locale.isEmpty () ? "en" : locale;
}

QString savingPath ()
{
	QDir dir = QDir::temp ();
	dir.mkdir (QCoreApplication::applicationName ());
	return dir.absoluteFilePath (QCoreApplication::applicationName ());
}

}
