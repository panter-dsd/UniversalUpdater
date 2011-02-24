#include <QtGui/QPushButton>

#include "updatepreferencewidget.h"

#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

namespace Gui
{

PreferencesDialog::PreferencesDialog (const QSettings& settings,
									  const Core::UpdaterPtrList& updatersList,
									  QWidget* parent)
		: QDialog (parent), ui_ (new Ui::PreferencesDialog),
		settings_ (settings.fileName(), settings.format()),
		updatersList_ (updatersList)
{
	ui_->setupUi (this);

	ui_->buttonBox->button (QDialogButtonBox::Apply)->setEnabled (false);

	connect (ui_->buttonBox->button (QDialogButtonBox::Apply), SIGNAL (clicked(bool)),
		this, SLOT (save()));
	connect (ui_->buttonBox->button (QDialogButtonBox::Ok), SIGNAL (clicked(bool)),
			 this, SLOT (save()));

	if (!updatersList_.isEmpty()) {
		for (Core::UpdaterPtrList::const_iterator it = updatersList_.begin(),
				end = updatersList_.end(); it != end; ++it) {
			addPage (new UpdatePreferenceWidget (*it, &settings_, this));
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
	ui_->updatePreferences->addItem (widget, widget->windowTitle());

	connect (widget, SIGNAL (preferenceChanged()),
			 this, SLOT (pageChanged()));
}

void PreferencesDialog::pageChanged ()
{
	ui_->buttonBox->button (QDialogButtonBox::Apply)->setEnabled (true);
}

void PreferencesDialog::save ()
{
	for (Pages::const_iterator it = pages_.constBegin(),
			end = pages_.constEnd(); it != end; ++it) {
		if ((*it)->isChanged()) {
			(*it)->save();
		}
	}
	ui_->buttonBox->button (QDialogButtonBox::Apply)->setEnabled (false);
}
}
