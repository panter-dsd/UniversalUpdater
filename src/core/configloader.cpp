#include <QtCore/QStringList>
#include <QtCore/QDebug>

#include "configloader.h"

namespace Core {

void ConfigLoader::readConfig ()
{
	UpdatersList l;

	settings_->sync();
	settings_->beginGroup ("PRODUCTS");

	foreach (const QString& group, settings_->childGroups()) {
		settings_->beginGroup (group);

		Core::Config config;
		config ["ProductID"] = group;
		foreach (const QString &key, settings_->childKeys()) {
			config [key] = settings_->value (key);
		}
		
		AbstractUpdater *updater = Core::UpdaterFactory::updaterForProtocol (config.value("UpdateProtocol").toString ());
		if (updater) {
			updater->setConfig (config);
			l.push_back (updater);
		} else {
			qDebug () << "Protocol "
			+ config.value("UpdateProtocol").toString ()
			+ " not supported";
		}

		settings_->endGroup();
	}
	
	settings_->endGroup();

	emit configReaded (l);
}
}





