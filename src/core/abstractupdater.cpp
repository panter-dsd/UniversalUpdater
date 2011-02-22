#include <QtCore/QDebug>

#include <algorithm>
#include <memory>
#include <assert.h>

#include "core.h"
#include "updateconfigfactory.h"

#include "abstractupdater.h"

namespace Core
{

ProductVersionList AbstractUpdater::availableUpdates () const
{
	const std::auto_ptr <AbstractUpdateConfig> ptr (UpdateConfigFactory::configForType (config_ ["ConfigType"]));

	if (ptr.get ()) {
		ptr->setCurrentProductVersion (currentProductVersion_);
		ptr->load (updateConfig_);
		productVersionList_ = ptr->availableUpdates ();
	}
	
	return productVersionList_;
}

QString AbstractUpdater::productName () const
{
	QString name;
	if (!productVersionList_.empty()) {
		name = productVersionList_.begin()->productNames() [Core::currentLocale()];
	}
	
	return name.isEmpty() ? currentProductVersion_.productID() : name;
}
}
