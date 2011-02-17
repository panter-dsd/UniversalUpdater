#include <assert.h>
#include <algorithm>

#include "core.h"
#include "qtwebupdater.h"

#include "updaterfactory.h"

namespace Core {
	
UpdaterList UpdaterFactory::updaterList_;

UpdaterFactory::UpdaterFactory ()
{
	if (updaterList_.isEmpty ()) {
		initializeCache ();

		assert (!updaterList_.isEmpty ());
	}
}

void UpdaterFactory::initializeCache ()
{
	updaterList_.push_back (new QtWebUpdater);
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
	const UpdaterList::const_iterator &it = std::find_if (updaterList_.constBegin (),
															   updaterList_.constEnd (),
															   UpdaterPredicate (type));

	return it != updaterList_.constEnd () ? (*it)->clone () : 0;
}


}