#ifndef QTXMLUPDATECONFIG_H
#define QTXMLUPDATECONFIG_H

#include "abstractupdateconfig.h"

namespace Core
{

class XmlUpdateConfig : public AbstractUpdateConfig
{

private:
	virtual AbstractUpdateConfig* clone_p () const;
	virtual bool isValid_p (const QString& type) const;
	virtual void parseConfig_p ();
};
}
#endif // QTXMLUPDATECONFIG_H
