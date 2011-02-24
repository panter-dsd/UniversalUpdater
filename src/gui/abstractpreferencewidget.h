#ifndef ABSTRACTPREFERENCEWIDGET_H
#define ABSTRACTPREFERENCEWIDGET_H

#include <QtGui/QWidget>

namespace Gui {

class AbstractPreferenceWidget : public QWidget
{
	Q_OBJECT

public:
	explicit AbstractPreferenceWidget (QWidget* parent = 0)
	: QWidget (parent), isChanged_ (false)
	{}
	virtual ~AbstractPreferenceWidget () {}
	
	bool isChanged () const {
		return isChanged_;
	}

Q_SIGNALS:
	void preferenceChanged ();

private:
	AbstractPreferenceWidget (const AbstractPreferenceWidget&);
	AbstractPreferenceWidget& operator= (const AbstractPreferenceWidget&);

protected Q_SLOTS:
	void setIsChanged () {
		isChanged_ = true;
		emit preferenceChanged ();
	}

private:
	bool isChanged_;
};
}
#endif // ABSTRACTPREFERENCEWIDGET_H
