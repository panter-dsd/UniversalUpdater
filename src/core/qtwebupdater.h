#ifndef QTWEBUPDATER_H
#define QTWEBUPDATER_H

#include <QtCore/QSharedPointer>

#include "abstractupdater.h"

class QNetworkAccessManager;
class QNetworkReply;

namespace Core {
class QtWebUpdater : public AbstractUpdater
{
	Q_OBJECT

public:
	explicit QtWebUpdater (const Config& config, QObject *parent = 0);
	~QtWebUpdater ();


private:
	virtual void getUpdateConfig_p ();
	virtual void downloadUpdate_p (const ProductVersion& version,
						   const QString& dir = QString ()) const;
	virtual void installUpdate_p (const ProductVersion& version,
						  const QString& dir = QString ()) const;
	virtual bool isFinished_p () const;

private Q_SLOTS:
	void updateConfigDownloaded ();

private:
	typedef QSharedPointer <QNetworkAccessManager> QNetworkAccessManagerPtr;
	QNetworkAccessManagerPtr manager_;
	typedef QSharedPointer <QNetworkReply> QNetworkReplyPtr;
	QNetworkReplyPtr reply_;
};
}
#endif // QTWEBUPDATER_H
