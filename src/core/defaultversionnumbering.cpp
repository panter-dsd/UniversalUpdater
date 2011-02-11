//TEST
#include <iostream>
#include <iterator>
//TEST

#include <algorithm>

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

	for (QStringList::const_iterator it_0 = parsedVersion_.begin (),
			it_1 = ol.begin (), end_0 = parsedVersion_.end (), end_1 = ol.end ();
			it_0 != end_0 && it_1 != end_1; ++it_0, ++it_1) {
		if (*it_0 != *it_1) {
			return *it_0 < *it_1;
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
	parsedVersion_ = stringVersion ().split (versionSeperator);
}

AbstractVersionNumbering* DefaultVersionNumbering::clone_p () const
{
	return new DefaultVersionNumbering ();
}
}
