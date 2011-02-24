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

const int bufferSize = 10240;

namespace Core
{

WebUpdater::WebUpdater (QObject *parent)
: AbstractUpdater (parent), manager_ (new QNetworkAccessManager (0))
{
}

WebUpdater::~WebUpdater ()
{
}

AbstractUpdater* WebUpdater::clone_p () const
{
	return new WebUpdater;
}

bool WebUpdater::isValid_p (const QString& protocol) const
{
	return protocol == "Web";
}

void WebUpdater::getUpdateConfig_p ()
{
	if (reply_.data() && reply_->isRunning()) {
		return;
	}
	
	const QUrl url (config_.value("UpdateConfigUrl"));
	if (url.isEmpty()) {
		return;
	}

	const QNetworkRequest request (url);

	reply_ = QNetworkReplyPtr (manager_->get (request));

	connect (reply_.data (), SIGNAL (finished ()),
			 this, SLOT (updateConfigDownloaded()));
}

QString outputFileName (const QString& dir, const QString& url)
{
	const int index = url.lastIndexOf ("/");
	return index > 0 ? dir + url.mid (index) : QString ();
}

QString md5hash (const QString& fileName)
{
	QFile file (fileName);

	if (!file.open (QIODevice::ReadOnly)) {
		return QString ();
	}

	QCryptographicHash hash (QCryptographicHash::Md5);

	while (!file.atEnd()) {
		const QByteArray &buf = file.read (bufferSize);
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

	return outputFile_.fileName();
}

void WebUpdater::installUpdate_p (const QString &fileName)
{
	const QString name = fileName.isEmpty()
						 ? outputFile_.fileName()
						 : fileName;

	if (!name.isEmpty() && QFile::exists (name)) {
		QDesktopServices::openUrl (QUrl::fromLocalFile (name));
	}
}

bool WebUpdater::isFinished_p () const
{
	return !reply_.data () || reply_->isFinished ();
}

void WebUpdater::updateConfigDownloaded ()
{
	Q_ASSERT (reply_.data());
	
	if (reply_->error() != QNetworkReply::NoError) {
		lastError_ = CheckError;
		errorText_ = reply_->errorString();
	} else {
		updateConfig_ = reply_->readAll ();
	}

	emit checkFinished ();
}

void WebUpdater::updateDownloaded ()
{
	outputFile_.close();

	if (reply_->error() != QNetworkReply::NoError) {
		lastError_ = DownloadError;
		errorText_ = reply_->errorString();
	}

	if (outputFile_.size() == 0
			|| lastError_ != NoError
			|| !isFileCorrect (outputFile_.fileName(), workVersion_.productMd5sum())) {
		outputFile_.remove();
	}
	
	emit downloadFinished ();
}

void WebUpdater::readyRead ()
{
	outputFile_.write (reply_.data ()->readAll ());
}

void WebUpdater::stopUpdate_p ()
{
	Q_ASSERT (reply_.data());

	reply_->abort();
}

}
