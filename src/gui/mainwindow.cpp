#include <QtCore/QSettings>
#include <QtCore/QDebug>

#include <QtGui/QMessageBox>

#include <algorithm>

#include "updaterwidget.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

namespace Gui
{

MainWindow::MainWindow (QWidget *parent)
		: QMainWindow (parent), ui_ (new Ui::MainWindow)
{
	ui_->setupUi (this);
}

MainWindow::~MainWindow()
{
	delete ui_;
}

void MainWindow::changeEvent (QEvent *e)
{
	QMainWindow::changeEvent (e);

	switch (e->type()) {

		case QEvent::LanguageChange:
			ui_->retranslateUi (this);
			break;

		default:
			break;
	}
}

void MainWindow::newUpdateAvailable (const Core::UpdaterPtr& updater)
{
	const Core::ProductVersion &version = *updater->availableUpdates().rbegin();
	
	const QString &message = "New version for %1 - %2.\nInstall it?";
	int result = QMessageBox::information (this,
										   "",
										message.arg(updater->productName()).arg(version.productVersion()),
										   QMessageBox::Yes | QMessageBox::No);

	if (result == QMessageBox::Yes) {
		show();
		UpdaterWidget *updaterWidget_ = new UpdaterWidget (updater, this);
		ui_->updaterWidgetsContainer->addTab (updaterWidget_,
											  updaterWidget_->windowTitle());
		updaterWidget_->downloadUpdate();
	}
}

}
