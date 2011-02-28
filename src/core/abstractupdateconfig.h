#ifndef ABSTRACTUPDATECONFIG_H
#define ABSTRACTUPDATECONFIG_H

#include <QtCore/QString>
#include <QtCore/QByteArray>

#include <set>

#include "productversion.h"

namespace Core
{

typedef QByteArray ConfigData;
typedef std::set <ProductVersion> ProductVersionList;

class AbstractUpdateConfig
{

public:
	AbstractUpdateConfig ()
	{}
	virtual ~AbstractUpdateConfig () {}

	AbstractUpdateConfig* clone () const {
		return clone_p ();
	}

	bool isValid (const QString& type) const {
		return isValid_p (type);
	}

	void load (const ConfigData& data) {
		data_ = data;

		if (!data_.isEmpty()) {
			parseConfig_p ();
		}
	}

	ProductVersion currentProductVersion () const {
		return currentProductVersion_;
	}

	void setCurrentProductVersion (const ProductVersion& productVersion) {
		currentProductVersion_ = productVersion;
	}

	ProductVersionList availableUpdates () const;
	ProductVersionList allUpdates () const {
		return productVersionList_;
	}

private:
	AbstractUpdateConfig (const AbstractUpdateConfig&);
	AbstractUpdateConfig& operator= (const AbstractUpdateConfig&);

private:
	virtual AbstractUpdateConfig* clone_p () const = 0;
	virtual bool isValid_p (const QString& type) const = 0;
	virtual void parseConfig_p () = 0;

protected:
	ConfigData data_;
	ProductVersion currentProductVersion_;
	ProductVersionList productVersionList_;
};
}
#endif // ABSTRACTUPDATECONFIG_H
