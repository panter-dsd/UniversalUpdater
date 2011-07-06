#ifndef WIDGETSTATESETTINGS_H
#define WIDGETSTATESETTINGS_H

#include <QtCore/QObject>

#include <assert.h>

class QWidget;
class QSettings;

namespace Utils
{
class WidgetStateSettings
{

public:
	explicit WidgetStateSettings (QWidget *widget, QSettings *settings = 0)
		: widget_ (widget), settings_ (settings),
		  group_ ("GUI/MainWindow") {
		assert (widget_);
	}

	explicit WidgetStateSettings (QWidget *widget, const QString &group,
								  QSettings *settings = 0)
		: widget_ (widget), settings_ (settings),
		  group_ (group) {
		assert (widget_);
	}

	void load (QSettings *settings = 0);
	void save (QSettings *settings = 0);

	QString group () const {
		return group_;
	}

	void setGroup (const QString &group) {
		group_ = group;
	}

	void setQSettings (QSettings *settings) {
		if (settings) {
			settings_ = settings;
		}
	}

private:
	WidgetStateSettings (const WidgetStateSettings &other);
	WidgetStateSettings &operator= (const WidgetStateSettings &other);

private:
	QWidget *widget_;
	QSettings *settings_;
	QString group_;
};
}
#endif // WIDGETSTATESETTINGS_H
