#ifndef ABSTRACTVERSIONNUMBERING_H
#define ABSTRACTVERSIONNUMBERING_H

#include <QtCore/QString>

namespace Core
{

class AbstractVersionNumbering
{

public:
	virtual ~AbstractVersionNumbering () {}

	bool operator== (const AbstractVersionNumbering& other) const {
		return isEqual_p (other);
	}

	bool operator< (const AbstractVersionNumbering& other) const {
		return isSmaller_p (other);
	}

	AbstractVersionNumbering* clone () const {
		return clone_p ();
	}

	bool isValid (const QString& stringVersion) const {
		return isValid_p (stringVersion);
	}

	QString stringVersion () const {
		return stringVersion_;
	}

	void setStringVersion (const QString& stringVersion) {
		stringVersion_ = stringVersion;
		parseVersion_p ();
	}

private:
	virtual bool isValid_p (const QString& stringVersion) const = 0;
	virtual void parseVersion_p () = 0;
	virtual AbstractVersionNumbering* clone_p () const = 0;
	virtual bool isEqual_p (const AbstractVersionNumbering& other) const = 0;
	virtual bool isSmaller_p (const AbstractVersionNumbering& other) const = 0;

protected:
	QString stringVersion_;
};
}
#endif // ABSTRACTVERSIONNUMBERING_H
