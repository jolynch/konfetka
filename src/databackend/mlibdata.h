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
#include <string>

class MediaItem:public QTreeWidgetItem {
	private:
	QHash<QString,QVariant> songInfo;
	public:
	QVariant info(QString);
	QHash<QString,QVariant> allInfo();
	void setInfo(QHash<QString,QVariant>);
	bool matches(QString,QString);
};

class MlibData:public QObject {
	Q_OBJECT
	///MLIB DATA
	///WHATEVER IS NEEDED
	private:
	DataBackend * conn;
	QHash<uint,MediaItem*> cache;
	QStringList lookUps;
	
	public:
	MlibData(DataBackend *,QObject * parent = 0);
	QVariant getInfo(std::string property, uint id);
	QVariant getInfo(QString property, uint id);
	MediaItem* getItem(uint id);
	MediaItem* operator[] (uint id);
	
	void clearCache();
	void getItemFromServer(uint id);
	void getInfoFromServer(QString property, uint id);
	void getListFromServer(Xmms::Coll::Coll* coll,QString property);
	bool gotList(std::string property,const Xmms::List <Xmms::Dict> &list);
	bool getMediaInfo(const Xmms::PropDict& info);
	
	signals:
	void infoChanged(int id);
	void gotListFromServer(QString property, QList<QString> info);

};

#endif



