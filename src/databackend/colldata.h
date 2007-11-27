#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#endif
#ifndef BOOST_BIND
#define BOOST_BIND
#include <boost/bind.hpp>
#endif
#ifndef COLLDATA_H
#define COLLDATA_H
#include "../databackend.h"
#include <QObject>
#include <QString>
#include <QList>
#include <QVariant>
#include <QStringList>
#include <string>

class CollData:public QObject
	{	Q_OBJECT
	private:
		DataBackend * conn;
		QStringList collectionsOnly;
		QStringList playlistsOnly;
	public:
		CollData(DataBackend * c,QObject * parent = 0);
		QStringList getCollections();
		QStringList getPlaylists();
//		QStringList getAllSorted();
		bool getPlaylistsFromServer(const Xmms::List<std::string>& list);
		bool getCollectionsFromServer(const Xmms::List<std::string>& list);
		void createCollection(const Xmms::Coll::Coll&,std::string,Xmms::Collection::Namespace);
		QString collAsQString(const Xmms::Coll::Coll&);
	public slots:
		void respondToCollectionChangedBroadcast(const Xmms::Dict& change);
	signals:
		void updated(QString /*name*/, Xmms::Collection::Namespace);
		void playlistsChanged(QStringList /*newList*/);
		void collectionsChanged(QStringList /*newList*/);
//		void allChanged(QStringList /*newList*/);
	};

#endif

