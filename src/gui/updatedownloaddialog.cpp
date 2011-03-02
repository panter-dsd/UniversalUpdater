#include <QtCore/QDir>

#include <QtGui/QMessageBox>

#include "core.h"

#include "updatedownloaddialog.h"
#include "ui_updatedownloaddialog.h"

namespace Gui
{

UpdateDownloadDialog::UpdateDownloadDialog (const Core::UpdaterPtr &updater,
		const Core::ProductVersion &version,
		QWidget * parent,
		Qt::WindowFlags f)
		: QDialog (parent, f), ui_ (new Ui::UpdateDownloadDialog),
		updater_ (updater), version_ (version)
{
	ui_->setupUi (this);
	setWindowTitle (updater_->productName()
					+ " - "
					+ version_.productVersion());
	setWindowIcon (Core::fileIcon (updater_->config() ["Icon"].toString()));

	connect (updater_.data(), SIGNAL (downloadFinished()),
			 this, SLOT (downloadFinished()));
	connect (updater_.data(), SIGNAL (downloadProgress(qint64,qint64)),
			 this, SLOT (downloadProgress(qint64,qint64)));
	

	if (updater_->isDownloaded (version)) {
		installUpdate();
	} else {
		ui_->sourceLabel->setText (version.productUrl ());
		const QString &updateFilePath = updater_->downloadUpdate (version);
		ui_->designationLabel->setText (QDir::toNativeSeparators (updateFilePath));
	}
}

void UpdateDownloadDialog::downloadFinished ()
{
	ui_->downloadProgressBar->setValue (ui_->downloadProgressBar->maximum ());
	ui_->stopButton->setEnabled (false);

	const Core::AbstractUpdater::UpdaterError error = updater_->lastError ();

	if (error != Core::AbstractUpdater::NoError) {
		QMessageBox::critical (this, windowTitle (), updater_->errorText ());
		return;
	}

	const int result = QMessageBox::information (this,
					   windowTitle (),
					   tr ("Update is downloaded. Install it?"),
					   QMessageBox::Ok | QMessageBox::Cancel);

	if (result == QMessageBox::Ok) {
		installUpdate ();
	}
}

void UpdateDownloadDialog::installUpdate ()
{
	const bool ifI = updater_->currentProductVersion().productID() == "uu";
	
	updater_->installUpdate ();
	
	if (updater_.data() && updater_->lastError() != Core::AbstractUpdater::NoError) {
		QMessageBox::critical (this,
							   windowTitle (),
							   tr ("Install error"));
		return;
	}
	
	//If it uu, then run install and close
	
	if (ifI) {
		QCoreApplication::quit();
	}
}

void UpdateDownloadDialog::downloadProgress (qint64 bytesReceived, qint64 bytesTotal)
{
	ui_->downloadProgressBar->setRange (0, bytesTotal);
	ui_->downloadProgressBar->setValue (bytesReceived);
}

}




