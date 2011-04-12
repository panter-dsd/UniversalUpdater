#include <QtCore/QStringList>
#include <QtCore/QDebug>

#include <algorithm>

#include "configloader.h"

namespace Core
{

struct FindUpdaterPredicate : public std::unary_function <AbstractUpdater, bool> {

public:
	FindUpdaterPredicate (const QString& id) : id_ (id) {}

	bool operator() (AbstractUpdater *u) {
		return u->currentProductVersion().productID() == id_;
	}

private:
	QString id_;
};

AbstractUpdater* ConfigLoader::getUpdater (const Config& config)
{
	const UpdatersList::iterator &end = updaters_.end();
	const UpdatersList::iterator &it = std::find_if (updaters_.begin(),
									   end,
									   FindUpdaterPredicate (config ["ProductID"].toString()));

	AbstractUpdater *updater = it != end

							   ? *it
							   : Core::UpdaterFactory::updaterForProtocol (config.value ("UpdateProtocol").toString ());

	if (it != end) {
		updaters_.erase (it);
	}

	return updater;
}

void clearExcessUpdaters (UpdatersList &l)
{
	while (!l.empty()) {
		if (!(*l.begin())->isWorked()) {
			(*l.begin())->deleteLater();
			l.erase(l.begin());
		}
	}
}

void ConfigLoader::readConfig ()
{
	UpdatersList l;
	settings_->sync();
	settings_->beginGroup ("PRODUCTS");

	foreach (const QString& group, settings_->childGroups()) {
		settings_->beginGroup (group);

		Config config;
		config ["ProductID"] = group;
		foreach (const QString &key, settings_->childKeys()) {
			config [key] = settings_->value (key);
		}

		AbstractUpdater *updater = getUpdater (config);

		if (updater && updater->config() != config) {
			updater->setConfig (config);
		}

		l.push_back (updater);

		settings_->endGroup();
	}

	settings_->endGroup();

	clearExcessUpdaters (updaters_);
	updaters_ = l;

	emit configReaded (l);
}
}





