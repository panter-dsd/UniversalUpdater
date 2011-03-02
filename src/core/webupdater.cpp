#include <QtCore/QUrl>
#include <QtCore/QCoreApplication>
#include <QtCore/QSharedPointer>
#include <QtCore/QCryptographicHash>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <QtCore/QProcess>
#include <QtCore/QFileInfo>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <assert.h>

#include "webupdater.h"

const int bufferSize = 10240;

namespace Core
{

WebUpdater::WebUpdater (QObject *parent)
		: AbstractUpdater (parent), manager_ (new QNetworkAccessManager (0)),
		currentUrl_ (0)
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
	const QStringList urls = config_.value ("UpdateConfigUrl").toStringList ();

	if (urls.isEmpty()) {
		return;
	}

	if (currentUrl_ >= urls.size ()) {
		currentUrl_ = 0;
		return;
	}

	const QUrl url (urls [currentUrl_]);

	if (url.isEmpty()) {
		return;
	}

	const QNetworkRequest request (url);

	QNetworkReply *reply_ = manager_->get (request);

	connect (reply_, SIGNAL (finished ()),
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

QString WebUpdater::downloadUpdate_p (const ProductVersion& version, const QString& dir)
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

	QNetworkReply *reply_ = manager_->get (request);

 	connect (reply_, SIGNAL (downloadProgress (qint64, qint64)),
			 this, SIGNAL (downloadProgress (qint64, qint64)));
	connect (reply_, SIGNAL (readyRead()),
			 this, SLOT (readyRead()));
	connect (reply_, SIGNAL (finished()),
			 this, SLOT (updateDownloaded()));

	return outputFile_.fileName();
}

void WebUpdater::installUpdate_p (const Core::ProductVersion& version, const QString& dir)
{
	const QString &name = outputFileName (dir, version.productUrl ());

	lastError_ =  QProcess::startDetached (name,
										   config_.value ("InstallerParameters").toStringList())
				  ? NoError : InstallError;
}

bool WebUpdater::isFinished_p () const
{
	return true;
}

void WebUpdater::updateConfigDownloaded ()
{
	QNetworkReply *reply_ = qobject_cast <QNetworkReply*> (sender());
	assert (reply_);
	
	if (reply_->error() != QNetworkReply::NoError) {
		lastError_ = CheckError;
		errorText_ = reply_->errorString();
		++currentUrl_;
		getUpdateConfig_p ();
	} else {
		updateConfig_ = reply_->readAll ();
	}
	
	reply_->deleteLater();
	emit checkFinished ();
}

void WebUpdater::updateDownloaded ()
{
	outputFile_.close();
	
	QNetworkReply *reply_ = qobject_cast <QNetworkReply*> (sender());
	assert (reply_);
	
	if (reply_->error() != QNetworkReply::NoError) {
		lastError_ = DownloadError;
		errorText_ = reply_->errorString();
	}

	if (outputFile_.size() == 0
			|| lastError_ != NoError
			|| !isFileCorrect (outputFile_.fileName(), workVersion_.productMd5sum())) {
		outputFile_.remove();
	}
	
	reply_->deleteLater();
	emit downloadFinished ();
}

void WebUpdater::readyRead ()
{
	QNetworkReply *reply_ = qobject_cast <QNetworkReply*> (sender());
	assert (reply_);
	
	outputFile_.write (reply_->readAll ());
}

void WebUpdater::stopUpdate_p ()
{

}

bool WebUpdater::isDownloaded_p (const ProductVersion& version,
								 const QString& dir) const
{
	const QString &name = outputFileName (dir, version.productUrl ());
	const QFileInfo fi (name);

	return !name.isEmpty()
		   && fi.exists ()
		   && version.productSize() == fi.size();
}

}
