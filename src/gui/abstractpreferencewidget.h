#ifndef ABSTRACTPREFERENCEWIDGET_H
#define ABSTRACTPREFERENCEWIDGET_H

#include <QtCore/QSettings>

#include <QtGui/QWidget>

namespace Gui {

class AbstractPreferenceWidget : public QWidget
{
	Q_OBJECT

public:
	explicit AbstractPreferenceWidget (QSettings* settings, QWidget* parent = 0)
	: QWidget (parent), settings_ (settings), isChanged_ (false)
	{}
	virtual ~AbstractPreferenceWidget () {}
	
	bool isChanged () const {
		return isChanged_;
	}

Q_SIGNALS:
	void preferenceChanged ();

public Q_SLOTS:
	void save () {
		savePreference ();
	}

private:
	AbstractPreferenceWidget (const AbstractPreferenceWidget&);
	AbstractPreferenceWidget& operator= (const AbstractPreferenceWidget&);

	virtual void savePreference () = 0;

protected Q_SLOTS:
	void setIsChanged () {
		isChanged_ = true;
		emit preferenceChanged ();
	}

protected:
	QSettings *settings_;

private:
	bool isChanged_;
};
}
#endif // ABSTRACTPREFERENCEWIDGET_H
