#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QSystemTrayIcon>

#include "abstractupdater.h"

namespace Ui
{

class MainWindow;
}

namespace Gui
{

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow (QWidget *parent = 0);
	~MainWindow();
	
public Q_SLOTS:
	void newUpdateAvailable (const Core::UpdaterPtr& updater);
	
protected:
	void changeEvent (QEvent *e);

private:
	MainWindow (const MainWindow&);
	MainWindow& operator= (const MainWindow&);

private Q_SLOTS:
	void trayActivated (QSystemTrayIcon::ActivationReason reason);
	
private:
	Ui::MainWindow *ui_;
	QSystemTrayIcon *trayIcon;
};
}
#endif // MAINWINDOW_H
