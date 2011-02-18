#include <QtCore/QLocale>

#include <core.h>

namespace Core
{
QString currentLocale ()
{
	const QString &locale = QLocale::system ().name ().section (QLatin1Char ('_'), 0, 0);
	return locale.isEmpty () ? "en" : locale;
}

}
