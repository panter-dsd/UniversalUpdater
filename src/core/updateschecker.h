#ifndef UPDATESCHECKER_H
#define UPDATESCHECKER_H

#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QSharedPointer>

#include "updaterfactory.h"

namespace Core {

typedef QSharedPointer <AbstractUpdater> AbstractUpdaterPtr;
typedef QVector <AbstractUpdaterPtr> AbstractUpdaterList;

class UpdatesChecker : public QObject
{

public:
	UpdatesChecker (QObject* parent = 0);
	~UpdatesChecker();

	void appendUpdater (const AbstractUpdaterPtr& ptr);

protected:
	virtual void timerEvent (QTimerEvent*);

private:
	UpdatesChecker (const UpdatesChecker& other);
	UpdatesChecker& operator= (const UpdatesChecker& other);

private:
	AbstractUpdaterList abstractUpdaterList_;
};
}
#endif // UPDATESCHECKER_H
