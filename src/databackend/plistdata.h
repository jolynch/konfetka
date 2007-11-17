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
#include <string>


class SinglePlaylist:public QAbstractItemModel
	{	Q_OBJECT
	private:
		bool connected;
		QList <uint> ids;
		DataBackend * conn;
		QStringList header;
		QStringList humanReadableHeader;

		void parseHumanReadableHeader();

	public:
		SinglePlaylist(DataBackend * c,QStringList hv_=QStringList());
		bool isConnected();
		void connectToggle();
		void setHeader(QStringList newVal);

		bool setInitialPlist(const Xmms::List< unsigned int > &list);

		//QAbstractItemModel function implementations
		QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
		QModelIndex parent ( const QModelIndex & index ) const;
		QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
		int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
		int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
//		bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;
		

	public slots:
		void infoChanged(int id);
//		void respondToChanges(const Xmms::Dict& val);
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


