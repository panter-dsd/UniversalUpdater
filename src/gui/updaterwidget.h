#ifndef UPDATERWIDGET_H
#define UPDATERWIDGET_H

#include <QtGui/QWidget>

#include "abstractupdater.h"
#include "updatesmodel.h"

namespace Ui
{

class UpdaterWidget;
}

namespace Gui
{

class UpdaterWidget : public QWidget
{
	Q_OBJECT

public:
	explicit UpdaterWidget (const Core::UpdaterPtr& updater, QWidget* parent = 0);
	~UpdaterWidget();

	Core::UpdaterPtr updater () const {
		return updater_;
	}
	
public Q_SLOTS:
	void checkForUpdates ();
	void update ();

protected:
	void changeEvent (QEvent *e);

private Q_SLOTS:
	void refreshDescription ();
	void downloadFinished ();
	void downloadProgress (qint64 bytesReceived, qint64 bytesTotal);

private:
	void clearDownloadProgress ();
	
private:
	Ui::UpdaterWidget *ui_;
	Core::UpdaterPtr updater_;
	Core::ProductVersionList productVersionList_;
	Core::UpdatesModel *model_;
};
}
#endif // UPDATERWIDGET_H
