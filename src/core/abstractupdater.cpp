#include <QtCore/QDebug>

#include <algorithm>
#include <memory>
#include <assert.h>

#include "core.h"
#include "qtxmlupdateconfig.h"

#include "abstractupdater.h"

namespace Core
{

AbstractUpdater::UpdateConfigCache AbstractUpdater::updateConfigCache_;

ProductVersionList AbstractUpdater::availableUpdates () const
{
	ProductVersionList l;

	if (updateConfigCache_.empty ()) {
		AbstractUpdater *a = const_cast <AbstractUpdater*> (this);
		a->initializeUpdateConfigCache ();

		assert (!updateConfigCache_.empty ());
	}

	if (updateConfig_.isEmpty ()) {
		return l;
	}

	typedef IsValidPredicate <AbstractUpdateConfig, ConfigData> UpdateConfigPredicate;

	const UpdateConfigCache::const_iterator &it = std::find_if (updateConfigCache_.begin (),
			updateConfigCache_.end (),
			UpdateConfigPredicate (updateConfig_));

	if (it != updateConfigCache_.end ()) {
		std::auto_ptr <AbstractUpdateConfig> ptr ( (*it)->clone ());

		ptr->setCurrentProductVersion (currentProductVersion_);
		ptr->load (updateConfig_);
		l = ptr->availableUpdates ();
	}

	return l;
}

void AbstractUpdater::initializeUpdateConfigCache ()
{
	updateConfigCache_.push_back (new QtXmlUpdateConfig);
}
}
