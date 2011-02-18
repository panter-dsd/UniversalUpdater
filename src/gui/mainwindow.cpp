#include <QtCore/QSettings>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

#include <QtGui/QMessageBox>
#include <QtGui/QStyle>
#include <QtGui/QAction>
#include <QtGui/QMenu>

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

	trayIcon = new QSystemTrayIcon(QIcon (":/share/images/tray_main_icon.png"), this);
	connect (trayIcon, SIGNAL (activated(QSystemTrayIcon::ActivationReason)),
			 this, SLOT (trayActivated(QSystemTrayIcon::ActivationReason)));
	trayIcon->show ();
	
	QMenu *trayContextMenu = new QMenu (this);
	
	QAction *exitAction = new QAction(style ()->standardIcon (QStyle::SP_DialogCloseButton),
									  QObject::tr ("Exit"),
									  this);
	connect (exitAction, SIGNAL (triggered ()),
			 QCoreApplication::instance(), SLOT (quit ()));
	trayContextMenu->addAction (exitAction);
	
	trayIcon->setContextMenu (trayContextMenu);
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
	const int result = QMessageBox::information (this,
										   "",
										message.arg(updater->productName()).arg(version.productVersion()),
										   QMessageBox::Yes | QMessageBox::No);

	if (result == QMessageBox::Yes) {
		UpdaterWidget *updaterWidget_ = new UpdaterWidget (updater, 0);
		updaterWidget_->show();
	}
}

void MainWindow::trayActivated (QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::Trigger) {
		if (isHidden()) {
			show ();
		} else {
			hide ();
		}
	}
}

}
