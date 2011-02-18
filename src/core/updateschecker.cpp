#include <QtCore/QDebug>

#include <algorithm>

#include "updateschecker.h"

namespace Core
{
UpdatesChecker::UpdatesChecker (QObject* parent)
		: QObject (parent)
{

}

UpdatesChecker::~UpdatesChecker()
{

}

void UpdatesChecker::timerEvent (QTimerEvent*)
{

}

void checkForStartup (const UpdaterPtr& ptr)
{
	if (ptr.data()->config() ["CheckOnStartup"] == QLatin1String ("true")) {
		ptr.data()->checkForUpdates();
	}
}

void UpdatesChecker::appendUpdater (const UpdaterPtr& ptr)
{
	abstractUpdaterList_.push_back (ptr);
	connect (ptr.data(), SIGNAL (checkFinished()),
			 this, SLOT (checkFinished()));
	connect (ptr.data(), SIGNAL (downloadFinished()),
			 this, SLOT (downloadFinished()));
	checkForStartup (ptr);
}

void UpdatesChecker::setUpdaterList (const UpdaterPtrList& l)
{
	abstractUpdaterList_ = l;

	if (abstractUpdaterList_.isEmpty()) {
		return;
	}

	for (UpdaterPtrList::const_iterator it = abstractUpdaterList_.constBegin(),
			end = abstractUpdaterList_.constEnd(); it != end; ++it) {
		connect (it->data(), SIGNAL (checkFinished()),
				 this, SLOT (checkFinished()));
		connect (it->data(), SIGNAL (downloadFinished()),
				 this, SLOT (downloadFinished()));
		checkForStartup (*it);
	}
}

class UpdaterPtrListFindPredicate
{

public:
	UpdaterPtrListFindPredicate (AbstractUpdater *u) : u_ (u) {}

	bool operator() (const UpdaterPtr& ptr) const {
		return ptr.data() == u_;
	}

private:
	AbstractUpdater *u_;
};

void UpdatesChecker::checkFinished ()
{
	AbstractUpdater *u = qobject_cast <AbstractUpdater*> (sender ());

	const UpdaterPtrList::const_iterator &it = std::find_if (abstractUpdaterList_.constBegin (),
			abstractUpdaterList_.constEnd (),
			UpdaterPtrListFindPredicate (u));

	if (it != abstractUpdaterList_.constEnd ()
			&& !it->data()->availableUpdates().empty()) {
		emit newUpdatesAvailabel (*it);
	}
}

void UpdatesChecker::downloadFinished ()
{
	AbstractUpdater *u = qobject_cast <AbstractUpdater*> (sender ());

	const UpdaterPtrList::const_iterator &it = std::find_if (abstractUpdaterList_.constBegin (),
			abstractUpdaterList_.constEnd (),
			UpdaterPtrListFindPredicate (u));

	if (it != abstractUpdaterList_.constEnd ()
			&& !it->data()->availableUpdates().empty()) {
		emit downloadFinished (*it);
	}
}
}
