#include <QtCore/QDebug>
#include <QtCore/QTimer>

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

void checkForStartup (const UpdaterPtr& ptr)
{
	Q_ASSERT (ptr.data());
	
	if (ptr->config() ["CheckOnStartup"] == QLatin1String ("true")) {
		ptr->checkForUpdates();
	}
}

void setTimer (const UpdaterPtr& ptr, QTimer *timer)
{
	Q_ASSERT (ptr.data() && timer);
	
	const QString &timerString = ptr->config() ["CheckPeriod"];

	if (timerString.isEmpty()) {
		return;
	}
	
	timer->start(timerString.toInt() * 60 * 1000);//Minutes to msec
}

void UpdatesChecker::appendUpdater (const UpdaterPtr& ptr)
{
	Q_ASSERT (ptr.data ());
	
	updaters_ [ptr] = new QTimer (this);
	connect (updaters_ [ptr], SIGNAL (timeout()),
			 ptr.data(), SLOT (checkForUpdates()));
	connect (ptr.data(), SIGNAL (checkFinished()),
			 this, SLOT (checkFinished()));
	connect (ptr.data(), SIGNAL (downloadFinished()),
			 this, SLOT (downloadFinished()));
	checkForStartup (ptr);
	setTimer (ptr, updaters_ [ptr]);
}

void UpdatesChecker::setUpdaterList (const UpdaterPtrList& l)
{
	if (l.isEmpty()) {
		return;
	}

	for (UpdaterPtrList::const_iterator it = l.constBegin(),
			end = l.constEnd(); it != end; ++it) {
		Q_ASSERT (it->data ());
	
		connect (it->data(), SIGNAL (checkFinished()),
				 this, SLOT (checkFinished()));
		connect (it->data(), SIGNAL (downloadFinished()),
				 this, SLOT (downloadFinished()));
		checkForStartup (*it);
		updaters_ [*it] = new QTimer (this);
		connect (updaters_ [*it], SIGNAL (timeout()),
				 it->data(), SLOT (checkForUpdates()));
		setTimer (*it, updaters_ [*it]);
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
	
	return UpdaterPtr ();
}

void UpdatesChecker::checkFinished ()
{
	AbstractUpdater *u = qobject_cast <AbstractUpdater*> (sender ());
	Q_ASSERT (u);

	const UpdaterPtr &ptr = ptrFromMap (u);

	if (ptr.data () && !ptr->availableUpdates().empty()) {
		emit newUpdatesAvailabel (ptr);
	}
}

void UpdatesChecker::downloadFinished ()
{
	AbstractUpdater *u = qobject_cast <AbstractUpdater*> (sender ());
	Q_ASSERT (u);

	const UpdaterPtr &ptr = ptrFromMap (u);

	if (ptr.data () && !ptr->availableUpdates().empty()) {
		emit downloadFinished (ptr);
	}
}

void UpdatesChecker::checkForUpdates ()
{
	UpdatersIterator it (updaters_);

	while (it.hasNext ()) {
		it.next ();

		it.key ()->checkForUpdates ();
	}
}
}
