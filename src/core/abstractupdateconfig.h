#ifndef ABSTRACTUPDATECONFIG_H
#define ABSTRACTUPDATECONFIG_H

#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QByteArray>

#include <set>

#include "productversion.h"

namespace Core {

struct ProductVersionListComparator {
	bool operator() (const ProductVersion& lhs, const ProductVersion& rhs) const
	{ return lhs < rhs;}
};

typedef QByteArray ConfigData;
typedef std::set <ProductVersion, ProductVersionListComparator> ProductVersionList;

class AbstractUpdateConfig
{
public:
	virtual ~AbstractUpdateConfig () {}
	
	AbstractUpdateConfig* clone () const
	{ return clone_p ();}
	
	bool isValid (const ConfigData& data) const
	{ return isValid_p (data);}
	
	void load (const ConfigData& data)
	{
		data_ = data;
		parseConfig_p ();
	}
	
	ProductVersion currentProductVersion () const
	{ return currentProductVersion_;}
	void setCurrentProductVersion (const ProductVersion& productVersion)
	{ currentProductVersion_ = productVersion;}
	
	ProductVersionList availableUpdates () const;

private:
	virtual AbstractUpdateConfig* clone_p () const = 0;
	virtual bool isValid_p (const ConfigData& data) const = 0;
	virtual void parseConfig_p () = 0;

protected:
	ConfigData data_;
	ProductVersion currentProductVersion_;
	ProductVersionList productVersionList_;
};
}
#endif // ABSTRACTUPDATECONFIG_H
