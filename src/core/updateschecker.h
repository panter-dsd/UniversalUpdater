#ifndef UPDATESCHECKER_H
#define UPDATESCHECKER_H

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QMapIterator>

#include "updaterfactory.h"

class QTimer;

namespace Core {

class UpdatesChecker : public QObject
{
	Q_OBJECT

public:
	explicit UpdatesChecker (QObject* parent = 0);
	~UpdatesChecker();

	void appendUpdater (Core::AbstractUpdater* ptr);

Q_SIGNALS:
	void newUpdatesAvailabel (Core::AbstractUpdater* updater);
	void downloadFinished (Core::AbstractUpdater* updater);

public Q_SLOTS:
	void checkForUpdates ();
	void setUpdaterList (const Core::UpdatersList& l);

private:
	UpdatesChecker (const UpdatesChecker& other);
	UpdatesChecker& operator= (const UpdatesChecker& other);

	void clearUpdaters ();

private Q_SLOTS:
	void checkFinished ();
	void downloadFinished ();

private:
	typedef QMap <AbstractUpdater*, QTimer*> Updaters;
	typedef QMapIterator <AbstractUpdater*, QTimer*> UpdatersIterator;
	Updaters updaters_;
};
}
#endif // UPDATESCHECKER_H
