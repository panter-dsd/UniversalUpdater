#include <QtCore/QCoreApplication>
#include <QtCore/QLocale>
#include <QtCore/QDir>
#include <QtCore/QMap>

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
	static const QString postfixes [] = {QObject::tr ("byte"),
										 QObject::tr ("KB"),
										 QObject::tr ("MB"),
										 QObject::tr ("GB")
										};
	static const size_t count = sizeof (postfixes) / sizeof (QString);
	const qint64 tempSize = size >> 10; //size >> 10 == size / 1024

	size_t i = 0;
	qint64 size_ = 1;

	for (; i < count && tempSize >= size_; ++i, size_ = size_ << 10) {}//size << 10 == size * 1024

	return size >= 0
		   ? QString::number (static_cast <double> (size) / size_, 'f', 2) + " " + postfixes [i]
		   : "unknow";
}

QIcon fileIcon (const QString &fileName)
{
	const QFileInfo fi (fileName);

	if (fileName.isEmpty() || !fi.exists ()) {
		return QIcon ();
	}

	QPixmap pixmap;

	return pixmap.load (fi.absoluteFilePath())
		   ? QIcon (pixmap)
		   : QFileIconProvider ().icon (fi);
}

}
