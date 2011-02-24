#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QtCore/QSharedPointer>

#include <QtGui/QDialog>

#include <memory>

#include "abstractupdater.h"
#include "abstractpreferencewidget.h"

namespace Ui {
	class PreferencesDialog;
}

namespace Gui {
class PreferencesDialog : public QDialog
{
	Q_OBJECT
	
public:
	PreferencesDialog (const Core::UpdaterPtrList& updatersList,
					   QWidget* parent = 0);
	virtual ~PreferencesDialog();

protected:
	virtual void changeEvent (QEvent* e);

private:
	PreferencesDialog (const PreferencesDialog& other);
	PreferencesDialog& operator= (const PreferencesDialog& other);

	void addPage (AbstractPreferenceWidget* widget);

private Q_SLOTS:
	void pageChanged ();

private:
	std::auto_ptr <Ui::PreferencesDialog> ui_;
	Core::UpdaterPtrList updatersList_;
	QVector <AbstractPreferenceWidget*> pages_;
};
}

#endif // PREFERENCESDIALOG_H
