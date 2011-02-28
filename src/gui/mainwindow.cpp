#include <QtCore/QSettings>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

#include <QtGui/QMessageBox>
#include <QtGui/QStyle>
#include <QtGui/QAction>
#include <QtGui/QMenu>

#include <algorithm>

#include "updaterwidget.h"
#include "preferencesdialog.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

namespace Gui
{

MainWindow::MainWindow (const QSettings& settings, QWidget* parent)
		: QMainWindow (parent), ui_ (new Ui::MainWindow),
		settings_ (settings.fileName(), settings.format())
{
	ui_->setupUi (this);
	setWindowTitle(QObject::tr ("Universal Updater"));

	trayIcon = new QSystemTrayIcon (QIcon (":/share/images/tray_main_icon.png"), this);
	trayIcon->setToolTip(windowTitle());
// 	connect (trayIcon, SIGNAL (activated (QSystemTrayIcon::ActivationReason)),
// 			 this, SLOT (trayActivated (QSystemTrayIcon::ActivationReason)));
	trayIcon->show ();

	QMenu *trayContextMenu = new QMenu (this);

	QAction *checkForUpdatesAction = new QAction (QIcon (),
									   tr ("Check for updates"),
									   this);
	connect (checkForUpdatesAction, SIGNAL (triggered ()),
			 this, SIGNAL (checkForUpdates()));
	trayContextMenu->addAction (checkForUpdatesAction);

	QAction *preferencesAction = new QAction (QIcon (":/share/images/preferences.png"),
												  tr ("Preferences"),
												  this);
	connect (preferencesAction, SIGNAL (triggered ()),
			 this, SLOT (preferences()));
	trayContextMenu->addAction (preferencesAction);
	
	QAction *exitAction = new QAction (style ()->standardIcon (QStyle::SP_DialogCloseButton),
									   tr ("Exit"),
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

UpdaterWidget* widgetForUpdater (const UpdaterWidgetList& l,
								 const Core::UpdaterPtr& updater)
{
	if (l.isEmpty()) {
		return 0;
	}

	for (UpdaterWidgetList::const_iterator it = l.constBegin(),
			end = l.end (); it != end; ++it) {
		if ( (*it)->updater ()->currentProductVersion () == updater->currentProductVersion ()) {
			return *it;
		}
	}

	return 0;
}

void MainWindow::setUpdaterList (const Core::UpdaterPtrList& l)
{
	updatersList_ = l;
}

void MainWindow::newUpdateAvailable (const Core::UpdaterPtr& updater)
{
	UpdaterWidget *updaterWidget_ = widgetForUpdater (updaterWidgetList_,
													  updater);
	if (!updaterWidget_) {
		updaterWidget_ = new UpdaterWidget (updater, 0);
		updaterWidget_->setWindowIcon(QIcon (":/share/images/tray_main_icon.png"));
		updaterWidgetList_.push_back (updaterWidget_);
	}
	if (!updaterWidget_->isActiveWindow()) {
		updaterWidget_->show();
		updaterWidget_->activateWindow();
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

void MainWindow::preferences ()
{
	PreferencesDialog d (settings_, updatersList_, this);
	d.setWindowIcon(QIcon (":/share/images/preferences.png"));
	d.exec ();
}

}
