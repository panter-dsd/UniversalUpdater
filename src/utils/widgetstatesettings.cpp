#include <QtCore/QSettings>

#include <QtGui/QWidget>

#include "widgetstatesettings.h"

class SettingsGroupChanger
{
public:
	SettingsGroupChanger (QSettings *settings, const QString &group)
		: settings_ (settings) {
		assert (settings);

		currentGroup_ = settings->group ();
		setGroup (group);
	}

	~SettingsGroupChanger () {
		setGroup (currentGroup_);
	}

private:
	void cleanGroup () {
		while (!settings_->group ().isEmpty()) {
			settings_->endGroup ();
		}
	}

	void setGroup (const QString group) {
		cleanGroup ();
		settings_->beginGroup (group);
	}

private:
	QSettings *settings_;
	QString currentGroup_;
};

namespace Utils
{

void WidgetStateSettings::load (QSettings *settings)
{
	setQSettings (settings);
	assert (settings_);

	const SettingsGroupChanger groupChanger (settings_, group_);

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

	const SettingsGroupChanger groupChanger (settings_, group_);

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
