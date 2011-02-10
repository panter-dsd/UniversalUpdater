#include <QtCore/QDebug>

#include <assert.h>
#include <algorithm>

#include "abstractversionnumbering.h"
#include "defaultversionnumbering.h"

#include "productversion.h"

namespace Core {
ProductVersion::VersionNumberingCache ProductVersion::versionNumberingCache_;

ProductVersion::ProductVersion ()
: version_ (0), size_ (0)
{
}

ProductVersion::ProductVersion (const ProductVersion& other)
: names_ (other.names_), id_ (other.id_), version_ (0), date_(other.date_),
size_ (other.size_), md5sum_ (other.md5sum_),
descriptions_ (other.descriptions_), url_ (other.url_)
{
	if (other.version_) {
		version_ = other.version_->clone ();
		version_->setStringVersion (other.version_->stringVersion ());
	}
}

ProductVersion::~ProductVersion ()
{
	if (version_) {
		delete version_;
	}
}

ProductVersion& ProductVersion::operator= (const ProductVersion& other)
{
	if (this != &other) {
		names_ = other.names_;
		id_ = other.id_;
		date_ = other.date_;
		size_ = other.size_;
		md5sum_ = other.md5sum_;
		descriptions_ = other.descriptions_;
		url_ = other.url_;

		if (version_) {
			delete version_;
			version_ = 0;
		}

		if (other.version_) {
			version_ = other.version_->clone ();
			version_->setStringVersion (other.version_->stringVersion ());
		}
	}

	return *this;
}

bool ProductVersion::operator== (const ProductVersion& other) const
{
	return version_ && other.version_ && *version_ == *other.version_;
}

bool ProductVersion::operator< (const ProductVersion& other) const
{
	return version_ && other.version_ && *version_ < *other.version_;
}

QString ProductVersion::productVersion () const
{
	return version_ ? version_->stringVersion () : QString ();
}

struct VersionNumberingCachePredicate {
public:
	VersionNumberingCachePredicate (const QString& version)
	: version_ (version)
	{}

	bool operator() (AbstractVersionNumbering *a) const {
		return a->isValid (version_);
	}

private:
	QString version_;
};

void ProductVersion::setProductVersion (const QString& version)
{
	if (versionNumberingCache_.empty ()) {
		initializeVersionNumberingCache ();

		assert (!versionNumberingCache_.empty ());
	}
	
	if (version_) {
		delete version_;
		version_ = 0;
	}
	
	const VersionNumberingCache::const_iterator &it = std::find_if (versionNumberingCache_.begin (),
																	versionNumberingCache_.end (),
																	VersionNumberingCachePredicate (version));
	
	if (it != versionNumberingCache_.end ()) {
		version_ = (*it)->clone ();
		version_->setStringVersion (version);
	}
}

void ProductVersion::initializeVersionNumberingCache ()
{
	versionNumberingCache_.push_back (new DefaultVersionNumbering);
}
}
