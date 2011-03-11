#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QSharedPointer>
#include <QtCore/QSettings>

#include <assert.h>

#include "updaterfactory.h"

namespace Core {

class ConfigLoader : public QObject
{
	Q_OBJECT
	
public:
	ConfigLoader (QSettings* settings, QObject* parent = 0)
	: QObject (parent), settings_ (settings)
	{ assert (settings_);}

Q_SIGNALS:
	void configReaded (const Core::UpdatersList& config);

public Q_SLOTS:
	void readConfig ();

private:
	ConfigLoader (const ConfigLoader&);
	ConfigLoader& operator= (const ConfigLoader&);

	AbstractUpdater* getUpdater (const Config& config);
private:
	QSettings *settings_;
	UpdatersList updaters_;
};
}
#endif // CONFIGLOADER_H
