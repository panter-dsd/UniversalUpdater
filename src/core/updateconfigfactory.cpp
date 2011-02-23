#include <QtCore/QDebug>

#include <algorithm>
#include <assert.h>

#include "xmlupdateconfig.h"
#include "core.h"

#include "updateconfigfactory.h"

namespace Core {

UpdateConfigList UpdateConfigFactory::updateConfigList_;

UpdateConfigFactory::UpdateConfigFactory ()
{
	if (updateConfigList_.isEmpty ()) {
		initializeCache ();

		assert (!updateConfigList_.isEmpty ());
	}
}

void UpdateConfigFactory::initializeCache ()
{
	updateConfigList_.push_back (new XmlUpdateConfig);
}

void UpdateConfigFactory::clearCache ()
{
	clearContainer (updateConfigList_);
}

AbstractUpdateConfig* UpdateConfigFactory::configForType (const QString& type)
{
	if (updateConfigList_.isEmpty ()) {
		initializeCache ();

		assert (!updateConfigList_.isEmpty ());
	}

	typedef IsValidPredicate <AbstractUpdateConfig, QString> UpdateConfigPredicate;
	const UpdateConfigList::const_iterator &it = std::find_if (updateConfigList_.constBegin (),
															updateConfigList_.constEnd (),
															UpdateConfigPredicate (type));

	return it != updateConfigList_.constEnd () ? (*it)->clone () : 0;
}

}


