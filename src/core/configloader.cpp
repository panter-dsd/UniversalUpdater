#include <QtCore/QStringList>
#include <QtCore/QDebug>

#include "configloader.h"

namespace Core {

UpdaterPtrList ConfigLoader::readConfig (QSettings* settings)
{
	UpdaterPtrList l;

	if (!settings && !settings_) {
		return l;
	}
	
	if (settings) {
		settings_ = settings;
	}

	settings_->beginGroup ("PRODUCTS");

	foreach (const QString& group, settings_->childGroups()) {
		settings_->beginGroup (group);

		Core::Config config;
		config ["ProductID"] = group;
		foreach (const QString &key, settings_->childKeys()) {
			config [key] = settings_->value (key).toString();
		}
		
		UpdaterPtr ptr (Core::UpdaterFactory::updaterForProtocol (config ["UpdateProtocol"]));
		if (!ptr.isNull()) {
			ptr->setConfig (config);
			l.push_back (ptr);
		}
		
		settings_->endGroup();
	}
	
	settings_->endGroup();

	return l;
}
}





