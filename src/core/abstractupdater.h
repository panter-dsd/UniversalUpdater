#ifndef ABSTRACTUPDATER_H
#define ABSTRACTUPDATER_H

#include <QtCore/QObject>
#include <QtCore/QMap>

#include <vector>

#include "abstractupdateconfig.h"

namespace Core
{

typedef QMap <QString, QString> Config;

class AbstractUpdater : public QObject
{
	Q_OBJECT

public:
	explicit AbstractUpdater (QObject *parent = 0)
			: QObject (parent) {}

	virtual ~AbstractUpdater() {}

	AbstractUpdater *clone () const {
		return clone_p ();
	}

	bool isValid (const Config& config) const {
		return isValid_p (config);
	}

	void setConfig (const Config& config) {
		config_ = config;
		currentProductVersion_.setProductID (config_ ["ProductID"]);
		currentProductVersion_.setProductVersion (config_ ["CurrentVersion"]);
	}

	ProductVersion currentProductVersion () const {
		return currentProductVersion_;
	}

	void setCurrentProductVersion (const ProductVersion& productVersion) {
		currentProductVersion_ = productVersion;
	}

	ProductVersionList availableUpdates () const;

	void downloadUpdate (const ProductVersion& version,
						 const QString& dir = QString ()) const {
		if (!config_.isEmpty()) {
			downloadUpdate_p (version, dir);
		}
	}

	void installUpdate (const ProductVersion& version,
						const QString& dir = QString ()) const {
		if (!config_.isEmpty()) {
			installUpdate_p (version, dir);
		}
	}

	bool isFinished () const {
		return isFinished_p ();
	}

public Q_SLOTS:
	void checkForUpdates () {
		getUpdateConfig_p ();
	}

Q_SIGNALS:
	void checkFinished ();
	void downloadFinished ();


private:
	void initializeUpdateConfigCache ();

private:
	virtual AbstractUpdater *clone_p () const = 0;
	virtual bool isValid_p (const Config& config) const = 0;
	virtual void getUpdateConfig_p () = 0;
	virtual void downloadUpdate_p (const ProductVersion& version,
								   const QString& dir = QString ()) const = 0;
	virtual void installUpdate_p (const ProductVersion& version,
								  const QString& dir = QString ()) const = 0;
	virtual bool isFinished_p () const = 0;

protected:
	Config config_;
	ConfigData updateConfig_;
	ProductVersion currentProductVersion_;
	typedef std::vector <AbstractUpdateConfig*> UpdateConfigCache;
	static UpdateConfigCache updateConfigCache_;
};
}
#endif // ABSTRACTUPDATER_H
