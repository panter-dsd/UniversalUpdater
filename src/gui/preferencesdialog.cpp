#include <QtGui/QPushButton>

#include "updatepreferencewidget.h"

#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

namespace Gui {

PreferencesDialog::PreferencesDialog (QWidget* parent)
: QDialog (parent), ui_ (new Ui::PreferencesDialog)
{
	ui_->setupUi (this);

	ui_->buttonBox->button (QDialogButtonBox::Apply)->setEnabled (false);
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
	pages_.push_back (PagePtr (widget));

	connect (widget, SIGNAL (preferenceChanged()),
		this, SLOT (pageChanged()));
}

void PreferencesDialog::pageChanged ()
{
	ui_->buttonBox->button (QDialogButtonBox::Apply)->setEnabled (true);
}
}