#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>

#include <algorithm>
#include <memory>
#include <assert.h>

#include "core.h"
#include "updateconfigfactory.h"

#include "abstractupdater.h"

namespace Core
{

ProductVersionList AbstractUpdater::availableUpdates () const
{
	const std::auto_ptr <AbstractUpdateConfig> ptr (UpdateConfigFactory::configForType (config_.value ("ConfigType").toString ()));

	if (ptr.get ()) {
		ptr->setCurrentProductVersion (currentProductVersion_);
		ptr->load (updateConfig_);
		productVersionList_ = ptr->availableUpdates ();
	}

	return productVersionList_;
}

ProductVersionList AbstractUpdater::allUpdates () const
{
	const std::auto_ptr <AbstractUpdateConfig> ptr (UpdateConfigFactory::configForType (config_.value ("ConfigType").toString ()));

	if (ptr.get ()) {
		ptr->setCurrentProductVersion (currentProductVersion_);
		ptr->load (updateConfig_);
		productVersionList_ = ptr->allUpdates ();
	}

	return productVersionList_;
}

QString AbstractUpdater::productName () const
{
	const QString name = !productVersionList_.empty()
						 ? productVersionList_.begin()->productNames().value (Core::currentLocale())
						 : config_.value ("Name").toString();

	return name.isEmpty() ? currentProductVersion_.productID () : name;
}

QString AbstractUpdater::savingPath () const
{
	QDir dir = QDir::temp ();
	dir.mkdir (QCoreApplication::applicationName ());
	return dir.absoluteFilePath (QCoreApplication::applicationName ());
}

QString AbstractUpdater::updaterStateText (UpdaterState state) const
{
	QString text;

	switch (sate) {
		case StopedState:
			text = tr ("Process is stopped");
			break;
		case CheckState:
			text = tr ("Checking for new version");
			break;
		case CheckFinishedState:
			text = tr ("Checking is finished");
			break;
		case DownloadState:
			text = tr ("Downloading new verion");
			break;
		case DownloadFinishedState:
			text = tr ("Downloading is finished");
			break;
		case InstallState:
			text = tr ("Installing new verion");
			break;
		case InstallFinishedState:
			text = tr ("Install finished");
			break;
		default:
			break;
	}

	return text;
}
}
