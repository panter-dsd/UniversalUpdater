#include <QtCore/QDir>
#include <QtCore/QDebug>

#include <QtGui/QMessageBox>

#include "core.h"

#include "updaterwidget.h"
#include "ui_updaterwidget.h"

namespace Gui
{
UpdaterWidget::UpdaterWidget (Core::UpdaterPtr updater, QWidget* parent)
		: QWidget (parent), ui_ (new Ui::UpdaterWidget), updater_ (updater)
{
	ui_->setupUi (this);
	setWindowTitle (updater->productName());

	connect (ui_->checkButton, SIGNAL (clicked ()),
			 updater.data(), SLOT (checkForUpdates()));
	connect (ui_->updateButton, SIGNAL (clicked ()),
			 this, SLOT (update ()));

	connect (updater_.data (), SIGNAL (checkFinished()),
			 this, SLOT (refreshUpdatesList()));
	connect (updater_.data (), SIGNAL (downloadFinished()),
			 this, SLOT (downloadFinished()));
	connect (ui_->updatesList, SIGNAL (itemSelectionChanged()),
			 this, SLOT (refreshDescription()));
	connect (updater_.data (), SIGNAL (downloadProgress (qint64, qint64)),
			 this, SLOT (downloadProgress (qint64, qint64)));

	refreshUpdatesList ();
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

void UpdaterWidget::refreshUpdatesList ()
{
	clearDownloadProgress ();
	ui_->updatesList->clear();
	productVersionList_ = updater_->availableUpdates ();

	if (productVersionList_.empty()) {
		return;
	}

	QListWidgetItem *item;

	for (Core::ProductVersionList::const_iterator it = productVersionList_.begin(),
			end = productVersionList_.end(); it != end; ++it) {
		item = new QListWidgetItem (it->productNames() [Core::currentLocale() ]
									+ " "
									+ it->productVersion());
		item->setData (Qt::UserRole, it->productVersion());
		item->setData (Qt::CheckStateRole, Qt::Unchecked);

		ui_->updatesList->insertItem (0, item);
	}

	if (ui_->updatesList->count() > 0) {
		ui_->updatesList->item (0)->setData (Qt::CheckStateRole, Qt::Checked);
		ui_->updatesList->setCurrentRow (0);
	}
}

Core::ProductVersion versionForItem (QListWidgetItem *item, const Core::ProductVersionList &l)
{
	const QString &version = item->data (Qt::UserRole).toString();

	for (Core::ProductVersionList::const_iterator it = l.begin(),
			end = l.end(); it != end; ++it) {
		if (it->productVersion() == version) {
			return *it;
		}
	}

	return Core::ProductVersion ();
}

void UpdaterWidget::refreshDescription ()
{
	ui_->updateDescription->clear();
	QListWidgetItem *item = ui_->updatesList->currentItem();

	if (!item) {
		return;
	}

	const Core::ProductVersion &version = versionForItem (item, productVersionList_);

	if (!version.empty ()) {
		QStringList html;
		html.push_back ("<p>");
		html.push_back ("<b>Date:</b> "
						+ version.productDate().toString());
		html.push_back ("<p>");
		html.push_back ("<b>Download size:</b> "
						+ QString::number (version.productSize())
						+ " byte");
		html.push_back ("<p>");
		html.push_back (version.productDescriptions() [Core::currentLocale() ]);
		ui_->updateDescription->setHtml (html.join ("\n"));
	}
}


Core::ProductVersion UpdaterWidget::checkedVersion () const
{
	QListWidgetItem *item = 0;

	for (int i = 0, count = ui_->updatesList->count(); i < count; ++i) {
		item = ui_->updatesList->item (i);

		if (item->data (Qt::CheckStateRole).toInt() == Qt::Checked) {
			break;
		}
	}

	return item
		   ? versionForItem (item, productVersionList_)
		   : Core::ProductVersion ();
}

void UpdaterWidget::update ()
{
	ui_->updateButton->setEnabled (false);

	clearDownloadProgress();

	const Core::ProductVersion &version = checkedVersion ();

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

