#include <QtCore/QUrl>
#include <QtCore/QCoreApplication>
#include <QtCore/QSharedPointer>
#include <QtCore/QDebug>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include "qtwebupdater.h"

namespace Core {

QtWebUpdater::QtWebUpdater (QObject *parent)
: AbstractUpdater (parent)
{
	manager_ = QNetworkAccessManagerPtr (new QNetworkAccessManager (this));
}

QtWebUpdater::~QtWebUpdater ()
{
}

AbstractUpdater* QtWebUpdater::clone_p () const
{
	return new QtWebUpdater (parent());
}

bool QtWebUpdater::isValid_p (const Config& config) const
{
	return config ["UpdateType"] == "Web";
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
	emit checkFinished ();
}

bool QtWebUpdater::isFinished_p () const
{
	return !reply_ || reply_->isFinished ();
}

}
