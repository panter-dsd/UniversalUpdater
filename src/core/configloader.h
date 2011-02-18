#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

#include <QtCore/QVector>
#include <QtCore/QSharedPointer>
#include <QtCore/QSettings>

#include "updaterfactory.h"

namespace Core {

class ConfigLoader
{
public:
	ConfigLoader (QSettings* settings = 0)
	: settings_ (settings)
	{}
	~ConfigLoader ()
	{}

	UpdaterPtrList readConfig (QSettings* settings = 0);

private:
	ConfigLoader (const ConfigLoader&);
	ConfigLoader& operator= (const ConfigLoader&);

private:
	QSettings *settings_;
};
}
#endif // CONFIGLOADER_H
