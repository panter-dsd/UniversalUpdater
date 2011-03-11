#include <QtCore/QDir>
#include <QtCore/QDebug>

#include <QtGui/QMessageBox>
#include <QtGui/QLayout>
#include <QtGui/QLabel>

#include "core.h"

#include "updaterwidget.h"
#include "ui_updaterwidget.h"

namespace Gui
{
UpdaterWidget::UpdaterWidget (const Core::UpdaterPtr& updater, QWidget* parent)
		: QWidget (parent), ui_ (new Ui::UpdaterWidget), updater_ (updater),
		model_ (0), informationLabel_ (0)
{
	ui_->setupUi (this);
	checkFinished();
	setWindowIcon (Core::fileIcon (updater_->config() ["Icon"].toString()));

	model_ = new Core::UpdatesModel (updater_, this);
	ui_->updatesList->setModel (model_);

	connect (ui_->checkButton, SIGNAL (clicked ()),
			 updater_.data(), SLOT (checkForUpdates()));
	connect (ui_->updateButton, SIGNAL (clicked ()),
			 this, SLOT (updateToVersion ()));

	connect (updater_.data (), SIGNAL (stateChanged(AbstractUpdater::UpdaterState)),
			 this, SLOT (updaterStateChanged (AbstractUpdater::UpdaterState)));
	connect (updater_.data (), SIGNAL (checkFinished()),
			 this, SLOT (checkFinished()));
	connect (ui_->updatesList->selectionModel(), SIGNAL (currentChanged (QModelIndex, QModelIndex)),
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

void UpdaterWidget::checkFinished()
{
	setWindowTitle (updater_->productName ()
					+ " - "
					+ updater_->currentProductVersion ().productVersion ());
}

void UpdaterWidget::checkForUpdates ()
{
	updater_->checkForUpdates ();
}

void UpdaterWidget::refreshDescription ()
{
	ui_->updateDescription->clear ();
	ui_->updateDescription->setText (model_->data (ui_->updatesList->currentIndex (),
									 Qt::ToolTipRole).toString ());
}

void UpdaterWidget::updateToVersion ()
{
	const Core::ProductVersion &version = model_->version (model_->checkedItem());

	if (!version.empty()) {
		emit updateToVersion (updater_, version);
	}
}

void UpdaterWidget::updaterStateChanged (AbstractUpdater::UpdaterState state)
{
	switch (state) {
		case AbstractUpdater::CheckState:
		case AbstractUpdater::DownloadState:
		case AbstractUpdater::InstallState:
			blockWidget (AbstractUpdater::updaterStateText(state));
			break;
		case AbstractUpdater::CheckFinishedState:
		case AbstractUpdater::DownloadFinishedState:
		case AbstractUpdater::InstallFinishedState:
			unblockWidget ();
			break;
		default: break;
	}
}

void UpdaterWidget::blockWidget (const QString& text)
{
	Core::deleteIf (informationLabel_);
	
	foreach (QWidget *w, findChildren<QWidget*>()) {
		w->hide();
	}
	
	informationLabel_ = new QLabel (text, this);
	informationLabel_->setAlignment (Qt::AlignCenter | Qt::AlignHCenter);
	layout()->addWidget(informationLabel_);
}

void UpdaterWidget::unblockWidget ()
{
	Core::deleteIf (informationLabel_);
	
	foreach (QWidget *w, findChildren<QWidget*>()) {
		w->show();
	}
}
}

