#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#endif
#ifndef PLISTDATA_H
#define PLISTDATA_H
#include "colldata.h"
#include "mlibdata.h"
#include "../databackend.h"
#include <QObject>
#include <QAbstractItemModel>
#include <QItemDelegate>
#include <QString>
#include <QHash>
#include <QStringList>
#include <QModelIndex>
#include <QVariant>
#include <QMimeData>
#include <QUrl>
#include <QDir>
#include <QSize>
#include <QFont>
#include <QColor>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QtAlgorithms>
#include <QSettings>
#include <string>
#include <list>

struct PlaylistDragInfo
	{
	QString name;
	QList <int> positions;
	};

class PlaylistDelegate:public QItemDelegate
	{	Q_OBJECT
	private:
		DataBackend * conn;
		QAbstractItemModel * model;
		Xmms::Playback::Status status;
		int pos;
		bool editing;
		QList<QVariant> ratios;
	public:
		PlaylistDelegate(QAbstractItemModel * m,DataBackend * c);
		QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;
		void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
		void setEditing(bool val);
		bool isEditing();
		int getWidthFor(int column, int totalWidth);
	public slots:
		void qsettingsValChanged(QString name, QVariant var);
		void posChanged(int p);
		void statusChanged(Xmms::Playback::Status s);
	signals:
		void ratiosChanged();
	};


class SinglePlaylist:public QAbstractItemModel
	{	Q_OBJECT
	private:
		QList <int> ids;
		DataBackend * conn;
		QStringList header;
		QStringList humanReadableHeader;
		std::string plistName;
		PlaylistDelegate * delegate;
		std::list< std::string > sortOrder;
		int insertCollectionAt;

		void parseHumanReadableHeader();
	public:
		static PlaylistDragInfo * getDragInfoFromMimeData(const QMimeData *data);

		SinglePlaylist(DataBackend * c,std::string name,QStringList hv_=QStringList());
		void setHeader(QStringList newVal);
		void setOrder(std::list< std::string > order);
		bool insertCollection(const Xmms::Coll::Coll& c);
		PlaylistDelegate * getDelegate();
		bool setInitialPlist(const Xmms::List< int > &list);
		QList<int> getIdsFromPositions(QList <int> pos);
		void forceRefresh(int row);

	//QAbstractItemModel function implementations
		QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
		Qt::ItemFlags flags(const QModelIndex &index) const;
		QModelIndex parent ( const QModelIndex & index ) const;
		QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
		int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
		int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
		QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
		bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
		Qt::DropActions supportedDropActions() const;
		bool dropMimeData(const QMimeData *data,Qt::DropAction action, int row, int column, const QModelIndex &parent);
		QStringList mimeTypes() const;
		QMimeData * mimeData(const QModelIndexList &indexes) const;
		void sort ( int column, Qt::SortOrder order = Qt::AscendingOrder );

	//Basic modification functions
	//These only modify the model itself - they do not call xmms2.
	private:
		void addToModel(int id,int pos=-1);
		void moveInModel(int oldPos,int newPos);
		void removeFromModel(int pos);
		
	//Response functions
	public slots:
		void infoChanged(int id);
		void respondToChanges(const Xmms::Dict& val);
	};

class PlistData:public QObject
	{	Q_OBJECT
	private:
		DataBackend * conn;
		std::string currentPlist;
		QHash <QString, SinglePlaylist *> plists;
		QStringList headerVals;
		std::list< std::string > sortOrder;

		void createPlaylist(std::string name);
		void refreshPlaylist(SinglePlaylist * plist,std::string name="");
	public:
		PlistData(DataBackend * c,QObject * parent=0);
		SinglePlaylist * getPlist (std::string plist="");
		QString getCurrentName();
	public slots:
		void setCurrentName(const std::string & name);
		void collectionChanged(QString name, Xmms::Collection::Namespace ns);
		void qsettingsValChanged(QString name,QVariant newVal);
		void playlistsChanged(QStringList newList);
	signals:
		void playlistReady(std::string,SinglePlaylist *);
	};

#endif


