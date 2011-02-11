#include <QtCore/QDebug>

#include "updaterwidget.h"
#include "ui_updaterwidget.h"

namespace Gui
{
UpdaterWidget::UpdaterWidget (UpdaterPtr updater, QWidget *parent)
		: QWidget (parent), ui_ (new Ui::UpdaterWidget), updater_ (updater)
{
	ui_->setupUi (this);

	connect (updater_.data (), SIGNAL (checkFinished()),
			 this, SLOT (refreshUpdatesList()));
	connect (ui_->updatesList, SIGNAL (itemSelectionChanged()),
			 this, SLOT (refreshDescription()));
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

	QListWidgetItem *item;

	for (Core::ProductVersionList::const_iterator it = productVersionList_.begin(),
			end = productVersionList_.end(); it != end; ++it) {
		item = new QListWidgetItem (it->productNames() ["ru"] + " " + it->productVersion());
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
}
