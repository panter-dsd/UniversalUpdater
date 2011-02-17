#include <algorithm>

#include <QtCore/QSettings>
#include <QtCore/QDebug>

#include "updaterfactory.h"
#include "updaterwidget.h"
#include "core.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

namespace Gui
{

MainWindow::MainWindow (QWidget *parent)
		: QMainWindow (parent), ui_ (new Ui::MainWindow)
{
	ui_->setupUi (this);

	connect (ui_->productBox, SIGNAL (activated (int)),
			 ui_->updaterWidgets, SLOT (setCurrentIndex (int)));

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

void MainWindow::changeEvent (QEvent *e)
{
	QMainWindow::changeEvent (e);

	switch (e->type()) {

		case QEvent::LanguageChange:
			ui_->retranslateUi (this);
			break;

		default:
			break;
	}
}

void MainWindow::loadConfig ()
{
	QSettings settings ("/home/panter/program/UU/share/example/updater.ini",
						QSettings::IniFormat);
	settings.beginGroup ("PRODUCTS");

	foreach (const QString& group, settings.childGroups()) {
		settings.beginGroup (group);

		if (addUpdaterWidget (group)) {
			ui_->productBox->addItem (settings.value ("Name", group).toString()
									  + " - " + settings.value ("CurrentVersion", group).toString(), group);
		}

		settings.endGroup();
	}

	settings.endGroup();

	ui_->productBox->setCurrentIndex (0);
}

bool MainWindow::addUpdaterWidget (const QString& product)
{
	QSettings settings ("/home/panter/program/UU/share/example/updater.ini",
						QSettings::IniFormat);
	settings.beginGroup ("PRODUCTS");
	settings.beginGroup (product);

	Core::Config config;
	config ["ProductID"] = product;
	foreach (const QString &key, settings.childKeys()) {
		config [key] = settings.value (key).toString();
	}

	bool ok = false;

	UpdaterPtr ptr (Core::UpdaterFactory::updaterForProtocol (config ["UpdateProtocol"]));
	if (!ptr.isNull()) {
		ptr->setConfig (config);

		UpdaterWidget *updaterWidget_ = new UpdaterWidget (ptr, this);
		connect (ui_->actionCheckForUpdates, SIGNAL (triggered ()),
				 updaterWidget_, SLOT (checkForUpdates()));

		ui_->updaterWidgets->addWidget (updaterWidget_);
		ok = true;
	}

	settings.endGroup();

	settings.endGroup();

	return ok;
}

void MainWindow::checkForUpdates ()
{
	UpdaterWidget *w = qobject_cast <UpdaterWidget*> (ui_->updaterWidgets->currentWidget ());

	if (w) {
		w->checkForUpdates();
	}
}
}
