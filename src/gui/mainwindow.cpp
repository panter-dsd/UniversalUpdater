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

#include "mainwindow.h"
#include "ui_mainwindow.h"

namespace Core {
class VersionNotifyQueue {
public:
	typedef QPair <AbstractUpdater*, Core::ProductVersion> VersionNotify;
	
public:
	void push (const VersionNotify& notify);
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
	VersionNotifyQueuePredicate (const VersionNotifyQueue::VersionNotify& notify)
	: notify_ (notify)
	{}
	
	bool operator() (const VersionNotifyQueue::VersionNotify& notify) const {
		return notify.first == notify_.first;
	}
	
private:
	VersionNotifyQueue::VersionNotify notify_;
};

void VersionNotifyQueue::push (const VersionNotifyQueue::VersionNotify& notify)
{
	const NotifyList::iterator &it = std::find_if (notifyList_.begin(),
												   notifyList_.end(),
												   VersionNotifyQueuePredicate (notify));

	if (it != notifyList_.constEnd()) {
		if (it->second < notify.second) {
			notifyList_.erase (it);
			notifyList_.push_back (notify);
		}
	} else {
		notifyList_.push_back (notify);
	}
}

void VersionNotifyQueue::pop (
)
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

MainWindow::MainWindow (const QSettings& settings, QWidget* parent)
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
	trayContextMenu->addAction (exitAction);

	trayIcon->setContextMenu (trayContextMenu);

	loadSettings ();
}

MainWindow::~MainWindow()
{
	saveSettings();
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
								 Core::AbstractUpdater* updater)
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

void MainWindow::setUpdaterList (const Core::UpdatersList& l)
{
	updatersList_ = l;

	for (UpdaterWidgetList::const_iterator it = updaterWidgetList_.constBegin(),
			end = updaterWidgetList_.constEnd(); it != end; ++it) {
		const int index = ui_->updaterWidgetsContainer->indexOf (*it);

		if (index >= 0) {
			ui_->updaterWidgetsContainer->removeTab (index);
		}

		delete *it;
	}

	updaterWidgetList_.clear();

	UpdaterWidget *updaterWidget;

	for (Core::UpdatersList::const_iterator it = updatersList_.constBegin(),
			end = updatersList_.constEnd(); it != end; ++it) {
		connect (*it, SIGNAL (checkFinished()),
				 this, SLOT (updateTabNames ()));
		connect (*it, SIGNAL (checkFinished()),
				 this, SLOT (updaterCheckedFinished ()));

		updaterWidget = new UpdaterWidget (*it, this);
		connect (updaterWidget, SIGNAL (updateToVersion (Core::AbstractUpdater*, Core::ProductVersion)),
				 this, SLOT (updateToVersion (Core::AbstractUpdater*, Core::ProductVersion)));
		updaterWidgetList_.push_back (updaterWidget);
		ui_->updaterWidgetsContainer->addTab (updaterWidget,
											  updaterWidget->windowIcon (),
											  updaterWidget->windowTitle ());
	}
}

void MainWindow::updaterCheckedFinished ()
{
	AbstractUpdater *updater = qobject_cast<AbstractUpdater*> (sender ());
	assert (updater);

	if (updater->availableUpdates().empty()) {
		return;
	}

	const Core::ProductVersion version = *updater->availableUpdates().begin();

	versionNotifyQueue_->push (Core::VersionNotifyQueue::VersionNotify (updater, version));
	checkVersionNotifyQueue ();
/*
	const QString text = tr ("New version %1 is available.\n")
						 + (updater->isDownloaded (version)
							? tr ("Install it?\n")
							: tr ("Download and install it?\n"))
						 + tr ("Size to download: ")
						 + Core::stringSize (updater->isDownloaded (version)
											 ? 0
											 : version.productSize());

	QMessageBox newVersionMessage (QMessageBox::Information,
								   version.productNames() [Core::currentLocale() ],
								   text.arg (version.productVersion()),
								   QMessageBox::Yes | QMessageBox::No);
	newVersionMessage.addButton (tr ("More"), QMessageBox::AcceptRole);

	const int result = newVersionMessage.exec ();

	switch (result) {

		case QMessageBox::Yes: {
			updateToVersion (updater, version);
			break;
		}

		case QMessageBox::No:
			break;

		default: {
			showAndActivate ();
			UpdaterWidget *w = widgetForUpdater (updaterWidgetList_, updater);
			ui_->updaterWidgetsContainer->setCurrentWidget (w);
			break;
		}
	}*/
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
										  notify.second.productNames() [Core::currentLocale() ],
										  text.arg (notify.second.productVersion()),
										  QMessageBox::Yes | QMessageBox::No);
	connect (newVersionMessage_, SIGNAL (finished(int)),
		this, SLOT (answerMessage(int)));

	newVersionMessage_->addButton (tr ("More"), QMessageBox::AcceptRole);
	newVersionMessage_->show ();
}

void MainWindow::answerMessage (int result)
{
	QAbstractButton *button = newVersionMessage_->clickedButton();

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

void MainWindow::updateToVersion (Core::AbstractUpdater* updater,
								  const Core::ProductVersion& version)
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
	UpdateDownloadDialog *d = qobject_cast <UpdateDownloadDialog*> (sender());
	assert (d);

	const int index = updateDownloadDialogPtrList.indexOf (d);
	assert (index >= 0);
	updateDownloadDialogPtrList.remove (index);
	d->deleteLater();
}

void MainWindow::loadSettings ()
{
	QSettings settings;
	settings.beginGroup ("GUI");
	settings.beginGroup ("MainWindow");

	const QPoint pos = settings.value ("pos").toPoint();

	if (!pos.isNull()) {
		move (pos);
	}

	const QSize size = settings.value ("size", QSize (640, 480)).toSize();

	resize (size);

	const bool isMaximized = settings.value ("IsMaximized", false).toBool();

	if (isMaximized) {
		setWindowState (Qt::WindowMaximized);
	}

	settings.endGroup();

	settings.endGroup();
}

void MainWindow::saveSettings ()
{
	QSettings settings;
	settings.beginGroup ("GUI");
	settings.beginGroup ("MainWindow");

	if (windowState() != Qt::WindowMaximized) {
		settings.setValue ("pos", pos());
		settings.setValue ("size", size());
		settings.setValue ("IsMaximized", false);
	} else {
		settings.setValue ("IsMaximized", true);
	}

	settings.endGroup();

	settings.endGroup();
}

void MainWindow::closeEvent (QCloseEvent* e)
{
	e->ignore();
	hide ();

	updateShowHideActionText ();
}

void MainWindow::updateShowHideActionText ()
{
	ui_->showHideAction->setText (isHidden() ? tr ("Show") : tr ("Hide"));
}

}
