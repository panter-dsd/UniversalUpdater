#ifndef DEFAULTVERSIONNUMBERING_H
#define DEFAULTVERSIONNUMBERING_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVector>

#include "abstractversionnumbering.h"

namespace Core {

class DefaultVersionNumbering : public AbstractVersionNumbering
{
public:
	bool operator== (const DefaultVersionNumbering& other) const;
	bool operator< (const DefaultVersionNumbering& other) const;

private:
	virtual bool isValid_p (const QString& stringVersion) const;
	virtual void parseVersion_p ();
	virtual AbstractVersionNumbering* clone_p () const;
	virtual bool isEqual_p (const AbstractVersionNumbering& other) const;
	virtual bool isSmaller_p (const AbstractVersionNumbering& other) const;

private:
	QStringList parsedVersion_;
};
}
#endif // DEFAULTVERSIONNUMBERING_H
