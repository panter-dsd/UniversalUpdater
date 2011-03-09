#ifndef ABSTRACTUPDATER_H
#define ABSTRACTUPDATER_H

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QSharedPointer>
#include <QtCore/QVariant>

#include "abstractupdateconfig.h"

namespace Core
{

typedef QMap <QString, QVariant> Config;

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

	enum UpdaterStates {
		NoState,
		StopedState,
		CheckState,
		CheckFinishedState,
		DownloadState,
		DownloadFinishedState,
		InstallState,
		InstallFinishedState
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
		errorText_.clear();
		config_ = config;
		currentProductVersion_.setProductID (config_.value ("ProductID").toString());
		currentProductVersion_.setProductVersion (config_.value ("CurrentVersion").toString());
	}

	ProductVersion currentProductVersion () const {
		return currentProductVersion_;
	}

	void setCurrentProductVersion (const ProductVersion& productVersion) {
		lastError_ = NoError;
		errorText_.clear();
		currentProductVersion_ = productVersion;
	}

	ProductVersionList availableUpdates () const;
	ProductVersionList allUpdates () const;

	bool isFinished () const {
		return isFinished_p ();
	}

	UpdaterError lastError () const {
		return lastError_;
	}

	QString errorText () const {
		return errorText_;
	}

	QString productName () const;

	bool isDownloaded (const ProductVersion& version) const {
		return version.empty()
			   ? false
			   : isDownloaded_p (version, savingPath ());
	}

public Q_SLOTS:
	void checkForUpdates () {
		emit stateChanged (CheckState);
		lastError_ = NoError;
		errorText_.clear();

		getUpdateConfig_p ();
	}

	QString downloadUpdate (const ProductVersion& version) {
		emit stateChanged (DownloadState);
		lastError_ = NoError;
		errorText_.clear();

		return config_.isEmpty()
			   ? QString ()
			   : downloadUpdate_p (version, savingPath ());
	}

	void installUpdate (const ProductVersion& version) {
		emit stateChanged (InstallState);
		lastError_ = NoError;
		errorText_.clear();
		
		if (!config_.isEmpty() && isDownloaded (version)) {
			installUpdate_p (version, savingPath ());
		} else {
			lastError_ = InstallError;
		}
	}

	void stopUpdate () {
		if (!isFinished ()) {
			stopUpdate_p ();
		}
	}

Q_SIGNALS:
	void checkFinished ();
	void downloadFinished ();
	void downloadProgress (qint64 bytesReceived, qint64 bytesTotal);
	void stateChanged (AbstractUpdater::UpdaterStates state);

private:
	QString savingPath () const;

private:
	virtual AbstractUpdater *clone_p () const = 0;
	virtual bool isValid_p (const QString& protocol) const = 0;
	virtual void getUpdateConfig_p () = 0;
	virtual QString downloadUpdate_p (const ProductVersion& version,
									  const QString& dir) = 0;
	virtual void installUpdate_p (const ProductVersion& version,
								  const QString& dir) = 0;
	virtual bool isFinished_p () const = 0;
	virtual void stopUpdate_p () = 0;
	virtual bool isDownloaded_p (const ProductVersion& version,
								 const QString& dir) const = 0;

protected:
	Config config_;
	ConfigData updateConfig_;
	ProductVersion currentProductVersion_;
	UpdaterError lastError_;
	mutable ProductVersionList productVersionList_;
	QString errorText_;
};

typedef QSharedPointer <AbstractUpdater> UpdaterPtr;
typedef QVector <UpdaterPtr> UpdaterPtrList;
}

#endif // ABSTRACTUPDATER_H

