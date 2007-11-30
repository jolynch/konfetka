#ifndef MLIBDATA_CPP
#define MLIBDATA_CPP
#include "mlibdata.h"

//An easy way to store a song's information, uses up some memory, but that's why it is a cache.
MediaInfo::MediaInfo() {
}

MediaInfo::MediaInfo(QHash<QString,QVariant> hash) {
	songInfo = hash;
}

QVariant MediaInfo::info(QString property) {
	if(songInfo.contains(property)) {
	return songInfo.value(property);
	}
	else
	return QVariant("Unknown");
}

void MediaInfo::setInfo(QString property,QVariant value) {
	songInfo.insert(property,value);
}

QString MediaInfo::source(QString property) {
	if(infoSource.contains(property)) {
	return infoSource.value(property);
	}
	else
	return "Unknown";	
}

bool MediaInfo::isValid() {
	return(infoSource.contains("status") && infoSource.value("status").toInt()!=3);
}

void MediaInfo::setSource(QString property, QString source) {
	infoSource.insert(property,source);
}

QHash<QString,QVariant> MediaInfo::allInfo() {
	QHash<QString,QVariant> returnVal(songInfo);
	return returnVal;
}

void MediaInfo::setAllInfo(QHash<QString,QVariant> hash) {
	songInfo = hash;
}

MlibData::MlibData(DataBackend * c,QObject * parent):QObject(parent) {
	conn = c;
	standardTags<<"artist"<<"album"<<"url"<<"encodedurl"<< "title"<<"genre"<<"duration"<<"timesplayed"<<"rating"<<"laststarted"<<"id";
	QSettings s;
	if(s.contains("standardTags"))
	standardTags = s.value("standardTags").toStringList();
	s.setValue("standardTags",standardTags);
	conn->medialib.broadcastEntryChanged()(Xmms::bind(&MlibData::mlibChanged, this));
	connect(&changeTimer,SIGNAL(timeout()),this,SIGNAL(updatesDone()));
	connect(&changeTimer,SIGNAL(timeout()),&changeTimer,SLOT(stop()));
	connect(&waitTimer,SIGNAL(timeout()),this,SLOT(fetchSomeMore()));
	connect(&periodicUpdateTimer,SIGNAL(timeout()),this,SIGNAL(periodicUpdate()));
	periodicUpdateTimer.start(10000);
}

void MlibData::fetchSomeMore() {
	for(int i=0;i<20 && !waitingIds.isEmpty();i++) {
		conn->medialib.getInfo(waitingIds.dequeue())(Xmms::bind(&MlibData::getMediaInfo,this));	
	}
	if(waitingIds.isEmpty()) {
// 	std::cout<<"stoping timer"<<std::endl;
	waitTimer.stop();
	}
	else {
// 	std::cout<<"going for another round"<<std::endl;
	waitTimer.start(250);
	}
}

bool MlibData::mlibChanged(const unsigned int& id) {
	if(cache.contains(id)) {
		if(changeTimer.isActive())
		changeTimer.stop();
		changeTimer.start(1000);
	getInfoFromServer(id);
	}
// 	std::cout<<"GETING ID FROM SERVER"<<std::endl;
	return true;
}

QStringList MlibData::getStandardTags() {
	return standardTags;
}

QVariant MlibData::getInfo(QString property, uint id) {
	if(cache.contains(id)) {
		if(property=="title" && cache.value(id)->info(property).toString()=="Unknown")
		return cache.value(id)->info("filename");
	return (cache.value(id))->info(property);
	}
	else {
	getInfoFromServer(id);
	return QVariant(-1);
	}
}
	
QVariant MlibData::getInfo(std::string property, uint id) {
	return getInfo(QString(property.c_str()),id);
}

QVariant MlibData::getInfo(char* property, uint id) {
	return getInfo(QString(property),id);
}

bool MlibData::hasInfo(uint id) {
	return cache.contains(id);
}

void MlibData::getInfoFromServer(uint id) {
	if(!waitingIds.contains(id)) {
// 		std::cout<<"enqueueing more songs "<<id<<std::endl;
		waitingIds.enqueue(id);
		if(!waitTimer.isActive())
		waitTimer.start(250);
	}
}

void MlibData::clearCache() {
	cache.clear();
}

bool MlibData::getMediaInfo(const Xmms::PropDict& info) {
// 	(boost::bind (&XClient::propDictToQHash,_1, _2, _3, boost::ref (hash))
	MediaInfo * newInfo = new MediaInfo();
	int id = info.get<int>("id");
	cache.insert(id,newInfo);
	info.each(boost::bind(&MlibData::getAllMediaInfoForId,this,id,_1,_2,_3));
	emit infoChanged(id);
	return true;
}

bool MlibData::getAllMediaInfoForId(int id,std::string key,Xmms::Dict::Variant val,std::string src) {
	QString newKey = QString::fromUtf8(key.c_str());
	QVariant newValue;
		if(val.type() == typeid(int)) {
			if(key=="duration") {
			int tmpInt = boost::get<int>(val);
			QString tmp;
			QTime foo (0,0,0);
			foo=foo.addMSecs(tmpInt);
			if(foo.hour()>0)
				tmp =foo.toString("h:mm:ss");
			else
				tmp =foo.toString("mm:ss");
			newValue = QVariant(tmp);
			}
			else {
			newValue = QVariant(boost::get<int>(val));
			}
		}
		else if(val.type() == typeid(uint32_t)) {
		newValue = QVariant(boost::get<uint32_t>(val));
		}
		else {
			if(key == "url") {
			std::string tmpStr = boost::get<std::string>(val);
			tmpStr = xmmsc_result_decode_url(NULL,tmpStr.c_str());
			newValue = QVariant(tmpStr.c_str());
			
			//xmms2 doesn't keep the filename, but this is usefull for unknown titles
			QString filename(tmpStr.c_str());
			filename = filename.mid(filename.lastIndexOf("/")+1);
			(cache.value(id))->setInfo(QString("filename"),QVariant(filename));
			}
			else {
			newValue = QVariant(QString::fromUtf8(boost::get<std::string>(val).c_str()));
			}
		}
	(cache.value(id))->setInfo(newKey,newValue);
	(cache.value(id))->setSource(newKey,QString::fromUtf8(src.c_str()));
	return true;
}

void MlibData::getListFromServer(Xmms::Coll::Coll* mlib,QString property) {
 	std::list<std::string> what;
 	what.push_back(property.toStdString());
//  	std::cout<<"getting"<<std::endl;
 	conn->collection.queryInfos(*mlib,what)(boost::bind(&MlibData::gotList,this,property.toStdString(),_1));
}

bool MlibData::gotList(std::string property,const Xmms::List <Xmms::Dict> &list) {
	QList<QString> info;
	for (list.first();list.isValid(); ++list) {
		if(list->contains(property))
		info.append(QString::fromUtf8((list->get<std::string>(property)).c_str()));
	}

// 	std::cout<<"BEGIN MLIB TEST"<<std::endl;
// 	std::cout<<property<<std::endl;
	emit gotListFromServer(QString(property.c_str()),info);
// 	std::cout<<"END MLIB TEST"<<std::endl;

	return true;
}

#endif



