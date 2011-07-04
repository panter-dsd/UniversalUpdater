#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QtCore/QSharedPointer>
#include <QtCore/QSettings>

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
	PreferencesDialog (const QSettings& settings,
					   const Core::UpdatersList& updatersList,
					   QWidget* parent = 0);
	virtual ~PreferencesDialog();

protected:
	virtual void changeEvent (QEvent* e);

private:
	PreferencesDialog (const PreferencesDialog& other);
	PreferencesDialog& operator= (const PreferencesDialog& other);

	void addPage (AbstractPreferenceWidget* widget);
	void loadSettings ();
	void saveSettings ();

private Q_SLOTS:
	void pageChanged ();
	void save ();

private:
	std::auto_ptr <Ui::PreferencesDialog> ui_;
	QSettings settings_;
	Core::UpdatersList updatersList_;
	typedef QVector <AbstractPreferenceWidget*> Pages;
	Pages pages_;
};
}

#endif // PREFERENCESDIALOG_H
