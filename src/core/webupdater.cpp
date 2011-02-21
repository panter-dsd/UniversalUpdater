#include <QtCore/QUrl>
#include <QtCore/QCoreApplication>
#include <QtCore/QSharedPointer>
#include <QtCore/QCryptographicHash>
#include <QtCore/QDebug>

#include <QtGui/QDesktopServices>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include "webupdater.h"

namespace Core
{

WebUpdater::WebUpdater (QObject *parent)
		: AbstractUpdater (parent)
{
	manager_ = QNetworkAccessManagerPtr (new QNetworkAccessManager (this));
}

WebUpdater::~WebUpdater ()
{
}

AbstractUpdater* WebUpdater::clone_p () const
{
	return new WebUpdater (parent());
}

bool WebUpdater::isValid_p (const QString& protocol) const
{
	return protocol == "Web";
}

void WebUpdater::getUpdateConfig_p ()
{
	const QUrl url (config_ ["UpdateConfigUrl"]);

	const QNetworkRequest request (url);

	reply_ = QNetworkReplyPtr (manager_->get (request));

	connect (reply_.data (), SIGNAL (finished ()),
			 this, SLOT (updateConfigDownloaded()));
}

QString outputFileName (const QString& dir, const QString& url)
{
	const int index = url.lastIndexOf ("/");
	return index > 0 ? dir + url.mid (index) : "";
}

QString md5hash (const QString& fileName)
{
	QFile file (fileName);

	if (!file.open (QIODevice::ReadOnly)) {
		return QString ();
	}

	QCryptographicHash hash (QCryptographicHash::Md5);

	while (!file.atEnd()) {
		const QByteArray &buf = file.read (10240);
		hash.addData (buf);
	}

	return hash.result().toHex();
}

bool isFileCorrect (const QString& fileName, const QString& md5)
{
	return md5hash (fileName) == md5;
}

QString WebUpdater::downloadUpdate_p (const ProductVersion& version,
									  const QString& dir)
{
	outputFile_.setFileName (outputFileName (dir, version.productUrl ()));

	if (outputFile_.exists()
			&& isFileCorrect (outputFile_.fileName(), version.productMd5sum())) {
		qDebug () << "File alredy downloaded";
		emit downloadFinished ();
		return outputFile_.fileName();
	}

	if (!outputFile_.open (QIODevice::WriteOnly | QIODevice::Truncate)) {
		qDebug () << "Error open file " << outputFile_.fileName ();
	}

	qDebug () << version.productUrl () << outputFile_.fileName();

	const QUrl url (version.productUrl ());

	const QNetworkRequest request (url);

	reply_ = QNetworkReplyPtr (manager_->get (request));

	connect (reply_.data (), SIGNAL (downloadProgress (qint64, qint64)),
			 this, SIGNAL (downloadProgress (qint64, qint64)));
	connect (reply_.data (), SIGNAL (readyRead()),
			 this, SLOT (readyRead()));
	connect (reply_.data (), SIGNAL (finished()),
			 this, SLOT (updateDownloaded()));

	connect (reply_.data (), SIGNAL (finished ()),
			 this, SIGNAL (downloadFinished()));

	return outputFile_.fileName();
}

void WebUpdater::installUpdate_p (const QString &fileName)
{
	if (!fileName.isEmpty() && QFile::exists (fileName)) {
		QDesktopServices::openUrl (QUrl::fromLocalFile (fileName));
	}
}

bool WebUpdater::isFinished_p () const
{
	return !reply_ || reply_->isFinished ();
}

void WebUpdater::updateConfigDownloaded ()
{
	if (reply_.data ()->error() != QNetworkReply::NoError) {
		lastError_ = CheckError;
	} else {
		updateConfig_ = reply_->readAll ();
	}

	emit checkFinished ();
}

void WebUpdater::updateDownloaded ()
{
	outputFile_.close();

	if (reply_.data ()->error() != QNetworkReply::NoError) {
		lastError_ = DownloadError;
	}

	if (outputFile_.size() == 0
			|| lastError_ != NoError) {
		outputFile_.remove();
	}
}

void WebUpdater::readyRead ()
{
	outputFile_.write (reply_.data ()->readAll ());
}

}