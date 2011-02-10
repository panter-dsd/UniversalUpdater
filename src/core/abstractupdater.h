#ifndef ABSTRACTUPDATER_H
#define ABSTRACTUPDATER_H

#include <QtCore/QObject>
#include <QtCore/QMap>

#include <vector>

#include "abstractupdateconfig.h"

namespace Core {

typedef QMap <QString, QString> Config;

class AbstractUpdater : public QObject
{
	Q_OBJECT
	
public:
	AbstractUpdater (const Config& config) : config_ (config)
	{}
	virtual ~AbstractUpdater()
	{}

	void checkForUpdates ()
	{
		getUpdateConfig_p ();
	}
	
	ProductVersion currentProductVersion () const
	{ return currentProductVersion_;}
	void setCurrentProductVersion (const ProductVersion& productVersion)
	{ currentProductVersion_ = productVersion;}
	
	ProductVersionList availableUpdates () const;

	void downloadUpdate (const ProductVersion& version,
						 const QString& dir = QString ()) const
	{ return downloadUpdate_p (version, dir);}
	
	void installUpdate (const ProductVersion& version,
					 const QString& dir = QString ()) const
	{ return installUpdate_p (version, dir);}

	bool isFinished () const
	{ return isFinished_p ();}
	
Q_SIGNALS:
	void downloadFinished ();


private:
	void initializeUpdateConfigCache ();
	
private:
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
