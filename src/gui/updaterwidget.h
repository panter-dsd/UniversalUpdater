#ifndef UPDATERWIDGET_H
#define UPDATERWIDGET_H

#include <QtGui/QWidget>

#include "abstractupdater.h"
#include "updatesmodel.h"

namespace Ui
{
	class UpdaterWidget;
}

using Core::AbstractUpdater;

class QLabel;

namespace Gui
{

class UpdaterWidget : public QWidget
{
	Q_OBJECT

public:
	explicit UpdaterWidget (Core::AbstractUpdater* updater, QWidget* parent = 0);
	~UpdaterWidget();

	Core::AbstractUpdater* updater () const {
		return updater_;
	}

public Q_SLOTS:
	void checkForUpdates ();
	void updateToVersion ();

Q_SIGNALS:
	void updateToVersion (Core::AbstractUpdater* updater,
						  const Core::ProductVersion& version);

protected:
	void changeEvent (QEvent *e);

private Q_SLOTS:
	void checkFinished();
	void refreshDescription ();
	void updaterStateChanged (AbstractUpdater::UpdaterState state);
	void blockWidget (const QString& text);
	void unblockWidget ();

private:
	Ui::UpdaterWidget *ui_;
	Core::AbstractUpdater *updater_;
	Core::ProductVersionList productVersionList_;
	Core::UpdatesModel *model_;
	QLabel *informationLabel_;
};
}
#endif // UPDATERWIDGET_H
