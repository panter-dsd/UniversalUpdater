#ifndef UPDATECONFIGFACTORY_H
#define UPDATECONFIGFACTORY_H

#include <QtCore/QVector>

#include "abstractupdateconfig.h"

namespace Core {

typedef QVector <AbstractUpdateConfig*> UpdateConfigList;

class UpdateConfigFactory
{
public:
	UpdateConfigFactory ();
	static AbstractUpdateConfig* configForType (const QString& type);
	
private:
	UpdateConfigFactory (const UpdateConfigFactory&);
	UpdateConfigFactory& operator= (const UpdateConfigFactory&);

	static void initializeCache ();

private:
	static UpdateConfigList updateConfigList_;
};
}
#endif // UPDATECONFIGFACTORY_H
