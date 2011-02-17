#ifndef QTWEBUPDATER_H
#define QTWEBUPDATER_H

#include <QtCore/QSharedPointer>
#include <QtCore/QFile>

#include "abstractupdater.h"

class QNetworkAccessManager;

class QNetworkReply;

namespace Core
{

class QtWebUpdater : public AbstractUpdater
{
	Q_OBJECT

public:
	explicit QtWebUpdater (QObject *parent = 0);
	~QtWebUpdater ();


private:
	virtual AbstractUpdater *clone_p () const;
	virtual bool isValid_p (const QString& protocol) const;
	virtual void getUpdateConfig_p ();
	virtual void downloadUpdate_p (const ProductVersion& version,
								   const QString& dir = QString ());
	virtual void installUpdate_p (const ProductVersion& version,
								  const QString& dir = QString ()) const;
	virtual bool isFinished_p () const;

private Q_SLOTS:
	void updateConfigDownloaded ();
	void downloadFinish ();
	void readyRead ();

private:
	typedef QSharedPointer <QNetworkAccessManager> QNetworkAccessManagerPtr;
	QNetworkAccessManagerPtr manager_;
	typedef QSharedPointer <QNetworkReply> QNetworkReplyPtr;
	mutable QNetworkReplyPtr reply_;
	QFile outputFile_;
};
}
#endif // QTWEBUPDATER_H
