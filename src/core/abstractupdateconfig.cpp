#include <QtCore/QDebug>

#include <algorithm>

#include "abstractupdateconfig.h"

namespace Core
{
ProductVersionList AbstractUpdateConfig::availableUpdates () const
{
	ProductVersionList l;

	if (productVersionList_.empty () || currentProductVersion_.empty ()) {
		return l;
	}

	const ProductVersionList::const_iterator &it = productVersionList_.upper_bound (currentProductVersion_);

	if (it != productVersionList_.end ()) {
		l.insert (it, productVersionList_.end ());
	}

	return l;
}
}
