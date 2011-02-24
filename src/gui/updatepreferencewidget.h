#ifndef UPDATEPREFERENCEWIDGET_H
#define UPDATEPREFERENCEWIDGET_H

#include <memory>

#include "abstractupdater.h"

#include "abstractpreferencewidget.h"

namespace Ui
{
	
	class UpdatePreferenceWidget;
}

namespace Gui
{
class UpdatePreferenceWidget : public AbstractPreferenceWidget
{
	Q_OBJECT

public:
	explicit UpdatePreferenceWidget (const Core::UpdaterPtr& updater,
									 QWidget* parent = 0);
	virtual ~UpdatePreferenceWidget ();

protected:
	virtual void changeEvent (QEvent* e);

private:
	UpdatePreferenceWidget (const UpdatePreferenceWidget&);
	UpdatePreferenceWidget& operator= (const UpdatePreferenceWidget&);

private Q_SLOTS:
	void addSource ();
	void editSource ();
	void removeSource ();

private:
	std::auto_ptr <Ui::UpdatePreferenceWidget> ui_;
	Core::UpdaterPtr updater_;
};
}
#endif // UPDATEPREFERENCEWIDGET_H
