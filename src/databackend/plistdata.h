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
#include <QString>
#include <QHash>
#include <QStringList>
#include <QModelIndex>
#include <QVariant>
#include <QMimeData>
#include <QUrl>
#include <QDir>
#include <string>


class SinglePlaylist:public QAbstractItemModel
	{	Q_OBJECT
	private:
		bool connected;
		QList <uint> ids;
		DataBackend * conn;
		QStringList header;
		QStringList humanReadableHeader;
		std::string plistName;

		void parseHumanReadableHeader();
	public:
		SinglePlaylist(DataBackend * c,std::string name,QStringList hv_=QStringList());
		bool isConnected();
		void connectToggle();
		void setHeader(QStringList newVal);

		bool setInitialPlist(const Xmms::List< unsigned int > &list);

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

	//Basic modification functions
	//These only modify the model itself - they do not call xmms2.
	private:
		void addToModel(uint id,int pos=-1);
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

		void createPlaylist(std::string name);
		void refreshPlaylist(SinglePlaylist * plist,std::string name="");

		void connectToServer(SinglePlaylist * plist);
		void disconnectFromServer(SinglePlaylist * plist);
	public:
		PlistData(DataBackend * c,QObject * parent=0);
		SinglePlaylist * getPlist (std::string plist="");
		QString getCurrentName();
	public slots:
		void setCurrentName(const std::string & name);
		void collectionChanged(QString name, Xmms::Collection::Namespace ns);
		void qsettingsValChanged(QString name,QVariant newVal);
	signals:
		void playlistReady(std::string,SinglePlaylist *);
	};

#endif


