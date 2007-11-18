#ifndef MLIBDATA_CPP
#define MLIBDATA_CPP
#include "mlibdata.h"

MediaInfo::MediaInfo() {
}

MediaInfo::MediaInfo(QHash<QString,QVariant> hash) {
	songInfo = hash;
}

QVariant MediaInfo::info(QString val) {
	if(songInfo.contains(val)) {
	return songInfo.value(val);
	}
	else
	return QVariant("Unknown");
}

void MediaInfo::setInfo(QString property,QVariant value) {
	songInfo.insert(property,value);
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

MlibData::MlibData(DataBackend * conn,QObject * parent):QObject(parent) {
	this->conn = conn;
	standardTags<<"artist"<<"album"<<"url"<<"encodedurl"<< "title"<<"genre"<<"duration"<<"timesplayed"<<"rating"<<"laststarted"<<"id";
	QSettings s;
	if(s.contains("standardTags"))
	standardTags = s.value("standardTags").toStringList();
	s.setValue("standardTags",standardTags);
	conn->medialib.broadcastEntryChanged()(Xmms::bind(&MlibData::mlibChanged, this));
	connect(&changeTimer,SIGNAL(timeout()),this,SIGNAL(updatesDone()));
	connect(&changeTimer,SIGNAL(timeout()),&changeTimer,SLOT(stop()));
}

bool MlibData::mlibChanged(const unsigned int& id) {
		if(changeTimer.isActive())
		changeTimer.stop();
		changeTimer.start(1000);
	getInfoFromServer(id);
// 	std::cout<<"GETING ID FROM SERVER"<<std::endl;
	return true;
}

QStringList MlibData::getStandardTags() {
	return standardTags;
}

QVariant MlibData::getInfo(QString property, uint id) {
	if(cache.contains(id))
	return (cache.value(id))->info(property);
	else {
	getInfoFromServer(id);
	return QVariant(-1);
	}
}
	
QVariant MlibData::getInfo(std::string property, uint id) {
	return getInfo(QString(property.c_str()),id);
}


void MlibData::getInfoFromServer(uint id) {
	conn->medialib.getInfo(id)(Xmms::bind(&MlibData::getMediaInfo,this));
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



