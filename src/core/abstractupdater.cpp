#include <QtCore/QDebug>

#include <algorithm>
#include <memory>
#include <assert.h>

#include "qtxmlupdateconfig.h"

#include "abstractupdater.h"

namespace Core {

AbstractUpdater::UpdateConfigCache AbstractUpdater::updateConfigCache_;

struct UpdateConfigPredicate {
public:
	UpdateConfigPredicate (const ConfigData& data) : data_ (data) {}
	
	bool operator() (AbstractUpdateConfig *config) const
	{
		return config->isValid (data_);
	}

private:
	ConfigData data_;
};


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
	
	const UpdateConfigCache::const_iterator &it = std::find_if (updateConfigCache_.begin (),
																updateConfigCache_.end (),
																UpdateConfigPredicate (updateConfig_));

	if (it != updateConfigCache_.end ()) {
		std::auto_ptr <AbstractUpdateConfig> ptr ((*it)->clone ());

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