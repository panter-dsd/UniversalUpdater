#include <QtCore/QDebug>
#include <QtCore/QTimer>

#include <algorithm>
#include <assert.h>

#include "updateschecker.h"

namespace Core
{
UpdatesChecker::UpdatesChecker (QObject* parent)
		: QObject (parent)
{

}

UpdatesChecker::~UpdatesChecker()
{
	clearUpdaters ();
}

void checkForStartup (AbstractUpdater* updater)
{
	assert (updater);

	if (updater->config().value ("CheckOnStartup").toBool()) {
		updater->checkForUpdates();
	}
}

void setTimer (AbstractUpdater* updater, QTimer *timer)
{
	assert (updater && timer);

	const QString timerString = updater->config().value ("CheckPeriod").toString ();

	bool ok = false;
	const int value = timerString.toInt(&ok);

	if (ok && value != 0) {
		timer->start (value * 60 * 1000);//Minutes to msec
	}
}

void UpdatesChecker::appendUpdater (AbstractUpdater* updater)
{
	assert (updater);

	checkForStartup (updater);

	QTimer *timer = new QTimer (this);
	connect (timer, SIGNAL (timeout()),
			 updater, SLOT (checkForUpdates()));
	setTimer (updater, timer);

	updaters_ [updater] = timer;
}

void UpdatesChecker::setUpdaterList (const Core::UpdatersList& l)
{
	clearUpdaters ();

	if (l.isEmpty()) {
		return;
	}

	for (UpdatersList::const_iterator it = l.constBegin(),
			end = l.constEnd(); it != end; ++it) {
		appendUpdater (*it);
	}
}

void UpdatesChecker::clearUpdaters ()
{
	UpdatersIterator it (updaters_);

	while (it.hasNext()) {
		it.next();

		QTimer *timer = it.value();
		timer->stop();
		delete timer;
	}

	updaters_.clear();
}

void UpdatesChecker::checkForUpdates ()
{
	UpdatersIterator it (updaters_);

	while (it.hasNext ()) {
		it.next ();

		AbstractUpdater *updater = it.key();
		updater->checkForUpdates ();
	}
}
}
