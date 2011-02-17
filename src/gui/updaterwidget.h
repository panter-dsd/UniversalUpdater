#ifndef UPDATERWIDGET_H
#define UPDATERWIDGET_H

#include <QtGui/QWidget>

#include "abstractupdater.h"

namespace Ui
{

class UpdaterWidget;
}

namespace Gui
{

typedef QSharedPointer <Core::AbstractUpdater> UpdaterPtr;

class UpdaterWidget : public QWidget
{
	Q_OBJECT

public:
	explicit UpdaterWidget (UpdaterPtr updater, QWidget *parent = 0);
	~UpdaterWidget();

public Q_SLOTS:
	void checkForUpdates ();
	void downloadUpdate ();

protected:
	void changeEvent (QEvent *e);

private Q_SLOTS:
	void refreshUpdatesList ();
	void refreshDescription ();
	void downloadFinished ();

private:
	Ui::UpdaterWidget *ui_;
	UpdaterPtr updater_;
	QString version_;
	Core::ProductVersionList productVersionList_;
};
}
#endif // UPDATERWIDGET_H
