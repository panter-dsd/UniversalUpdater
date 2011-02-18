#ifndef UPDATESCHECKER_H
#define UPDATESCHECKER_H

#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QSharedPointer>

#include "updaterfactory.h"

namespace Core {

typedef QSharedPointer <AbstractUpdater> UpdaterPtr;
typedef QVector <UpdaterPtr> UpdaterPtrList;

class UpdatesChecker : public QObject
{
	Q_OBJECT

public:
	UpdatesChecker (QObject* parent = 0);
	~UpdatesChecker();

	void appendUpdater (const UpdaterPtr& ptr);
	void setUpdaterList (const UpdaterPtrList& l);

Q_SIGNALS:
	void checkFinished (const UpdaterPtr& updater);
	void downloadFinished (const UpdaterPtr& updater);

protected:
	virtual void timerEvent (QTimerEvent*);

private:
	UpdatesChecker (const UpdatesChecker& other);
	UpdatesChecker& operator= (const UpdatesChecker& other);

private Q_SLOTS:
	void checkFinished ();
	void downloadFinished ();

private:
	UpdaterPtrList abstractUpdaterList_;
	QVector <int> timers_;
};
}
#endif // UPDATESCHECKER_H
