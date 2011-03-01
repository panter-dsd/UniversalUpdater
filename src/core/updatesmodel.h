#ifndef UPDATESMODEL_H
#define UPDATESMODEL_H

#include <QtCore/QAbstractItemModel>

#include "abstractupdater.h"

namespace Core
{

class UpdatesModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	UpdatesModel (const UpdaterPtr& updater, QObject* parent = 0);
	virtual ~UpdatesModel();

	virtual QVariant data (const QModelIndex& index,
						   int role = Qt::DisplayRole) const;
	virtual int columnCount (const QModelIndex& parent = QModelIndex()) const;
	virtual int rowCount (const QModelIndex& parent = QModelIndex()) const;
	virtual bool setData (const QModelIndex& index,
						  const QVariant& value,
						  int role = Qt::EditRole);
	virtual QVariant headerData (int section,
								 Qt::Orientation orientation,
								 int role = Qt::DisplayRole) const;
	virtual Qt::ItemFlags flags (const QModelIndex& index) const;
	virtual QModelIndex index (int row,
							   int column,
							   const QModelIndex & parent = QModelIndex()) const;
	virtual QModelIndex parent (const QModelIndex& index) const {
		return QModelIndex ();
	}
	
	ProductVersion version (const QModelIndex& index) {
		return productVersionForIndex (index);
	}
	
	QModelIndex checkedItem () const {
		return checkedItem_;
	}

private:
	UpdatesModel (const UpdatesModel& other);
	UpdatesModel& operator= (const UpdatesModel& other);

	ProductVersion productVersionForIndex (const QModelIndex& index) const;
	QString versionDescription (const ProductVersion &version) const;

private Q_SLOTS:
	void refreshUpdatesList ();

private:
	UpdaterPtr updater_;
	ProductVersionList allUpdates_;
	ProductVersionList availableUpdates_;
	QModelIndex checkedItem_;
};
}

#endif // UPDATESMODEL_H
