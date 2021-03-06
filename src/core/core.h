#ifndef CORE_H
#define CORE_H

#include <QtCore/QString>

#include <QtGui/QIcon>

namespace Core
{

template <class T, class U>

struct IsValidPredicate : public std::unary_function <T, bool> {

public:
	IsValidPredicate (const U &u) : u_ (u) {}

	bool operator() (T *t) const {
		return t->isValid (u_);
	}

private:
	U u_;
};

template <class Container>
void deleteAll (const Container &container)
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

template <class T>
void deleteIf (T *&t)
{
	if (t) {
		delete t;
		t = 0;
	}
}

QString currentLocale ();

QString savingPath ();
QString stringSize (qint64 size);

QIcon fileIcon (const QString &fileName);

class FlagLocker
{
public:
	FlagLocker (bool *flag) : flag_ (flag) {
		invertFlag ();
	}
	~FlagLocker () {
		invertFlag ();
	}

private:
	FlagLocker (const FlagLocker &);
	FlagLocker &operator= (const FlagLocker &);

	void invertFlag () const {
		*flag_ = !*flag_ ;
	}

private:
	bool *flag_;
};
}

#endif //CORE_H
