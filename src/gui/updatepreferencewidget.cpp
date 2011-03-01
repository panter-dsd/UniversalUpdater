#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QFileIconProvider>

#include "core.h"

#include "updatepreferencewidget.h"
#include "ui_updatepreferencewidget.h"

namespace Gui
{

UpdatePreferenceWidget::UpdatePreferenceWidget (const Core::UpdaterPtr& updater,
		QSettings* settings,
		QWidget* parent)
		: AbstractPreferenceWidget (settings, parent), ui_ (new Ui::UpdatePreferenceWidget),
		updater_ (updater)
{
	ui_->setupUi (this);
	setWindowTitle (updater_->productName());
	setWindowIcon (Core::fileIcon (updater_->config() ["Icon"].toString()));

	ui_->currentVersionLabel->setText (updater_->currentProductVersion().productVersion());

	connect (ui_->addSourceButton, SIGNAL (clicked()),
			 this, SLOT (addSource()));
	connect (ui_->editSourceButton, SIGNAL (clicked()),
			 this, SLOT (editSource()));
	connect (ui_->removeSourceButton, SIGNAL (clicked()),
			 this, SLOT (removeSource()));

	connect (ui_->moveUpButton, SIGNAL (clicked()),
			 this, SLOT (moveUp()));
	connect (ui_->moveDownButton, SIGNAL (clicked()),
			 this, SLOT (moveDown()));

	connect (ui_->checkOnStartupEdit, SIGNAL (stateChanged (int)),
			 this, SLOT (setIsChanged ()));
	connect (ui_->checkIntervalEdit, SIGNAL (valueChanged (int)),
			 this, SLOT (setIsChanged ()));

	connect (ui_->sourcesList->selectionModel(), SIGNAL (currentChanged(QModelIndex,QModelIndex)),
		this, SLOT (setButtonsEnabled()));

	setButtonsEnabled ();
	loadPreference ();
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
	const QString &url = QInputDialog::getText (this,
						 tr ("Adding source"),
						 tr ("Source"));

	if (!url.isEmpty()) {
		ui_->sourcesList->addItem (url);
		setIsChanged ();
		setButtonsEnabled ();
	}
}

void UpdatePreferenceWidget::editSource ()
{
	QListWidgetItem *item = ui_->sourcesList->currentItem();

	if (!item) {
		return;
	}

	const QString &url = QInputDialog::getText (this,
						 tr ("Editing source"),
						 tr ("Source"),
						 QLineEdit::Normal,
						 item->text());

	if (!url.isEmpty()) {
		item->setText (url);
		setIsChanged ();
		setButtonsEnabled ();
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

	if (item) {
		const int result = QMessageBox::question (this,
			"",
			tr ("Do you really want to delete the source?"),
			QMessageBox::Ok | QMessageBox::Cancel);

		if (result == QMessageBox::Ok) {
				delete item;
				setIsChanged ();
				setButtonsEnabled ();
		}
	}
}

QStringList listWidgetItems (QListWidget *w)
{
	QStringList l;

	for (int i = 0, count = w->count(); i < count; ++i) {
		l.push_back (w->item (i)->text ());
	}

	return l;
}

void UpdatePreferenceWidget::loadPreference ()
{
	settings_->beginGroup ("PRODUCTS");
	settings_->beginGroup (updater_->currentProductVersion().productID());
	ui_->checkOnStartupEdit->setChecked (settings_->value ("CheckOnStartup").toBool ());
	ui_->checkIntervalEdit->setValue (settings_->value ("CheckPeriod").toInt());
	ui_->sourcesList->addItems (settings_->value ("UpdateConfigUrl").toStringList());
	settings_->endGroup();
	settings_->endGroup();
}

void UpdatePreferenceWidget::savePreference ()
{
	settings_->beginGroup ("PRODUCTS");
	settings_->beginGroup (updater_->currentProductVersion().productID());
	settings_->setValue ("CheckOnStartup",
						 ui_->checkOnStartupEdit->checkState() == Qt::Checked);
	settings_->setValue ("CheckPeriod",
						 ui_->checkIntervalEdit->value());
	settings_->setValue ("UpdateConfigUrl",
						 listWidgetItems (ui_->sourcesList));
	settings_->endGroup();
	settings_->endGroup();
}

void UpdatePreferenceWidget::moveUp ()
{
	const int row = ui_->sourcesList->currentRow();
	
	if (row <= 0) {
		return;
	}
	
	QListWidgetItem *item = ui_->sourcesList->takeItem(row);
	ui_->sourcesList->insertItem(row - 1, item);
	ui_->sourcesList->setCurrentItem(item);
	setIsChanged ();
	setButtonsEnabled ();
}

void UpdatePreferenceWidget::moveDown ()
{
	const int row = ui_->sourcesList->currentRow();
	
	if (row < 0 || row == ui_->sourcesList->count() - 1) {
		return;
	}
	
	QListWidgetItem *item = ui_->sourcesList->takeItem(row);
	ui_->sourcesList->insertItem(row + 1, item);
	ui_->sourcesList->setCurrentItem(item);
	setIsChanged ();
	setButtonsEnabled ();
}

void UpdatePreferenceWidget::setButtonsEnabled ()
{
	bool isEnabled;

	isEnabled = ui_->sourcesList->count () > 0
	&& ui_->sourcesList->currentItem ();

	ui_->editSourceButton->setEnabled(isEnabled);

	isEnabled = ui_->sourcesList->count () > 1
	&& ui_->sourcesList->currentItem ();
	ui_->removeSourceButton->setEnabled(isEnabled);

	isEnabled = ui_->sourcesList->count () > 0
	&& ui_->sourcesList->currentRow() > 0;
	ui_->moveUpButton->setEnabled(isEnabled);

	isEnabled = ui_->sourcesList->count () > 0
	&& ui_->sourcesList->currentRow() >= 0
	&& ui_->sourcesList->currentRow() < ui_->sourcesList->count() - 1;
	ui_->moveDownButton->setEnabled(isEnabled);
}

}
