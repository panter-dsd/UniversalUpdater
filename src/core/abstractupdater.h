#ifndef ABSTRACTUPDATER_H
#define ABSTRACTUPDATER_H

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QSharedPointer>

#include <vector>

#include "abstractupdateconfig.h"

namespace Core
{

typedef QMap <QString, QString> Config;

class AbstractUpdater : public QObject
{
	Q_OBJECT

public:
	enum UpdaterError {
		NoError = 0,
		CheckError,
		DownloadError,
		InstallError
	};

public:
	explicit AbstractUpdater (QObject *parent = 0)
	: QObject (parent), lastError_ (NoError) {}

	virtual ~AbstractUpdater() {}

	AbstractUpdater *clone () const {
		return clone_p ();
	}
	
	bool isValid (const QString& protocol) const {
		return isValid_p (protocol);
	}
	
	Config config () const {
		return config_;
	}
	void setConfig (const Config& config) {
		lastError_ = NoError;
		config_ = config;
		currentProductVersion_.setProductID (config_ ["ProductID"]);
		currentProductVersion_.setProductVersion (config_ ["CurrentVersion"]);
	}

	ProductVersion currentProductVersion () const {
		return currentProductVersion_;
	}

	void setCurrentProductVersion (const ProductVersion& productVersion) {
		lastError_ = NoError;
		currentProductVersion_ = productVersion;
	}

	ProductVersionList availableUpdates () const;

	void downloadUpdate (const ProductVersion& version,
						 const QString& dir = QString ()) {
		lastError_ = NoError;
		if (!config_.isEmpty()) {
			downloadUpdate_p (version, dir);
		}
	}

	void installUpdate (const ProductVersion& version,
						const QString& dir = QString ()) {
		lastError_ = NoError;

		if (!config_.isEmpty()) {
			installUpdate_p (version, dir);
		}
	}

	bool isFinished () const {
		return isFinished_p ();
	}
	
	UpdaterError lastError () const {
		return lastError_;
	}

public Q_SLOTS:
	void checkForUpdates () {
		lastError_ = NoError;
		getUpdateConfig_p ();
	}

Q_SIGNALS:
	void checkFinished ();
	void downloadFinished ();

private:
	virtual AbstractUpdater *clone_p () const = 0;
	virtual bool isValid_p (const QString& protocol) const = 0;
	virtual void getUpdateConfig_p () = 0;
	virtual void downloadUpdate_p (const ProductVersion& version,
								   const QString& dir = QString ()) = 0;
	virtual void installUpdate_p (const ProductVersion& version,
								  const QString& dir = QString ()) = 0;
	virtual bool isFinished_p () const = 0;

protected:
	Config config_;
	ConfigData updateConfig_;
	ProductVersion currentProductVersion_;
	UpdaterError lastError_;
};

typedef QSharedPointer <AbstractUpdater> UpdaterPtr;
}
#endif // ABSTRACTUPDATER_H
