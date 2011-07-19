#include <QtCore/QSettings>

#include <QtGui/QWidget>

#include "widgetstatesettings.h"

class SettingsGroupChanger
{
public:
	SettingsGroupChanger (QSettings *settings, const QString &group)
		: settings_ (settings), group_ (group) {
		assert (settings);

		currentGroup_ = settings->group ();
		cleanGroup ();
		settings->beginGroup (group_);
	}

	~SettingsGroupChanger () {
		cleanGroup ();
		settings_->beginGroup (currentGroup_);
	}

private:
	void cleanGroup () {
		while (!settings_->group ().isEmpty()) {
			settings_->endGroup ();
		}
	}

private:
	QSettings *settings_;
	QString group_;
	QString currentGroup_;
};

namespace Utils
{

void WidgetStateSettings::load (QSettings *settings)
{
	setQSettings (settings);
	assert (settings_);

	SettingsGroupChanger groupChanger (settings_, group_);

	const QPoint pos = settings_->value ("pos", defaultValues_ ["pos"]).toPoint();

	if (!pos.isNull()) {
		widget_->move (pos);
	}

	const QSize size = settings_->value ("size", defaultValues_ ["size"]).toSize();

	widget_->resize (size);

	const bool isMaximized = settings_->value ("IsMaximized",
											   defaultValues_ ["IsMaximized"]).toBool();

	if (isMaximized) {
		widget_->setWindowState (Qt::WindowMaximized);
	}
}

void WidgetStateSettings::save (QSettings *settings)
{
	setQSettings (settings);
	assert (settings_);

	SettingsGroupChanger groupChanger (settings_, group_);

	if (widget_->windowState() != Qt::WindowMaximized) {
		settings_->setValue ("pos", widget_->pos());
		settings_->setValue ("size", widget_->size());
		settings_->setValue ("IsMaximized", false);
	} else {
		settings_->setValue ("IsMaximized", true);
	}
}

void WidgetStateSettings::initDefaultValues ()
{
	defaultValues_ ["pos"] = QPoint ();
	defaultValues_ ["size"] = QSize (640, 480);
	defaultValues_ ["IsMaximized"] = false;
}
}
