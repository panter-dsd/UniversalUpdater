#include <QtGui/QPushButton>

#include "updatepreferencewidget.h"

#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

namespace Gui
{

PreferencesDialog::PreferencesDialog (const QSettings& settings,
									  const Core::UpdatersList& updatersList,
									  QWidget* parent)
		: QDialog (parent), ui_ (new Ui::PreferencesDialog),
		settings_ (settings.fileName(), settings.format()),
		updatersList_ (updatersList)
{
	ui_->setupUi (this);

	ui_->buttonBox->button (QDialogButtonBox::Apply)->setEnabled (false);

	connect (ui_->buttonBox->button (QDialogButtonBox::Apply), SIGNAL (clicked(bool)),
		this, SLOT (save()));
	connect (ui_->buttonBox->button (QDialogButtonBox::Ok), SIGNAL (clicked(bool)),
			 this, SLOT (save()));

	if (!updatersList_.isEmpty()) {
		for (Core::UpdatersList::const_iterator it = updatersList_.begin(),
				end = updatersList_.end(); it != end; ++it) {
			addPage (new UpdatePreferenceWidget (*it, &settings_, this));
		}
	}
	
	loadSettings ();
}

PreferencesDialog::~PreferencesDialog()
{
	saveSettings();
}

void PreferencesDialog::changeEvent (QEvent* e)
{
	QWidget::changeEvent (e);

	switch (e->type()) {

		case QEvent::LanguageChange:
			ui_->retranslateUi (this);
			break;

		default:
			break;
	}
}

void PreferencesDialog::addPage (Gui::AbstractPreferenceWidget* widget)
{
	pages_.push_back (widget);
	ui_->updatePreferences->addTab (widget,
									widget->windowIcon(),
									widget->windowTitle());

	connect (widget, SIGNAL (preferenceChanged()),
			 this, SLOT (pageChanged()));
}

void PreferencesDialog::pageChanged ()
{
	ui_->buttonBox->button (QDialogButtonBox::Apply)->setEnabled (true);
}

void PreferencesDialog::save ()
{
	for (Pages::const_iterator it = pages_.constBegin(),
			end = pages_.constEnd(); it != end; ++it) {
		if ((*it)->isChanged()) {
			(*it)->save();
		}
	}
	ui_->buttonBox->button (QDialogButtonBox::Apply)->setEnabled (false);
}

void PreferencesDialog::loadSettings ()
{
	settings_.beginGroup("GUI");
	settings_.beginGroup("PreferencesDialog");

	const QPoint pos = settings_.value("pos").toPoint();
	if (!pos.isNull()) {
		move (pos);
	}
	const QSize size = settings_.value("size", QSize(640, 480)).toSize();
	resize (size);
	const bool isMaximized = settings_.value("IsMaximized", false).toBool();
	if (isMaximized) {
		setWindowState(Qt::WindowMaximized);
	}
	
	settings_.endGroup();
	settings_.endGroup();
}

void PreferencesDialog::saveSettings ()
{
	settings_.beginGroup("GUI");
	settings_.beginGroup("PreferencesDialog");
	
	if (windowState() != Qt::WindowMaximized) {
		settings_.setValue("pos", pos());
		settings_.setValue("size", size());
		settings_.setValue("IsMaximized", false);
	} else {
		settings_.setValue("IsMaximized", true);
	}

	
	settings_.endGroup();
	settings_.endGroup();
	settings_.sync();
}
}
