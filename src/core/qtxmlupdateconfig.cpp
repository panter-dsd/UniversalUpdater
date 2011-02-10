#include <QtCore/QXmlStreamReader>
#include <QtCore/QDebug>

#include "qtxmlupdateconfig.h"

const QString attributeName = "id";

namespace Core {

AbstractUpdateConfig* QtXmlUpdateConfig::clone_p () const
{
	return new QtXmlUpdateConfig;
}

bool QtXmlUpdateConfig::isValid_p (const ConfigData& data) const
{
	QXmlStreamReader reader (data);

	return reader.readNextStartElement()
	&& reader.name () == QLatin1String ("updates");
}

void readProductVersion (QXmlStreamReader &reader, ProductVersion &version)
{
	while (!reader.atEnd ()) {
		reader.readNext();

		if (reader.isEndElement()) {
			break;
		}
		
		if (reader.name() == QLatin1String ("version")) {
			version.setProductVersion (reader.readElementText());
			continue;
		}
		if (reader.name() == QLatin1String ("size")) {
			version.setProductSize (reader.readElementText().toLongLong ());
			continue;
		}
		if (reader.name() == QLatin1String ("md5sum")) {
			version.setProductMd5sum (reader.readElementText());
			continue;
		}
		if (reader.name() == QLatin1String ("description")) {
			version.setProductDescription (reader.readElementText());
			continue;
		}
		if (reader.name() == QLatin1String ("url")) {
			version.setProductUrl (reader.readElementText());
			continue;
		}
		
		reader.readElementText();
	}
}

ProductVersionList readProductTree (QXmlStreamReader &reader,
									const ProductVersion& currentProductVersion)
{
	ProductVersionList l;

	const QXmlStreamAttributes &attr = reader.attributes ();
	if (!attr.hasAttribute (attributeName)) {
		return l;
	}
	
	const QString &productName = attr.value(attributeName).toString ();

	if (productName != currentProductVersion.productID ()) {
		return l;
	}

	while (!reader.atEnd ()) {
		reader.readNext();

		if (reader.isEndElement()) {
			break;
		}

		if (reader.isStartElement () && reader.name () == QLatin1String ("update")) {
			ProductVersion pv;
			pv.setProductName (productName);
			readProductVersion (reader, pv);
			l.insert (pv);
		}
	}

	return l;
}

void QtXmlUpdateConfig::parseConfig_p ()
{
	productVersionList_.clear ();
	QXmlStreamReader reader (data_);
	reader.readNextStartElement ();//updates
	
	while (!reader.atEnd ()) {
		reader.readNext();

		if (reader.isStartElement () && reader.name () == QLatin1String ("project")) {
			const ProductVersionList &tmp = readProductTree (reader, currentProductVersion_);
			if (!tmp.empty ()) {
				productVersionList_.insert (tmp.begin (), tmp.end ());
			}
		}
	}
}
}