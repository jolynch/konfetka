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
#include <string>

class MediaInfo{
	private:
	QHash<QString,QVariant> songInfo;
	QHash<QString,QString> infoSource;
	public:
	MediaInfo();
	MediaInfo(QHash<QString,QVariant>);
	QVariant info(QString);
	void setInfo(QString,QVariant);
	void setSource(QString,QString);
	QHash<QString,QVariant> allInfo();
	void setAllInfo(QHash<QString,QVariant>);
};

class MlibData:public QObject {
	Q_OBJECT
	///MLIB DATA
	///WHATEVER IS NEEDED
	private:
	DataBackend * conn;
	QHash<uint,MediaInfo*> cache;
	QStringList standardTags;
	QTimer changeTimer;
	
	public:
	MlibData(DataBackend *,QObject * parent = 0);
	QVariant getInfo(std::string property, uint id);
	QVariant getInfo(QString property, uint id);
	QStringList getStandardTags();

	void clearCache();
	void getInfoFromServer(uint id);
	void getListFromServer(Xmms::Coll::Coll* coll,QString property);
	bool gotList(std::string property,const Xmms::List <Xmms::Dict> &list);
	bool getMediaInfo(const Xmms::PropDict& info);
	bool mlibChanged(const unsigned int& id);
	bool getAllMediaInfoForId(int,std::string,Xmms::Dict::Variant,std::string);
	
	signals:
	void infoChanged(int id);
	void gotListFromServer(QString property, QList<QString> info);
	void updatesDone();

};

#endif



