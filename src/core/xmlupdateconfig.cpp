#include <QtCore/QXmlStreamReader>
#include <QtCore/QDebug>

#include "xmlupdateconfig.h"

namespace Core
{

AbstractUpdateConfig* XmlUpdateConfig::clone_p () const
{
	return new XmlUpdateConfig;
}

bool XmlUpdateConfig::isValid_p (const QString& type) const
{
	return type == QLatin1String ("XML");
}

void readProductVersion (QXmlStreamReader &reader, ProductVersion &version)
{
	while (!reader.atEnd ()) {
		reader.readNext();

		if (reader.isEndElement()) {
			break;
		}

		const QString &name = reader.name ().toString();

		if (name == QLatin1String ("version")) {
			version.setProductVersion (reader.readElementText());
			continue;
		}

		if (name == QLatin1String ("date")) {
			version.setProductDate (QDate::fromString (reader.readElementText(), "yyyy-MM-dd"));
			continue;
		}

		if (name == QLatin1String ("description")) {
			const QXmlStreamAttributes &attr = reader.attributes ();

			const QString &value = QString::fromUtf8(QByteArray::fromBase64(reader.readElementText().toUtf8 ()));

			if (attr.hasAttribute ("lang")) {
				ProductDescriptions m = version.productDescriptions();
				m [attr.value ("lang").toString() ] = value;
				version.setProductDescriptions (m);
			}

			continue;
		}

		if (name == QLatin1String ("url")) {
#ifdef Q_OS_WIN
			static const QString os = "win";
#endif
#ifdef Q_OS_LINUX
			static const QString os = "linux";
#endif

			const QXmlStreamAttributes &attr = reader.attributes ();
			QString url = reader.readElementText();

			if (!attr.hasAttribute ("os")
					|| attr.value ("os") != os) {
				continue;
			}

			version.setProductUrl (url);

			version.setProductMd5sum (attr.value ("md5sum").toString());
			version.setProductSize (attr.value ("size").toString().toLongLong());
			continue;
		}

		reader.readElementText();
	}
}

ProductNames productNames (const QXmlStreamAttributes &attr)
{
	ProductNames m;

	for (QXmlStreamAttributes::const_iterator it = attr.begin (),
			end = attr.end (); it != end; ++it) {
		const QString &key = it->name ().toString();

		if (key.startsWith ("name_")) {
			m [key.section ('_', 1, 1) ] = it->value().toString();
		}
	}

	return m;
}

ProductVersionList readProductTree (QXmlStreamReader &reader,
									const ProductVersion& currentProductVersion)
{
	ProductVersionList l;

	const QXmlStreamAttributes &attr = reader.attributes ();

	if (!attr.hasAttribute ("id")) {
		return l;
	}

	const QString &productID = attr.value ("id").toString ();

	if (productID != currentProductVersion.productID ()) {
		return l;
	}

	const ProductNames &names = productNames (attr);


	while (!reader.atEnd ()) {
		reader.readNext();

		if (reader.isEndElement()) {
			break;
		}

		if (reader.isStartElement () && reader.name () == QLatin1String ("update")) {
			ProductVersion pv;
			pv.setProductID (productID);
			pv.setProductNames (names);
			readProductVersion (reader, pv);

			if (!pv.productUrl ().isEmpty()) {
				l.insert (pv);
			}
		}
	}

	return l;
}

void XmlUpdateConfig::parseConfig_p ()
{
	productVersionList_.clear ();
	QXmlStreamReader reader (data_);
	reader.readNextStartElement ();//updates

	while (!reader.atEnd ()) {
		reader.readNext();

		if (reader.isStartElement () && reader.name () == QLatin1String ("product")) {
			const ProductVersionList &tmp = readProductTree (reader, currentProductVersion_);

			if (!tmp.empty ()) {
				productVersionList_.insert (tmp.begin (), tmp.end ());
			}
		}
	}
}
}
