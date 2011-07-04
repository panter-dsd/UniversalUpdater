#include <assert.h>
#include <algorithm>

#include "core.h"
#include "webupdater.h"

#include "updaterfactory.h"

namespace Core {

UpdatersList UpdaterFactory::updaterList_;

void UpdaterFactory::initializeCache ()
{
	updaterList_.push_back (new WebUpdater);
}

void UpdaterFactory::clearCache ()
{
	clearContainer (updaterList_);
}

AbstractUpdater* UpdaterFactory::updaterForProtocol (const QString& type)
{
	if (updaterList_.isEmpty ()) {
		initializeCache ();

		assert (!updaterList_.isEmpty ());
	}

	typedef IsValidPredicate <AbstractUpdater, QString> UpdaterPredicate;
	const UpdatersList::const_iterator &it = std::find_if (updaterList_.constBegin (),
															   updaterList_.constEnd (),
															   UpdaterPredicate (type));

	return it != updaterList_.constEnd () ? (*it)->clone () : 0;
}


}