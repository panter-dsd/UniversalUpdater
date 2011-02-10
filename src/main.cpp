#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

//TEST
#include "qtwebupdater.h"
#include <iostream>
//TEST

int main (int argc, char **argv)
{
	QCoreApplication app (argc, argv);

	Core::Config config;
	config ["UpdateConfigUrl"] = "http://127.0.0.1/version.xml";

	Core::ProductVersion pv;
	pv.setProductID("converter_network");
	pv.setProductVersion("1.3.0");
	
	Core::QtWebUpdater u (config);
	u.setCurrentProductVersion(pv);
	u.checkForUpdates();
	while (!u.isFinished()) {app.processEvents();}
	
	Core::ProductVersionList l = u.availableUpdates ();

	for (Core::ProductVersionList::const_iterator it = l.begin(),
		end = l.end(); it != end; ++it) {
		qDebug () << it->productUrl();
	}

	//return app.exec ();
	return 0;
}