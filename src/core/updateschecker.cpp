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

void UpdatesChecker::appendUpdater (const AbstractUpdaterPtr& ptr)
{
	abstractUpdaterList_.push_back (ptr);
}

}