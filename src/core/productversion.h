#ifndef PRODUCTVERSION_H
#define PRODUCTVERSION_H

#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QMap>
#include <QtCore/QDate>

class AbstractVersionNumbering;

namespace Core
{

typedef QMap <QString, QString> ProductNames;
typedef QMap <QString, QString> ProductDescriptions;

class ProductVersion
{

public:
	ProductVersion ();
	ProductVersion (const ProductVersion& other);
	~ProductVersion ();
	ProductVersion& operator= (const ProductVersion& other);

	bool operator== (const ProductVersion& other) const;
	bool operator< (const ProductVersion& other) const;

	bool empty () const {
		return !version_;
	}

	ProductNames productNames () const {
		return names_;
	}

	void setProductNames (const ProductNames& name) {
		names_ = name;
	}

	QString productID () const {
		return id_;
	}

	void setProductID (const QString& id) {
		id_ = id;
	}

	QString productVersion () const;
	void setProductVersion (const QString& version);

	QDate productDate () const {
		return date_;
	}

	void setProductDate (const QDate& date) {
		date_ = date;
	}

	size_t productSize () const {
		return size_;
	}

	void setProductSize (size_t size) {
		size_ = size;
	}

	QString productMd5sum () const {
		return md5sum_;
	}

	void setProductMd5sum (const QString& md5sum) {
		md5sum_ = md5sum;
	}

	ProductDescriptions productDescriptions () const {
		return descriptions_;
	}

	void setProductDescriptions (const ProductDescriptions& description) {
		descriptions_ = description;
	}

	QString productUrl () const {
		QString tmp = url_;
		tmp.replace ("{VERSION}", productVersion());
		return tmp;
	}

	void setProductUrl (const QString& url) {
		url_ = url;
	}

private:
	void initializeVersionNumberingCache ();

private:
	typedef QVector <AbstractVersionNumbering*> VersionNumberingCache;
	static VersionNumberingCache versionNumberingCache_;

	ProductNames names_;
	QString id_;
	AbstractVersionNumbering* version_;
	QDate date_;
	size_t size_;
	QString md5sum_;
	ProductDescriptions descriptions_;
	QString url_;
};
}
#endif // PRODUCTVERSION_H
