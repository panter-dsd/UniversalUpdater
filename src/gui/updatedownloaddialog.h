#ifndef UPDATEDOWNLOADDIALOG_H
#define UPDATEDOWNLOADDIALOG_H

#include <QtGui/QDialog>

#include "abstractupdater.h"

namespace Ui
{

class UpdateDownloadDialog;
}

namespace Gui
{

class UpdateDownloadDialog : public QDialog
{
	Q_OBJECT

public:
	UpdateDownloadDialog (const Core::UpdaterPtr &updater,
						  const Core::ProductVersion &version,
						  QWidget * parent = 0,
						  Qt::WindowFlags f = 0);
	virtual ~UpdateDownloadDialog ()
	{}

private:
	UpdateDownloadDialog (const UpdateDownloadDialog&);
	UpdateDownloadDialog& operator= (const UpdateDownloadDialog&);

	void installUpdate ();

private Q_SLOTS:
	void downloadFinished ();
	void downloadProgress (qint64 bytesReceived, qint64 bytesTotal);

private:
	Ui::UpdateDownloadDialog *ui_;
	Core::UpdaterPtr updater_;
	Core::ProductVersion version_;
};
}
#endif // UPDATEDOWNLOADDIALOG_H
