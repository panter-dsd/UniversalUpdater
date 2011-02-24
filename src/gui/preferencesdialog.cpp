#include <QtGui/QPushButton>

#include "updatepreferencewidget.h"

#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

namespace Gui
{

PreferencesDialog::PreferencesDialog (const Core::UpdaterPtrList& updatersList,
									  QWidget* parent)
		: QDialog (parent), ui_ (new Ui::PreferencesDialog),
		updatersList_ (updatersList)
{
	ui_->setupUi (this);

	ui_->buttonBox->button (QDialogButtonBox::Apply)->setEnabled (false);

	if (!updatersList_.isEmpty()) {
		for (Core::UpdaterPtrList::const_iterator it = updatersList_.begin(),
			end = updatersList_.end(); it != end; ++it) {
			addPage (new UpdatePreferenceWidget (*it, this));
			}
	}
}

PreferencesDialog::~PreferencesDialog()
{

}

void PreferencesDialog::changeEvent (QEvent* e)
{
	QWidget::changeEvent (e);

	switch (e->type()) {

		case QEvent::LanguageChange:
			ui_->retranslateUi (this);
			break;

		default:
			break;
	}
}

void PreferencesDialog::addPage (Gui::AbstractPreferenceWidget* widget)
{
	pages_.push_back (widget);
	ui_->updatePreferences->addItem(widget, widget->windowTitle());

	connect (widget, SIGNAL (preferenceChanged()),
			 this, SLOT (pageChanged()));
}

void PreferencesDialog::pageChanged ()
{
	ui_->buttonBox->button (QDialogButtonBox::Apply)->setEnabled (true);
}
}
