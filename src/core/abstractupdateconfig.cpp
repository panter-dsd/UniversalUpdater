#include <QtCore/QDebug>

#include <algorithm>

#include "abstractupdateconfig.h"

namespace Core
{
ProductVersionList AbstractUpdateConfig::availableUpdates () const
{
	return productVersionList_.empty () || currentProductVersion_.empty ()
		   ? ProductVersionList ()
		   : ProductVersionList (productVersionList_.begin (),
								 productVersionList_.lower_bound (currentProductVersion_));
}
}
