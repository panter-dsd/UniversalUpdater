#include <QtCore/QDebug>

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

	connect (ui_->downloadButton, SIGNAL (clicked ()),
			 this, SLOT (downloadUpdate ()));

	connect (updater_.data (), SIGNAL (checkFinished()),
			 this, SLOT (refreshUpdatesList()));
	connect (updater_.data (), SIGNAL (downloadFinished()),
			 this, SLOT (downloadFinished()));
	connect (ui_->updatesList, SIGNAL (itemSelectionChanged()),
			 this, SLOT (refreshDescription()));

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
	updater_->checkForUpdates ();
}

void UpdaterWidget::refreshUpdatesList ()
{
	ui_->updatesList->clear();
	productVersionList_ = updater_->availableUpdates ();

	if (productVersionList_.empty()) {
		return;
	}

	QListWidgetItem *item;

	for (Core::ProductVersionList::const_iterator it = productVersionList_.begin(),
			end = productVersionList_.end(); it != end; ++it) {
		item = new QListWidgetItem (it->productNames() [Core::currentLocale()]
									+ " "
									+ it->productVersion());
		item->setData (Qt::UserRole, it->productVersion());
		item->setData (Qt::CheckStateRole, Qt::Unchecked);

		ui_->updatesList->insertItem (0, item);
	}

	if (ui_->updatesList->count() > 0) {
		ui_->updatesList->item (0)->setData (Qt::CheckStateRole, Qt::Checked);
	}
}

void UpdaterWidget::refreshDescription ()
{
	ui_->updateDescription->clear();
	QListWidgetItem *item = ui_->updatesList->currentItem();

	if (!item) {
		return;
	}

	const QString &version = item->data (Qt::UserRole).toString();

	for (Core::ProductVersionList::const_iterator it = productVersionList_.begin(),
			end = productVersionList_.end(); it != end; ++it) {
		if (it->productVersion() == version) {
			QStringList html;
			html.push_back ("<p>");
			html.push_back ("<b>Date:</b> "
							+ it->productDate().toString());
			html.push_back ("<p>");
			html.push_back ("<b>Download size:</b> "
							+ QString::number (it->productSize())
							+ " byte");
			html.push_back ("<p>");
			html.push_back ("<b>MD5:</b> "
							+ it->productMd5sum());
			html.push_back ("<p>");
			html.push_back (it->productDescriptions() ["ru"]);
			ui_->updateDescription->setHtml (html.join ("\n"));
			break;
		}
	}
}

void UpdaterWidget::downloadUpdate ()
{
	QListWidgetItem *item = 0;

	for (int i = 0, count = ui_->updatesList->count(); i < count; ++i) {
		item = ui_->updatesList->item (i);

		if (item->data (Qt::CheckStateRole).toInt() == Qt::Checked) {
			break;
		}
	}

	if (!item) {
		return;
	}

	version_ = item->data (Qt::UserRole).toString();

	for (Core::ProductVersionList::const_iterator it = productVersionList_.begin(),
			end = productVersionList_.end(); it != end; ++it) {
		if (it->productVersion() == version_) {
			updater_->downloadUpdate (*it, "/var/tmp");
		}
	}
}

void UpdaterWidget::downloadFinished ()
{
	for (Core::ProductVersionList::const_iterator it = productVersionList_.begin(),
			end = productVersionList_.end(); it != end; ++it) {
		if (it->productVersion() == version_) {
			updater_->installUpdate (*it, "/var/tmp");
		}
	}
}
}

