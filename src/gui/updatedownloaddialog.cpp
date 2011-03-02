#include <QtCore/QDir>
#include <QtCore/QDebug>

#include <QtGui/QMessageBox>
#include <QtGui/QCloseEvent>

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

	ui_->stopButton->setIcon (style()->standardIcon(QStyle::SP_MediaStop));

	connect (ui_->stopButton, SIGNAL (clicked(bool)),
			 updater_.data(), SLOT (stopUpdate()));

	connect (updater_.data(), SIGNAL (downloadFinished()),
			 this, SLOT (downloadFinished()));
	connect (updater_.data(), SIGNAL (downloadProgress(qint64,qint64)),
			 this, SLOT (downloadProgress(qint64,qint64)));
}

void UpdateDownloadDialog::showEvent (QShowEvent* e)
{
	if (updater_->isDownloaded (version_)) {
		installUpdate();
	} else {
		ui_->sourceLabel->setText (version_.productUrl ());
		const QString &updateFilePath = updater_->downloadUpdate (version_);
		ui_->designationLabel->setText (QDir::toNativeSeparators (updateFilePath));
	}

	QDialog::showEvent (e);
}

void UpdateDownloadDialog::downloadFinished ()
{
	ui_->downloadProgressBar->setValue (ui_->downloadProgressBar->maximum ());
	ui_->stopButton->setEnabled (false);

	const Core::AbstractUpdater::UpdaterError error = updater_->lastError ();

	if (error != Core::AbstractUpdater::NoError) {
		QMessageBox::critical (this, windowTitle (), updater_->errorText ());
		emit rejected ();
		return;
	}

	const int result = QMessageBox::information (this,
					   windowTitle (),
					   tr ("Update is downloaded. Install it?"),
					   QMessageBox::Ok | QMessageBox::Cancel);

	if (result == QMessageBox::Ok) {
		installUpdate ();
	} else {
		emit rejected ();
	}
}

void UpdateDownloadDialog::installUpdate ()
{
	const bool ifI = updater_->currentProductVersion().productID() == "uu";
	
	updater_->installUpdate (version_);
	
	if (updater_.data() && updater_->lastError() != Core::AbstractUpdater::NoError) {
		QMessageBox::critical (this,
							   windowTitle (),
							   tr ("Install error"));
		emit rejected ();
	} else {
		//If it uu, then run install and close
		
		if (ifI) {
			QCoreApplication::quit();
		}
		emit accepted ();
	}

}

void UpdateDownloadDialog::downloadProgress (qint64 bytesReceived, qint64 bytesTotal)
{
	ui_->downloadProgressBar->setRange (0, bytesTotal);
	ui_->downloadProgressBar->setValue (bytesReceived);
}

void UpdateDownloadDialog::closeEvent (QCloseEvent* e)
{
	e->ignore();
	setWindowState (Qt::WindowMinimized);
}

}




