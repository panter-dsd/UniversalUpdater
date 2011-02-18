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
	updaters_ [ptr] = new QTimer (this);
	connect (ptr.data(), SIGNAL (checkFinished()),
			 this, SLOT (checkFinished()));
	connect (ptr.data(), SIGNAL (downloadFinished()),
			 this, SLOT (downloadFinished()));
	checkForStartup (ptr);
}

void UpdatesChecker::setUpdaterList (const UpdaterPtrList& l)
{
	if (l.isEmpty()) {
		return;
	}

	for (UpdaterPtrList::const_iterator it = l.constBegin(),
			end = l.constEnd(); it != end; ++it) {
		connect (it->data(), SIGNAL (checkFinished()),
				 this, SLOT (checkFinished()));
		connect (it->data(), SIGNAL (downloadFinished()),
				 this, SLOT (downloadFinished()));
		checkForStartup (*it);
		updaters_ [*it] = new QTimer (this);
	}
}

UpdaterPtr UpdatesChecker::ptrFromMap (AbstractUpdater *u) const
{
	UpdatersIterator it (updaters_);

	while (it.hasNext ()) {
		it.next ();

		if (it.key ().data () == u) {
			return it.key ();
		}
	}
}

void UpdatesChecker::checkFinished ()
{
	AbstractUpdater *u = qobject_cast <AbstractUpdater*> (sender ());

	UpdaterPtr ptr = ptrFromMap (u);

	if (!ptr.isNull() && !ptr->availableUpdates().empty()) {
		emit newUpdatesAvailabel (ptr);
	}
}

void UpdatesChecker::downloadFinished ()
{
	AbstractUpdater *u = qobject_cast <AbstractUpdater*> (sender ());

	UpdaterPtr ptr = ptrFromMap (u);
	
	if (!ptr.isNull() && !ptr->availableUpdates().empty()) {
		emit downloadFinished (ptr);
	}
}
}
