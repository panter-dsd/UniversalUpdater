#include <QtCore/QSettings>

#include "qtwebupdater.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

namespace Gui {

MainWindow::UpdaterCache MainWindow::updaterCache_;

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent), ui_(new Ui::MainWindow)
{
    ui_->setupUi(this);
	initializeUpdaterCache ();
	loadConfig ();
/*
	ui_->productBox->addItem("Converter");
	ui_->productBox->setItemData(0, "converter_network", Qt::UserRole);

	Core::Config config;
	config ["UpdateConfigUrl"] = "http://127.0.0.1/version.xml";
	updater_ = UpdaterPtr (new Core::QtWebUpdater (this));
	connect (ui_->actionCheckForUpdates, SIGNAL (triggered ()),
			 updater_.data (), SLOT (checkForUpdates()));

	
	connect (updater_.data (), SIGNAL (checkFinished()),
			 this, SLOT (refreshUpdatesList()));

	Core::ProductVersion pv;
	pv.setProductID("converter_network");
	pv.setProductVersion("1.3.0.1");
	updater_->setCurrentProductVersion(pv);
	*/
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

void MainWindow::initializeUpdaterCache ()
{
	updaterCache_.push_back (new Core::QtWebUpdater (this));
}

void MainWindow::loadConfig ()
{
	QSettings settings ("/home/panter/program/SimiconUpdater/share/example/updater.ini",
						QSettings::IniFormat);
	settings.beginGroup("PRODUCTS");

	foreach (const QString& group, settings.childGroups()) {
		settings.beginGroup(group);
		ui_->productBox->addItem (settings.value("Name", group).toString(), group);
		settings.endGroup();
	}
	settings.endGroup();
}

void MainWindow::refreshUpdatesList ()
{/*
	ui_->updatesList->clear();
	productVersionList_ = updater_->availableUpdates ();

	QListWidgetItem *item;

	for (Core::ProductVersionList::const_iterator it = productVersionList_.begin(),
		end = productVersionList_.end(); it != end; ++it) {
		item = new QListWidgetItem (it->productNames() ["ru"] + " " + it->productVersion());
	item->setData(Qt::CheckStateRole, Qt::Unchecked);
	ui_->updatesList->insertItem(0, item);
		}
		if (ui_->updatesList->count() > 0) {
			ui_->updatesList->item(0)->setData(Qt::CheckStateRole, Qt::Checked);
		}*/
}
}
