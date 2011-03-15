#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QSettings>

#include <QtGui/QMainWindow>
#include <QtGui/QSystemTrayIcon>

#include <memory.h>

#include "abstractupdater.h"
#include <auto_ptr.h>

namespace Ui
{

class MainWindow;
}

namespace Core
{

class VersionNotifyQueue;
}

class QMessageBox;

namespace Gui
{

class UpdaterWidget;

class UpdateDownloadDialog;

typedef QVector <UpdaterWidget*> UpdaterWidgetList;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow (const QSettings& settings, QWidget *parent = 0);
	~MainWindow();

Q_SIGNALS:
	void checkForUpdates ();

public Q_SLOTS:
	void setUpdaterList (const Core::UpdatersList& l);

protected:
	void changeEvent (QEvent *e);
	void closeEvent (QCloseEvent* e);

private:
	MainWindow (const MainWindow&);
	MainWindow& operator= (const MainWindow&);

	void loadSettings ();
	void saveSettings ();
	void updateShowHideActionText ();
	void showAndActivate ();

	void checkVersionNotifyQueue ();

private Q_SLOTS:
	void trayActivated (QSystemTrayIcon::ActivationReason reason);
	void preferences ();
	void showHide ();
	void updateTabNames ();
	void downloadDialogFinished ();
	void updateToVersion (Core::AbstractUpdater* updater,
						  const Core::ProductVersion& version);
	void updaterCheckedFinished ();
	void answerMessage (int result);

private:
	Ui::MainWindow *ui_;
	QSettings settings_;
	QSystemTrayIcon *trayIcon;
	UpdaterWidgetList updaterWidgetList_;
	Core::UpdatersList updatersList_;

	typedef QVector <UpdateDownloadDialog*> UpdateDownloadDialogPtrList;
	UpdateDownloadDialogPtrList updateDownloadDialogPtrList;

	std::auto_ptr <Core::VersionNotifyQueue> versionNotifyQueue_;
	QMessageBox *newVersionMessage_;
};
}
#endif // MAINWINDOW_H
