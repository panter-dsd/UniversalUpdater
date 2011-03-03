#include <QtCore/QCoreApplication>
#include <QtCore/QLocale>
#include <QtCore/QDir>

#include <QtGui/QFileIconProvider>

#include <core.h>

namespace Core
{
QString currentLocale ()
{
	const QString locale = QLocale::system ().name ().section (QLatin1Char ('_'), 0, 0);
	return locale.isEmpty () ? "en" : locale;
}

QString stringSize (qint64 size)
{
	static const double kb = 1024;
	static const double mb = kb * kb;
	static const double gb = kb * mb;
	static const double tb = kb * gb;
	
	QString str = "%1 %2";
	
	if (size >= tb) {
		str = str.arg (size / tb, 0, 'f', 2).arg ("TB");
	} else {
		if (size >= gb) {
			str = str.arg (size / gb, 0, 'f', 2).arg ("GB");
		} else {
			if (size >= mb) {
				str = str.arg (size / mb, 0, 'f', 2).arg ("MB");
			} else {
				if (size >= kb) {
					str = str.arg (size / kb, 0, 'f', 2).arg ("KB");
				} else {
					str = str.arg (size).arg ("byte");
				}
			}
		}
	}
	
	return str;
}

QIcon fileIcon (const QString &fileName)
{
	QIcon icon;

	const QFileInfo fi (fileName);

	if (fileName.isEmpty() || !fi.exists ()) {
		return icon;
	}

	QPixmap pixmap;
	if (pixmap.load (fi.absoluteFilePath())) {
		icon = QIcon (pixmap);
		return icon;
	}
	
	const QFileIconProvider prov;
	icon = prov.icon (fi);

	return icon;
}

}
