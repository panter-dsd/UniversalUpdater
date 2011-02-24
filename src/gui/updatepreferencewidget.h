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
public:
	explicit UpdatePreferenceWidget (const Core::UpdaterPtr& updater,
									 QWidget* parent = 0);
	~UpdatePreferenceWidget ();

protected:
	virtual void changeEvent (QEvent* e);

private:
	UpdatePreferenceWidget (const UpdatePreferenceWidget&);
	UpdatePreferenceWidget& operator= (const UpdatePreferenceWidget&);

private:
	std::auto_ptr <Ui::UpdatePreferenceWidget> ui_;
	Core::UpdaterPtr updater_;
};
}
#endif // UPDATEPREFERENCEWIDGET_H
