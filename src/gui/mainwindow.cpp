#include <QtCore/QSettings>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

#include <QtGui/QMessageBox>
#include <QtGui/QStyle>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QPushButton>
#include <QtGui/QCloseEvent>

#include <algorithm>
#include <assert.h>

#include "updaterwidget.h"
#include "preferencesdialog.h"
#include "core.h"
#include "updatedownloaddialog.h"
#include "widgetstatesettings.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

namespace Core
{
class VersionNotifyQueue
{
public:
	typedef QPair <AbstractUpdater *, Core::ProductVersion> VersionNotify;

public:
	void push (const VersionNotify &notify);
	void pop ();
	VersionNotify front () const;
	bool isEmpty () const {
		return notifyList_.isEmpty();
	}

private:
	typedef QVector <VersionNotify> NotifyList;
	NotifyList notifyList_;

};

struct VersionNotifyQueuePredicate
		: public std::unary_function <VersionNotifyQueue::VersionNotify, bool> {
public:
	VersionNotifyQueuePredicate (const VersionNotifyQueue::VersionNotify &notify)
		: notify_ (notify)
	{}

	bool operator() (const VersionNotifyQueue::VersionNotify &notify) const {
		return notify.second.productID() == notify_.second.productID();
	}

private:
	VersionNotifyQueue::VersionNotify notify_;
};

void VersionNotifyQueue::push (const VersionNotifyQueue::VersionNotify &notify)
{
	const NotifyList::iterator &it = std::find_if (notifyList_.begin(),
									 notifyList_.end(),
									 VersionNotifyQueuePredicate (notify));

	if (it != notifyList_.end()) {
		if (it->second < notify.second && it != notifyList_.begin()) {
			notifyList_.erase (it);
			notifyList_.push_back (notify);
		}
	} else {
		notifyList_.push_back (notify);
	}
}

void VersionNotifyQueue::pop ()
{
	notifyList_.pop_front();
}

VersionNotifyQueue::VersionNotify VersionNotifyQueue::front () const
{
	return notifyList_.front();
}
}

