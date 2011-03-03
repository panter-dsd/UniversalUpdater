#include "defaultversionnumbering.h"

const char versionSeperator = '.';

namespace Core
{

bool DefaultVersionNumbering::isEqual_p (const AbstractVersionNumbering& other) const
{
	return parsedVersion_ == static_cast <DefaultVersionNumbering const&> (other).parsedVersion_;
}

bool DefaultVersionNumbering::isSmaller_p (const AbstractVersionNumbering& other) const
{
	const QStringList &ol = static_cast <DefaultVersionNumbering const&> (other).parsedVersion_;

	if (!parsedVersion_.isEmpty() && !ol.isEmpty()) {
		for (QStringList::const_iterator it_0 = parsedVersion_.constBegin (),
				it_1 = ol.constBegin (),
				end_0 = parsedVersion_.constEnd (), end_1 = ol.constEnd ();
				it_0 != end_0 && it_1 != end_1; ++it_0, ++it_1) {
			if (*it_0 != *it_1) {
				return *it_0 < *it_1;
			}
		}
	}

	return parsedVersion_.size () < ol.size ();
}

bool DefaultVersionNumbering::isValid_p (const QString& stringVersion) const
{
	return stringVersion.contains (versionSeperator);
}

void DefaultVersionNumbering::parseVersion_p ()
{
	parsedVersion_ = stringVersion_.split (versionSeperator);
}

AbstractVersionNumbering* DefaultVersionNumbering::clone_p () const
{
	return new DefaultVersionNumbering ();
}
}
