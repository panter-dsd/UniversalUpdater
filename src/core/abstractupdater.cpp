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
	ProductVersionList l;

	std::auto_ptr <AbstractUpdateConfig> ptr (UpdateConfigFactory::configForType (config_ ["ConfigType"]));

	if (ptr.get ()) {
		ptr->setCurrentProductVersion (currentProductVersion_);
		ptr->load (updateConfig_);
		l = ptr->availableUpdates ();
	}

	return l;
}
}