namespace Gui
{

MainWindow::MainWindow (const QSettings &settings, QWidget *parent)
	: QMainWindow (parent), ui_ (new Ui::MainWindow),
	  settings_ (settings.fileName(), settings.format()),
	  versionNotifyQueue_ (new Core::VersionNotifyQueue),
	  newVersionMessage_ (0)
{
	ui_->setupUi (this);
	setWindowTitle (QObject::tr ("Universal Updater"));

	trayIcon = new QSystemTrayIcon (QIcon (":/share/images/tray_main_icon.png"), this);
	trayIcon->setToolTip (windowTitle());
	connect (trayIcon, SIGNAL (activated (QSystemTrayIcon::ActivationReason)),
			 this, SLOT (trayActivated (QSystemTrayIcon::ActivationReason)));
	trayIcon->show ();

	QMenu *trayContextMenu = new QMenu (this);

	connect (ui_->showHideAction, SIGNAL (triggered ()),
			 this, SLOT (showHide()));
	trayContextMenu->addAction (ui_->showHideAction);

	QAction *exitAction = new QAction (style ()->standardIcon (QStyle::SP_DialogCloseButton),
									   tr ("Exit"),
									   this);
	connect (exitAction, SIGNAL (triggered ()),
			 QCoreApplication::instance(), SLOT (quit ()));
#ifndef NDEBUG
	trayContextMenu->addAction (exitAction);
#endif

	trayIcon->setContextMenu (trayContextMenu);

	Utils::WidgetStateSettings (this, &settings_).load ();
}

MainWindow::~MainWindow()
{
	Utils::WidgetStateSettings (this, &settings_).save ();
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

namespace
{
struct FindUpdaterWidgetPredicate
		: public std::unary_function <Gui::UpdaterWidget *, bool> {

	FindUpdaterWidgetPredicate (Core::AbstractUpdater *updater)
		: version_ (updater->currentProductVersion ()) {}

	bool operator() (Gui::UpdaterWidget *w) {
		return w->updater ()->currentProductVersion () == version_;
	}

private:
	Core::ProductVersion version_;
};
}

UpdaterWidget *widgetForUpdater (const UpdaterWidgetList &l,
								 Core::AbstractUpdater *updater)
{
	const UpdaterWidgetList::const_iterator &it = std::find_if (l.constBegin (),
			l.constEnd (),
			FindUpdaterWidgetPredicate (updater));

	return it != l.constEnd () ? *it : 0;
}

void MainWindow::deleteUpdaterWidget (UpdaterWidget *w)
{
	const int index = ui_->updaterWidgetsContainer->indexOf (w);

	if (index >= 0) {
		ui_->updaterWidgetsContainer->removeTab (index);
	}

	delete w;
}

void MainWindow::addUpdaterWidget (Core::AbstractUpdater *updater)
{
	UpdaterWidget *updaterWidget = new UpdaterWidget (updater, this);
	connect (updaterWidget, SIGNAL (updateToVersion (Core::AbstractUpdater *, Core::ProductVersion)),
			 this, SLOT (updateToVersion (Core::AbstractUpdater *, Core::ProductVersion)));
	updaterWidgetList_.push_back (updaterWidget);
	ui_->updaterWidgetsContainer->addTab (updaterWidget,
										  updaterWidget->windowIcon (),
										  updaterWidget->windowTitle ());

	connect (updater, SIGNAL (checkFinished()),
			 this, SLOT (updateTabNames ()));
	connect (updater, SIGNAL (checkFinished()),
			 this, SLOT (updaterCheckedFinished ()));
}

void MainWindow::setUpdaterList (const Core::UpdatersList &l)
{
	updatersList_ = l;

	for (UpdaterWidgetList::const_iterator it = updaterWidgetList_.constBegin(),
			end = updaterWidgetList_.constEnd(); it != end; ++it) {
		deleteUpdaterWidget (*it);
	}

	updaterWidgetList_.clear();

	for (Core::UpdatersList::const_iterator it = updatersList_.constBegin(),
			end = updatersList_.constEnd(); it != end; ++it) {
		addUpdaterWidget (*it);
	}
}

void MainWindow::updaterCheckedFinished ()
{
	AbstractUpdater *updater = qobject_cast<AbstractUpdater *> (sender ());
	assert (updater);

	const Core::ProductVersionList &list = updater->availableUpdates();

	if (!list.empty()) {
		const Core::ProductVersion version = *list.begin ();

		versionNotifyQueue_->push (Core::VersionNotifyQueue::VersionNotify (updater, version));

		checkVersionNotifyQueue ();
	}
}

void MainWindow::checkVersionNotifyQueue ()
{
	if (newVersionMessage_ || versionNotifyQueue_->isEmpty()) {
		return;
	}

	const Core::VersionNotifyQueue::VersionNotify notify = versionNotifyQueue_->front ();

	const QString text = tr ("New version %1 is available.\n")
						 + (notify.first->isDownloaded (notify.second)
							? tr ("Install it?\n")
							: tr ("Download and install it?\n"))
						 + tr ("Size to download: ")
						 + Core::stringSize (notify.first->isDownloaded (notify.second)
								 ? 0
								 : notify.second.productSize());

	newVersionMessage_ = new QMessageBox (QMessageBox::Information,
										  notify.second.productNames() [Core::currentLocale ()],
										  text.arg (notify.second.productVersion()),
										  QMessageBox::Yes | QMessageBox::No);

	connect (newVersionMessage_, SIGNAL (finished (int)),
			 this, SLOT (answerMessage (int)));

	newVersionMessage_->addButton (tr ("More"), QMessageBox::AcceptRole);

	newVersionMessage_->show ();
}

void MainWindow::answerMessage (int result)
{
	QAbstractButton *button = newVersionMessage_->clickedButton ();

	const Core::VersionNotifyQueue::VersionNotify notify = versionNotifyQueue_->front ();

	if (button) {
		if (newVersionMessage_->standardButton (button) == QMessageBox::Yes) {
			updateToVersion (notify.first, notify.second);
		} else {
			if (newVersionMessage_->standardButton (button) != QMessageBox::No) {//Button More
				showAndActivate ();
				UpdaterWidget *w = widgetForUpdater (updaterWidgetList_, notify.first);
				ui_->updaterWidgetsContainer->setCurrentWidget (w);
			}
		}
	}

	newVersionMessage_->deleteLater();
	newVersionMessage_ = 0;
	versionNotifyQueue_->pop();
	checkVersionNotifyQueue ();
}

void MainWindow::updateToVersion (Core::AbstractUpdater *updater,
								  const Core::ProductVersion &version)
{
	if (!updater->isFinished()) {
		updater->stopUpdate();
	}

	UpdateDownloadDialog *d = new UpdateDownloadDialog (updater, version);

	updateDownloadDialogPtrList.push_back (d);

	connect (d, SIGNAL (accepted()),
			 this, SLOT (downloadDialogFinished()));
	connect (d, SIGNAL (rejected()),
			 this, SLOT (downloadDialogFinished()));

	Utils::WidgetStateSettings widgetStateSettings (d,
								"GUI/UpdateDownloadDialog",
								&settings_);
	widgetStateSettings.setDefaultValue ("size", QSize (0, 0));
	widgetStateSettings.load ();
	d->show();
}

void MainWindow::trayActivated (QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::Trigger) {
		showHide ();
	}
}

void MainWindow::preferences ()
{
	PreferencesDialog d (settings_, updatersList_);
	d.setWindowIcon (QIcon (":/share/images/preferences.png"));
	d.setWindowTitle (windowTitle() + " - " + d.windowTitle());
	d.exec ();
}

void MainWindow::showAndActivate ()
{
	setWindowState (windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
	show ();
	activateWindow();
}

void MainWindow::showHide ()
{
	if (isHidden()) {
		showAndActivate ();
	} else {
		hide ();
	}

	updateShowHideActionText ();
}

void MainWindow::updateTabNames ()
{
	for (int i = 0, count = ui_->updaterWidgetsContainer->count();
			i < count; ++i) {
		ui_->updaterWidgetsContainer->setTabText (i,
				ui_->updaterWidgetsContainer->widget (i)->windowTitle());
	}
}

void MainWindow::downloadDialogFinished ()
{
	UpdateDownloadDialog *d = qobject_cast <UpdateDownloadDialog *> (sender());
	assert (d);

	const int index = updateDownloadDialogPtrList.indexOf (d);
	assert (index >= 0);
	updateDownloadDialogPtrList.remove (index);

	Utils::WidgetStateSettings (d,
								"GUI/UpdateDownloadDialog",
								&settings_).save ();
	d->deleteLater();
}

void MainWindow::closeEvent (QCloseEvent *e)
{
	e->ignore();
	hide ();

	updateShowHideActionText ();
}

void MainWindow::updateShowHideActionText ()
{
	ui_->showHideAction->setText (isHidden () ? tr ("Show") : tr ("Hide"));
}

}
