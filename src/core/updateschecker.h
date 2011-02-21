#ifndef UPDATESCHECKER_H
#define UPDATESCHECKER_H

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QMapIterator>
#include <QtCore/QSharedPointer>
#include <QtCore/QTimer>

#include "updaterfactory.h"

namespace Core {

class UpdatesChecker : public QObject
{
	Q_OBJECT

public:
	explicit UpdatesChecker (QObject* parent = 0);
	~UpdatesChecker();

	void appendUpdater (const UpdaterPtr& ptr);
	void setUpdaterList (const UpdaterPtrList& l);

Q_SIGNALS:
	void newUpdatesAvailabel (const Core::UpdaterPtr& updater);
	void downloadFinished (const Core::UpdaterPtr& updater);

public Q_SLOTS:
	void checkForUpdates ();

protected:
	virtual void timerEvent (QTimerEvent*);

private:
	UpdatesChecker (const UpdatesChecker& other);
	UpdatesChecker& operator= (const UpdatesChecker& other);

	UpdaterPtr ptrFromMap (AbstractUpdater *u) const;

private Q_SLOTS:
	void checkFinished ();
	void downloadFinished ();

private:
	typedef QMap <UpdaterPtr, QTimer*> Updaters;
	typedef QMapIterator <UpdaterPtr, QTimer*> UpdatersIterator;
	Updaters updaters_;
	QVector <int> timers_;
};
}
#endif // UPDATESCHECKER_H
