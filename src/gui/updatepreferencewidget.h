#ifndef UPDATEPREFERENCEWIDGET_H
#define UPDATEPREFERENCEWIDGET_H

#include <QtGui/QWidget>

#include <memory>

#include "abstractupdater.h"

namespace Ui
{
	
	class UpdatePreferenceWidget;
}

namespace Gui
{
class UpdatePreferenceWidget : public QWidget
{
	Q_OBJECT

public:
	explicit UpdatePreferenceWidget (const Core::UpdaterPtr& updater,
									 QWidget* parent = 0);
	~UpdatePreferenceWidget ();

	bool isChanged () const {
		return isChanged_;
	}

protected:
	virtual void changeEvent (QEvent* e);

private:
	UpdatePreferenceWidget (const UpdatePreferenceWidget&);
	UpdatePreferenceWidget& operator= (const UpdatePreferenceWidget&);

private Q_SLOTS:
	void addSource ();
	void editSource ();
	void removeSource ();
	void setIsChanged (int) {
		isChanged_ = true;
	}

private:
	std::auto_ptr <Ui::UpdatePreferenceWidget> ui_;
	Core::UpdaterPtr updater_;
	bool isChanged_;
};
}
#endif // UPDATEPREFERENCEWIDGET_H
