#ifndef UPDATERFACTORY_H
#define UPDATERFACTORY_H

#include "abstractupdater.h"

namespace Core {

typedef QVector <AbstractUpdater*> UpdaterList;
class UpdaterFactory
{

public:
	static AbstractUpdater* updaterForProtocol (const QString& protocol);

	static void clearCache ();

private:
	UpdaterFactory();
	UpdaterFactory (const UpdaterFactory&);
	UpdaterFactory& operator= (const UpdaterFactory&);

	static void initializeCache ();

private:
	static UpdaterList updaterList_;
};
}
#endif // UPDATERFACTORY_H
