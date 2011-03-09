#include <QtCore/QUrl>
#include <QtCore/QCoreApplication>
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
		: AbstractUpdater (parent), manager_ (new QNetworkAccessManager (this)),
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

	replyList.push_back (reply_);

	connect (reply_, SIGNAL (finished ()),
			 this, SLOT (updateConfigDownloaded()));

	connect (reply_, SIGNAL (finished()),
			 this, SLOT (replyFinished()));
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
		return outputFile_.fileName();
	}

	qDebug () << version.productUrl () << outputFile_.fileName();

	const QUrl url (version.productUrl ());

	const QNetworkRequest request (url);

	QNetworkReply *reply_ = manager_->get (request);
	replyList.push_back (reply_);

	connect (reply_, SIGNAL (downloadProgress (qint64, qint64)),
			 this, SIGNAL (downloadProgress (qint64, qint64)));
	connect (reply_, SIGNAL (readyRead()),
			 this, SLOT (readyRead()));
	connect (reply_, SIGNAL (finished()),
			 this, SLOT (updateDownloaded()));
	connect (reply_, SIGNAL (finished()),
			 this, SLOT (replyFinished()));

	return outputFile_.fileName();
}

void WebUpdater::installUpdate_p (const Core::ProductVersion& version, const QString& dir)
{
	const QString &name = outputFileName (dir, version.productUrl ());

	const bool isCorrect = isFileCorrect (name, version.productMd5sum());
	if (!isCorrect) {
		QFile::remove(name);
	}
	
	lastError_ = isCorrect
				  && QProcess::startDetached (name,
											  config_.value ("InstallerParameters").toStringList())
				  ? NoError : InstallError;
	emit stateChanged (InstallFinishedState);
}

bool WebUpdater::isFinished_p () const
{
	return replyList.isEmpty();
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

	emit checkFinished ();
	emit stateChanged (CheckFinishedState);
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
			|| lastError_ != NoError) {
		outputFile_.remove();
	}

	emit downloadFinished ();
	emit stateChanged (DownloadFinishedState);
}

void WebUpdater::readyRead ()
{
	QNetworkReply *reply_ = qobject_cast <QNetworkReply*> (sender());
	assert (reply_);

	/*This is for testing
	while (!reply_->atEnd()) {
		outputFile_.write(reply_->read(1));
	}
	*/
	outputFile_.write (reply_->readAll ());
}

void WebUpdater::stopUpdate_p ()
{
	for (ReplyList::const_iterator it = replyList.begin(),
			end = replyList.end(); it != end; ++it) {
		(*it)->abort ();
	}
	
	emit stateChanged (StopedState);
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

void WebUpdater::replyFinished ()
{
	QNetworkReply *reply_ = qobject_cast <QNetworkReply*> (sender());
	assert (reply_);

	const int index = replyList.indexOf (reply_);
	replyList.remove (index);
	reply_->deleteLater();
}

}
