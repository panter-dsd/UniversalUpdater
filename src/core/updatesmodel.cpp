#include <QtCore/QStringList>
#include <QtCore/QDebug>

#include <algorithm>

#include "core.h"

#include "updatesmodel.h"

namespace Core
{
UpdatesModel::UpdatesModel (Core::AbstractUpdater* updater, QObject* parent)
		: QAbstractItemModel (parent), updater_ (updater)
{
	connect (updater_, SIGNAL (checkFinished()),
			 this, SLOT (refreshUpdatesList()));

	refreshUpdatesList ();
}

UpdatesModel::~UpdatesModel()
{

}

void UpdatesModel::refreshUpdatesList ()
{
	allUpdates_ = updater_->allUpdates();
	availableUpdates_ = updater_->availableUpdates();

	const QModelIndex &idx = index (0, 0);
	const ProductVersion &version = productVersionForIndex (idx);

	if (availableUpdates_.find (version) != availableUpdates_.end()) {
		checkedItem_ = idx;
	}

	reset ();
}


ProductVersion UpdatesModel::productVersionForIndex (const QModelIndex& index) const
{
	int row = index.row();

	if (row < 0 || row >= allUpdates_.size()) {
		return ProductVersion ();
	}

	ProductVersionList::const_iterator it = allUpdates_.begin();

	const ProductVersionList::const_iterator &end = allUpdates_.end();

	for (; it != end && row > 0; ++it, --row);

	return it == allUpdates_.end() ? ProductVersion () : *it;
}

QString UpdatesModel::versionDescription (const ProductVersion &version) const
{
	QStringList html;
	html.push_back ("<p>");
	html.push_back ("<b>" + QObject::tr ("Date") + ":</b> "
					+ version.productDate().toString());

	html.push_back ("<p>");
	html.push_back ("<b>" + QObject::tr ("Update size") + ":</b> "
					+ Core::stringSize (version.productSize()));

	html.push_back ("<p>");
	html.push_back ("<b>" + QObject::tr ("Size for download") + ":</b> "
					+ Core::stringSize (updater_->isDownloaded (version)
										? 0
										: version.productSize()));

	html.push_back ("<p>");
	html.push_back (version.productDescriptions() [Core::currentLocale() ]);

	return html.join ("\n");
}

QVariant UpdatesModel::data (const QModelIndex& index, int role) const
{
	const ProductVersion &version = productVersionForIndex (index);

	if (version.empty()) {
		return QVariant ();
	}

	QVariant value;

	switch (role) {

		case Qt::DisplayRole:
			value = version.productNames() [currentLocale() ]
					+ " "
					+ version.productVersion();
			break;

		case Qt::ToolTipRole:
			value = versionDescription (version);
			break;

		case Qt::CheckStateRole:
			value = (checkedItem_ == index) ? Qt::Checked : Qt::Unchecked;
			break;
	}

	return value;
}

int UpdatesModel::columnCount (const QModelIndex& parent) const
{
	return 1;
}

int UpdatesModel::rowCount (const QModelIndex& parent) const
{
	return allUpdates_.size();
}

bool UpdatesModel::setData (const QModelIndex& index, const QVariant& value, int role)
{
	if (role == Qt::CheckStateRole) {
		const QModelIndex oldChecked = checkedItem_;
		checkedItem_ = index;

		if (oldChecked.isValid()) {
			emit dataChanged (oldChecked, oldChecked);
		}

		return true;
	}

	return QAbstractItemModel::setData (index, value, role);
}

QVariant UpdatesModel::headerData (int section, Qt::Orientation orientation, int role) const
{
	return QAbstractItemModel::headerData (section, orientation, role);
}

Qt::ItemFlags UpdatesModel::flags (const QModelIndex& index) const
{
	Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	const ProductVersion &version = productVersionForIndex (index);

	const ProductVersionList::const_iterator &it = std::find (availableUpdates_.begin(),
			availableUpdates_.end(),
			version);

	if (it != availableUpdates_.end()) {
		flags |= Qt::ItemIsUserCheckable;
	}

	return flags;
}

QModelIndex UpdatesModel::index (int row,
								 int column,
								 const QModelIndex& parent) const
{
	return parent.isValid() ? QModelIndex () : createIndex (row, column);
}

}
