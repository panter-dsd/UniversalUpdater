#ifndef UPDATERWIDGET_H
#define UPDATERWIDGET_H

#include <QtGui/QWidget>

#include "abstractupdater.h"

namespace Ui {
    class UpdaterWidget;
}

namespace Gui {

typedef QSharedPointer <Core::AbstractUpdater> UpdaterPtr;

class UpdaterWidget : public QWidget
{
    Q_OBJECT

public:
	explicit UpdaterWidget (UpdaterPtr updater, QWidget *parent = 0);
    ~UpdaterWidget();

public Q_SLOTS:
	void checkForUpdates ();

protected:
    void changeEvent(QEvent *e);

private Q_SLOTS:
	void refreshUpdatesList ();
	void refreshDescription ();

private:
    Ui::UpdaterWidget *ui_;
	UpdaterPtr updater_;
	Core::ProductVersionList productVersionList_;
};
}
#endif // UPDATERWIDGET_H
