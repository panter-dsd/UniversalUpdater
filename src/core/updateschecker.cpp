#include <QtCore/QDebug>
#include <QtCore/QTimer>

#include <algorithm>

#include "updateschecker.h"

namespace Core
{
#if QT_VERSION  < 0x040700
template <class T>
bool operator< (const QSharedPointer<T> & ptr1, const QSharedPointer<T> & ptr2)
{
	return ptr1.data () < ptr2.data ();
}

#endif //QT_VERSION

UpdatesChecker::UpdatesChecker (QObject* parent)
		: QObject (parent)
{

}

UpdatesChecker::~UpdatesChecker()
{
	clearUpdaters ();
}

void checkForStartup (const UpdaterPtr& ptr)
{
	Q_ASSERT (ptr.data());

	if (ptr->config().value ("CheckOnStartup") == QLatin1String ("true")) {
		ptr->checkForUpdates();
	}
}

void setTimer (const UpdaterPtr& ptr, QTimer *timer)
{
	Q_ASSERT (ptr.data() && timer);

	const QString &timerString = ptr->config().value ("CheckPeriod");
	qDebug () << timerString;
qDebug () << ptr->currentProductVersion().productID();
	if (timerString.isEmpty()) {
		return;
	}

	timer->start (timerString.toInt() * 60 * 1000);//Minutes to msec
}

void UpdatesChecker::appendUpdater (const UpdaterPtr& ptr)
{
	Q_ASSERT (ptr.data ());

	connect (ptr.data(), SIGNAL (checkFinished()),
			 this, SLOT (checkFinished()));
	connect (ptr.data(), SIGNAL (downloadFinished()),
			 this, SLOT (downloadFinished()));
	checkForStartup (ptr);
	QTimer *timer = new QTimer (this);
	connect (timer, SIGNAL (timeout()),
			 ptr.data(), SLOT (checkForUpdates()));
	updaters_ [ptr] = timer;
	setTimer (ptr, timer);
}

void UpdatesChecker::setUpdaterList (const UpdaterPtrList& l)
{
	clearUpdaters ();

	if (l.isEmpty()) {
		return;
	}

	for (UpdaterPtrList::const_iterator it = l.constBegin(),
			end = l.constEnd(); it != end; ++it) {
		appendUpdater (*it);
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

void UpdatesChecker::clearUpdaters ()
{
	UpdatersIterator it (updaters_);

	while (it.hasNext()) {
		it.next();

		it.value()->deleteLater();
	}

	updaters_.clear();
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
