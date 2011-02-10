#include "qtwebupdater.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

namespace Gui {
MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent), ui_(new Ui::MainWindow)
{
    ui_->setupUi(this);

	Core::Config config;
	config ["UpdateConfigUrl"] = "http://127.0.0.1/version.xml";
	updater_ = UpdaterPtr (new Core::QtWebUpdater (config, this));
	connect (ui_->actionCheckForUpdates, SIGNAL (triggered ()),
			 updater_.data (), SLOT (checkForUpdates()));

	
	connect (updater_.data (), SIGNAL (checkFinished()),
			 this, SLOT (refreshUpdatesList()));

	Core::ProductVersion pv;
	pv.setProductID("converter_network");
	pv.setProductVersion("1.3.0.1");
	updater_->setCurrentProductVersion(pv);
}

MainWindow::~MainWindow()
{
    delete ui_;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui_->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::refreshUpdatesList ()
{
	ui_->updatesList->clear();
	productVersionList_ = updater_->availableUpdates ();

	QListWidgetItem *item;

	for (Core::ProductVersionList::const_iterator it = productVersionList_.begin(),
		end = productVersionList_.end(); it != end; ++it) {
		item = new QListWidgetItem (it->productVersion());
	ui_->updatesList->insertItem(0, item);
		}
}
}