#include <QtCore/QUrl>
#include <QtCore/QCoreApplication>
#include <QtCore/QSharedPointer>
#include <QtCore/QDebug>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include "qtwebupdater.h"

namespace Core {

QtWebUpdater::QtWebUpdater (const Config& config)
: AbstractUpdater (config)
{
	manager_ = QNetworkAccessManagerPtr (new QNetworkAccessManager (this));
}

QtWebUpdater::~QtWebUpdater ()
{
}

void QtWebUpdater::getUpdateConfig_p ()
{
	const QUrl url (config_ ["UpdateConfigUrl"]);
	
	const QNetworkRequest request (url);
	
	reply_ = QNetworkReplyPtr (manager_->get (request));

	connect (reply_.data (), SIGNAL (finished ()),
		this, SLOT (updateConfigDownloaded()));
}

void QtWebUpdater::downloadUpdate_p (const ProductVersion& version,
									 const QString& dir) const
{
}

void QtWebUpdater::installUpdate_p (const ProductVersion& version,
									const QString& dir) const
{
}

void QtWebUpdater::updateConfigDownloaded ()
{
	updateConfig_ = reply_->readAll ();
}

bool QtWebUpdater::isFinished_p () const
{
	return !reply_ || reply_->isFinished ();
}

}
