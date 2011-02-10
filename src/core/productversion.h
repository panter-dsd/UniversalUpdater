#ifndef PRODUCTVERSION_H
#define PRODUCTVERSION_H

#include <QtCore/QString>
#include <QtCore/QVector>

class AbstractVersionNumbering;

namespace Core {
class ProductVersion
{
public:
	ProductVersion ();
	ProductVersion (const ProductVersion& other);
	~ProductVersion ();
	ProductVersion& operator= (const ProductVersion& other);

	bool operator== (const ProductVersion& other) const;
	bool operator< (const ProductVersion& other) const;

	bool empty () const
	{ return !version_;}

	QString productName () const
	{ return name_;}
	void setProductName (const QString& name)
	{ name_ = name;}
	
	QString productID () const
	{ return id_;}
	void setProductID (const QString& id)
	{ id_ = id;}
	
	QString productVersion () const;
	void setProductVersion (const QString& version);
	
	size_t productSize () const
	{ return size_;}
	void setProductSize (size_t size)
	{ size_ = size;}
	
	QString productMd5sum () const
	{ return md5sum_;}
	void setProductMd5sum (const QString& md5sum)
	{ md5sum_ = md5sum;}
	
	QString productDescription () const
	{ return description_;}
	void setProductDescription (const QString& description)
	{ description_ = description;}
	
	QString productUrl () const
	{ return url_;}
	void setProductUrl (const QString& url)
	{ url_ = url;}
	

private:
	void initializeVersionNumberingCache ();
	
private:
	typedef QVector <AbstractVersionNumbering*> VersionNumberingCache;
	static VersionNumberingCache versionNumberingCache_;

	QString name_;
	QString id_;
	AbstractVersionNumbering* version_;
	size_t size_;
	QString md5sum_;
	QString description_;
	QString url_;
};
}
#endif // PRODUCTVERSION_H
