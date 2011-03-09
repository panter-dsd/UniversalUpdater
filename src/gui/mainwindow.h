#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QSettings>

#include <QtGui/QMainWindow>
#include <QtGui/QSystemTrayIcon>

#include "abstractupdater.h"

namespace Ui
{

class MainWindow;
}

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
	void setUpdaterList (const Core::UpdaterPtrList& l);
	void newUpdateAvailable (const Core::UpdaterPtr& updater);
	
protected:
	void changeEvent (QEvent *e);

private:
	MainWindow (const MainWindow&);
	MainWindow& operator= (const MainWindow&);

	void loadSettings ();
	void saveSettings ();

private Q_SLOTS:
	void trayActivated (QSystemTrayIcon::ActivationReason reason);
	void preferences ();
	void showHide ();
	void updateTabNames ();
	void downloadDialogFinished ();
	void updateToVersion (const Core::UpdaterPtr& updater,
						  const Core::ProductVersion& version);
	
private:
	Ui::MainWindow *ui_;
	QSettings settings_;
	QSystemTrayIcon *trayIcon;
	UpdaterWidgetList updaterWidgetList_;
	Core::UpdaterPtrList updatersList_;
	typedef QVector <UpdateDownloadDialog*> UpdateDownloadDialogPtrList;
	UpdateDownloadDialogPtrList updateDownloadDialogPtrList;
};
}
#endif // MAINWINDOW_H
