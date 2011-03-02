#ifndef QTWEBUPDATER_H
#define QTWEBUPDATER_H

#include <QtCore/QSharedPointer>
#include <QtCore/QFile>

#include "abstractupdater.h"

class QNetworkAccessManager;

class QNetworkReply;

namespace Core
{

class WebUpdater : public AbstractUpdater
{
	Q_OBJECT

public:
	explicit WebUpdater (QObject *parent = 0);
	~WebUpdater ();

private:
	virtual AbstractUpdater *clone_p () const;
	virtual bool isValid_p (const QString& protocol) const;
	virtual void getUpdateConfig_p ();
	virtual QString downloadUpdate_p (const ProductVersion& version,
								   const QString& dir);
	virtual void installUpdate_p (const ProductVersion& version,
								  const QString &dir);
	virtual bool isFinished_p () const;
	virtual void stopUpdate_p ();
	virtual bool isDownloaded_p (const ProductVersion& version,
							   const QString& dir) const;

private Q_SLOTS:
	void updateConfigDownloaded ();
	void updateDownloaded ();
	void readyRead ();

private:
	typedef QSharedPointer <QNetworkAccessManager> QNetworkAccessManagerPtr;
	QNetworkAccessManagerPtr manager_;
	QFile outputFile_;
	int currentUrl_;
};
}
#endif // QTWEBUPDATER_H
