#ifndef CORE_H
#define CORE_H

#include <QtCore/QString>

#include <QtGui/QIcon>

namespace Core
{

template <class T, class U>

struct IsValidPredicate {

public:
	IsValidPredicate (const U& u) : u_ (u) {}

	bool operator() (T *t) const {
		return t->isValid (u_);
	}

private:
	U u_;
};

template <class Container>
void deleteAll (const Container& container)
{
	for (typename Container::const_iterator it = container.begin (),
			end = container.end (); it != end; ++it) {
		delete *it;
	}
}

template <class Container>
void clearContainer (Container &container)
{
	if (container.empty ()) {
		return;
	}

	deleteAll (container);

	container.clear ();
}

QString currentLocale ();

QString savingPath ();
QString stringSize (qint64 size);

QIcon fileIcon (const QString &fileName);

class FlagLocker {
public:
	FlagLocker (bool *flag) : flag_ (flag)
	{invertFlag (flag_);}
	~FlagLocker ()
	{invertFlag (flag_);}
	
private:
	FlagLocker (const FlagLocker&);
	FlagLocker& operator= (const FlagLocker&);
	
	void invertFlag (bool *flag) const
	{*flag_ = !*flag_ ;}
	
private:
	bool *flag_;
};
}

#endif //CORE_H
