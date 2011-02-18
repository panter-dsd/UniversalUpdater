#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

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

protected:
	void changeEvent (QEvent *e);

private:
	MainWindow (const MainWindow&);
	MainWindow& operator= (const MainWindow&);

public Q_SLOTS:
	void newUpdateAvailable (const Core::UpdaterPtr& updater);

private:
	Ui::MainWindow *ui_;
};
}
#endif // MAINWINDOW_H
