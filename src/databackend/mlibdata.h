#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#endif

#ifndef BOOST_BIND
#define BOOST_BIND
#include <boost/bind.hpp>
#endif

#ifndef MLIBDATA_H
#define MLIBDATA_H
#include "../databackend.h"
#include <QObject>
#include <QString>
#include <QVariant>
#include <QHash>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QTime>
#include <QTimer>
#include <QUrl>
#include <QQueue>
#include <QTimer>
#include <QBuffer>
#include <QtDebug>


//Simple class to store information about an entry in the Media Library
class MediaInfo{
	private:
	QHash<QString,QVariant> songInfo; //Hash storing property->value [Ex "artist"->QVariant("311")]
	QHash<QString,QString> infoSource; //Hash storing property->source [Ex "artist"->"mad"]
	
	public:
	//constructors
	MediaInfo();
	MediaInfo(QHash<QString,QVariant>);
	//getters
	QVariant info(QString property);
	QString source(QString property);
	//convenience function, makes it easy for other things to tell if a song is valid (status!=3)
	bool isValid(); 
	//setters
	void setInfo(QString,QVariant);
	void setSource(QString,QString);
	//convenience getter/setter for all properties
	QHash<QString,QVariant> allInfo();
	void setAllInfo(QHash<QString,QVariant>);
};

//Backend data for medialib information.  Everything revolves around "getInfo" which
//will give data about an id, or if it doesn't have it go get it from the server
class MlibData:public QObject {
	Q_OBJECT
	
	private:
                DataBackend * conn;
		//Requests for new Ids are queued and then fetched every half second to reduce lag gui-side
		QQueue<int> waitingIds;
		QTimer waitTimer;
		QTimer changeTimer;
		//This is all the ids that have been fetched so far, does not contain all ids in the mlib
		QHash<int,MediaInfo*> cache;
		//What tags gui should supply
		QStringList standardTags;
		//Periodically tells gui-side to update
		//TODO actually do this
		QTimer periodicUpdateTimer;
		
		public:
		MlibData(DataBackend *c,QObject * parent = 0);
		//Pass me hex for all I care, it is getting decoded... well not really
		QVariant getInfo(std::string property, int id);
		QVariant getInfo(QString property, int id);
		QVariant getInfo(const char* property, int id);
		//Kind of a useless check, but wth
		bool hasInfo(int id);
		//Allows access to standard tags
		QStringList getStandardTags();

		void clearCache();
		void getInfoFromServer(int id);
		void getListFromServer(Xmms::Coll::Coll* coll,QString property);
		bool gotList(std::string property,const Xmms::List <Xmms::Dict> &list);
		bool getMediaInfo(const Xmms::PropDict& info);
		bool mlibChanged(const   int& id);
		bool getAllMediaInfoForId(int,std::string,Xmms::Dict::Variant,std::string);
		//very important, lets us store album art in our caches ... for now too much mem is used though
		//bool gotAlbumCover(int id,const Xmms::bin& res);
		//QImage getAlbumArtForId(int id);
	public slots:
		void fetchSomeMore();
	
	signals:
		void infoChanged(int id);
		void gotListFromServer(QString property, QList<QString> info);
		void updatesDone();
		void periodicUpdate();
	public:
		static const int NoInfo = -1;
};

#endif



