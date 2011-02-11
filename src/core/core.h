#ifndef CORE_H
#define CORE_H

namespace Core {

template <class T, class U>
struct IsValidPredicate {
public:
	IsValidPredicate (const U& u) : u_ (u) {}
	
	bool operator() (T *t) const
	{
		return t->isValid (u_);
		}
		
private:
	U u_;
};
}

#endif //CORE_H