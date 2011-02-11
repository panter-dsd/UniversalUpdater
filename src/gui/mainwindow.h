#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QSharedPointer>

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

private Q_SLOTS:
	void checkForUpdates ();

private:
	MainWindow (const MainWindow&);
	MainWindow& operator= (const MainWindow&);

	void initializeUpdaterCache ();
	void loadConfig ();
	bool addUpdaterWidget (const QString& product);

private:
	Ui::MainWindow *ui_;
	typedef QSharedPointer <Core::AbstractUpdater> UpdaterPtr;
	typedef QVector <Core::AbstractUpdater*> UpdaterCache;
	static UpdaterCache updaterCache_;
};
}
#endif // MAINWINDOW_H
