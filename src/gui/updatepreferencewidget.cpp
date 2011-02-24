#include "updatepreferencewidget.h"
#include "ui_updatepreferencewidget.h"

namespace Gui {

UpdatePreferenceWidget::UpdatePreferenceWidget (const Core::UpdaterPtr& updater,
												QWidget *parent)
: QWidget (parent), ui_ (new Ui::UpdatePreferenceWidget), updater_ (updater)
{
	ui_->setupUi (this);
	setWindowTitle (updater_->productName());

	ui_->currentVersionLabel->setText (updater_->currentProductVersion().productVersion());
	const Core::Config &config = updater_->config ();
	ui_->checkOnStartupEdit->setChecked(config.value("CheckOnStartup").toBool ());
	ui_->checkIntervalEdit->setValue(config.value("CheckPeriod").toInt());
}

UpdatePreferenceWidget::~UpdatePreferenceWidget ()
{
}

void UpdatePreferenceWidget::changeEvent (QEvent* e)
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

}