#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QSharedPointer>
#include <QtCore/QSettings>

#include "updaterfactory.h"

namespace Core {

class ConfigLoader : public QObject
{
	Q_OBJECT
	
public:
	ConfigLoader (QSettings* settings, QObject* parent = 0)
	: QObject (parent), settings_ (settings)
	{ Q_ASSERT (settings_);}
	~ConfigLoader ()
	{}

Q_SIGNALS:
	void configReaded (const UpdaterPtrList& config);

public Q_SLOTS:
	void readConfig ();

private:
	ConfigLoader (const ConfigLoader&);
	ConfigLoader& operator= (const ConfigLoader&);

private:
	QSettings *settings_;
};
}
#endif // CONFIGLOADER_H
