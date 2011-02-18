#include <QtCore/QDebug>

#include "updateschecker.h"

namespace Core {
UpdatesChecker::UpdatesChecker(QObject* parent)
: QObject (parent)
{

}

UpdatesChecker::~UpdatesChecker()
{

}

void UpdatesChecker::timerEvent (QTimerEvent*)
{

}

void UpdatesChecker::appendUpdater (const UpdaterPtr& ptr)
{
	abstractUpdaterList_.push_back (ptr);
}

void UpdatesChecker::setUpdaterList (const UpdaterPtrList& l)
{
	abstractUpdaterList_ = l;
	qDebug () << l.size();
}

}