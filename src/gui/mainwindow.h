#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QSharedPointer>

#include <QtGui/QMainWindow>

#include "abstractupdater.h"

namespace Ui {
    class MainWindow;
}

namespace Gui {
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private Q_SLOTS:
	void refreshUpdatesList ();

private:
    Ui::MainWindow *ui_;
	typedef QSharedPointer <Core::AbstractUpdater> UpdaterPtr;
	UpdaterPtr updater_;
	Core::ProductVersionList productVersionList_;
};
}
#endif // MAINWINDOW_H
