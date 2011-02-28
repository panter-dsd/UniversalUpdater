#include <QtCore/QDir>
#include <QtCore/QDebug>

#include <QtGui/QMessageBox>

#include "core.h"

#include "updaterwidget.h"
#include "ui_updaterwidget.h"

namespace Gui
{
UpdaterWidget::UpdaterWidget (const Core::UpdaterPtr& updater, QWidget* parent)
		: QWidget (parent), ui_ (new Ui::UpdaterWidget), updater_ (updater),
		model_ (0)
{
	ui_->setupUi (this);
	setWindowTitle (updater->productName()
					+ " - "
					+ updater_->currentProductVersion().productVersion());

	model_ = new Core::UpdatesModel (updater, this);
	ui_->updatesList->setModel (model_);

	connect (ui_->checkButton, SIGNAL (clicked ()),
			 updater.data(), SLOT (checkForUpdates()));
	connect (ui_->updateButton, SIGNAL (clicked ()),
			 this, SLOT (update ()));
	connect (ui_->stopButton, SIGNAL (clicked()),
			 updater_.data(), SLOT (stopUpdate()));

	connect (updater_.data (), SIGNAL (downloadFinished()),
			 this, SLOT (downloadFinished()));
	connect (ui_->updatesList->selectionModel(), SIGNAL (currentChanged(QModelIndex,QModelIndex)),
			 this, SLOT (refreshDescription()));
	connect (updater_.data (), SIGNAL (downloadProgress (qint64, qint64)),
			 this, SLOT (downloadProgress (qint64, qint64)));
}

UpdaterWidget::~UpdaterWidget()
{
	delete ui_;
}

void UpdaterWidget::changeEvent (QEvent *e)
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

void UpdaterWidget::checkForUpdates ()
{
	clearDownloadProgress ();
	updater_->checkForUpdates ();
}

void UpdaterWidget::refreshDescription ()
{
	ui_->updateDescription->clear();
	ui_->updateDescription->setText(model_->data(ui_->updatesList->currentIndex(),
												 Qt::ToolTipRole).toString());
}

void UpdaterWidget::update ()
{
	ui_->updateButton->setEnabled (false);
	ui_->stopButton->setEnabled (true);

	clearDownloadProgress();

	const Core::ProductVersion &version = model_->version (ui_->updatesList->currentIndex());

	if (!version.empty()) {
		ui_->sourceLabel->setText (version.productUrl ());
		const QString &updateFilePath = updater_->downloadUpdate (version,
										Core::savingPath());
		ui_->designationLabel->setText (QDir::toNativeSeparators (updateFilePath));
	}
}

void UpdaterWidget::downloadFinished ()
{
	ui_->downloadProgressBar->setValue (ui_->downloadProgressBar->maximum ());
	ui_->updateButton->setEnabled (true);
	ui_->stopButton->setEnabled (false);

	const Core::AbstractUpdater::UpdaterError &error = updater_->lastError ();

	if (error != Core::AbstractUpdater::NoError) {
		QMessageBox::critical (this, windowTitle (), updater_->errorText ());
		return;
	}

	const int result = QMessageBox::information (this,
					   windowTitle (),
					   tr ("Update is downloaded. Install it?"),
					   QMessageBox::Ok | QMessageBox::Cancel);

	if (result == QMessageBox::Cancel) {
		return;
	}

	updater_->installUpdate ();

	//If it uu, then run install and close

	if (updater_->currentProductVersion().productID() == "uu") {
		QCoreApplication::quit();
	}
}

void UpdaterWidget::downloadProgress (qint64 bytesReceived, qint64 bytesTotal)
{
	ui_->downloadProgressBar->setRange (0, bytesTotal);
	ui_->downloadProgressBar->setValue (bytesReceived);
}

void UpdaterWidget::clearDownloadProgress ()
{
	ui_->downloadProgressBar->setValue (ui_->downloadProgressBar->minimum ());
	ui_->sourceLabel->setText (QString ());
	ui_->designationLabel->setText (QString ());
}
}

