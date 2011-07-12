#include <QtCore/QLocale>
#include <QtCore/QDebug>

#include <QtGui/QFileIconProvider>

#include <algorithm>
#include <math.h>

#include <core.h>

namespace Core
{
QString currentLocale ()
{
	const QString locale = QLocale::system ().name ().section (QLatin1Char ('_'), 0, 0);
	return locale.isEmpty () ? "en" : locale;
}

struct StringSize {
	StringSize (const QString &text, size_t number)
		: text_ (text), size_ (pow (1024, number))
	{}

	StringSize (qint64 size) : size_ (size) {}

	QString text () const {
		return text_;
	}

	qint64 size () const {
		return size_;
	}

	bool operator< (const StringSize &ss) const {
		return size_ < ss.size_;
	}

private:
	QString text_;
	qint64 size_;
};

QString stringSize (qint64 size)
{
	static const StringSize postfixes [] = {StringSize (QObject::tr ("byte"), 0),
											StringSize (QObject::tr ("KB"), 1),
											StringSize (QObject::tr ("MB"), 2),
											StringSize (QObject::tr ("GB"), 3)
										   };

	enum {count = sizeof (postfixes) / sizeof (StringSize) };

	const StringSize *const s = std::upper_bound (postfixes,
								postfixes + count,
								StringSize (size)) - 1;

	return size >= 0
		   ? QString::number (static_cast <double> (size) / s->size (), 'f', 2) + " " + s->text ()
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
