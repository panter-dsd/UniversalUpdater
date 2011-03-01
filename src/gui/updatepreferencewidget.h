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
	UpdatePreferenceWidget (const Core::UpdaterPtr& updater,
							QSettings* settings,
							QWidget* parent = 0);
	virtual ~UpdatePreferenceWidget ();

protected:
	virtual void changeEvent (QEvent* e);

private:
	UpdatePreferenceWidget (const UpdatePreferenceWidget&);
	UpdatePreferenceWidget& operator= (const UpdatePreferenceWidget&);

	virtual void savePreference ();
	virtual void loadPreference ();

private Q_SLOTS:
	void setButtonsEnabled ();

private Q_SLOTS:
	void addSource ();
	void editSource ();
	void removeSource ();

	void moveUp ();
	void moveDown ();

private:
	std::auto_ptr <Ui::UpdatePreferenceWidget> ui_;
	Core::UpdaterPtr updater_;
};
}
#endif // UPDATEPREFERENCEWIDGET_H
