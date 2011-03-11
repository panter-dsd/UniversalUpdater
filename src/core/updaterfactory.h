#ifndef UPDATERFACTORY_H
#define UPDATERFACTORY_H

#include "abstractupdater.h"

namespace Core {

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
	static UpdatersList updaterList_;
};
}
#endif // UPDATERFACTORY_H
