#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>

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
	ui_->sourcesList->addItems(config.value ("UpdateConfigUrl").toStringList());

	connect (ui_->addSourceButton, SIGNAL (clicked()),
		this, SLOT (addSource()));
	connect (ui_->editSourceButton, SIGNAL (clicked()),
			 this, SLOT (editSource()));
	connect (ui_->removeSourceButton, SIGNAL (clicked()),
			 this, SLOT (removeSource()));
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

void UpdatePreferenceWidget::addSource ()
{
	const QString &url = QInputDialog::getText(this,
											   tr ("Adding source"),
											   tr ("Source"));

	if (!url.isEmpty()) {
		ui_->sourcesList->addItem(url);
	}
}

void UpdatePreferenceWidget::editSource ()
{
	const QListWidgetItem *item = ui_->sourcesList->currentItem();
	if (!item) {
		return;
	}

	const QString &url = QInputDialog::getText(this,
											   tr ("Editing source"),
											   tr ("Source"),
											   QLineEdit::Normal,
											item->text());
	
	if (!url.isEmpty()) {
		ui_->sourcesList->addItem(url);
	}
}

void UpdatePreferenceWidget::removeSource ()
{
	if (ui_->sourcesList->count() == 1) {
		QMessageBox::critical (this,
							   windowTitle(),
							   tr ("Must be at least one source."));
		return;
	}
	
	const QListWidgetItem *item = ui_->sourcesList->currentItem();
	if (!item) {
		return;
	}

	if (item) {
		delete item;
	}
}

}