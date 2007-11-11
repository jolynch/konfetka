#ifndef MLIBDATA_CPP
#define MLIBDATA_CPP
#include "mlibdata.h"

void MediaItem::setInfo(QHash<QString,QVariant> hash) {
songInfo = hash;
}

QVariant MediaItem::info(QString val) {
	if(songInfo.contains(val))
	return songInfo.value(val);
	else
	return QVariant(-1);
}

QHash<QString,QVariant> MediaItem::allInfo() {
return songInfo;
}

bool MediaItem::matches(QString what,QString term) {
	
	if(what=="all") {
	QHash<QString, QVariant>::const_iterator i;
	for(i=songInfo.constBegin(); i != songInfo.constEnd(); ++i) {
		if(i.value().toString().contains(term,Qt::CaseInsensitive)) {
		return true;
		}
	}
	}
	else {
		if(songInfo.value(what).toString().contains(term,Qt::CaseInsensitive))
		return true;
	}
return false;
}



MlibData::MlibData(DataBackend * conn,QObject * parent):QObject(parent) {
	this->conn = conn;
	lookUps<<"artist"<<"album"<<"url"<<"encodedurl"<< "title"<<"genre"<<"duration"<<"timesplayed"<<"rating"<<"lastplayed"<<"id";
	conn->medialib.broadcastEntryChanged()(Xmms::bind(&MlibData::mlibChanged, this));
	connect(&changeTimer,SIGNAL(timeout()),this,SIGNAL(updatesDone()));
	connect(&changeTimer,SIGNAL(timeout()),&changeTimer,SLOT(stop()));

}

bool MlibData::mlibChanged(const unsigned int& id) {
		if(changeTimer.isActive())
		changeTimer.stop();
		changeTimer.start(1000);
	getItemFromServer(id);
// 	std::cout<<"GETING ID FROM SERVER"<<std::endl;
	return true;
}


QVariant MlibData::getInfo(QString property, uint id) {
	if(cache.contains(id))
	return (cache.value(id))->info(property);
	else {
	getInfoFromServer(property,id);
	return QVariant(-1);
	}
}
	
QVariant MlibData::getInfo(std::string property, uint id) {
	return getInfo(QString(property.c_str()),id);
}

void MlibData::getInfoFromServer(QString property, uint id) {
}

MediaItem* MlibData::getItem(uint id) {
	if(cache.contains(id)) {
	return cache.value(id);
	}
	else {
	getItemFromServer(id);
	return NULL;
	}
}

MediaItem* MlibData::operator[] (uint id) {
	return getItem(id);
}

void MlibData::clearCache() {
	cache.clear();
}

void MlibData::getItemFromServer(uint id) {
	conn->medialib.getInfo(id)(Xmms::bind(&MlibData::getMediaInfo,this));
}

bool MlibData::getMediaInfo(const Xmms::PropDict& info) {
	MediaItem * newItem = new MediaItem();
	QHash<QString,QVariant> curInfo;
	int tmpInt;
	std::string tmpString;
	
	for(int i=0;i<lookUps.size();i++) {
		if(info.contains(lookUps.value(i).toStdString()) || lookUps.value(i)=="encodedurl") {
			try {
				if(lookUps.value(i)=="encodedurl")
				tmpString = info.get<std::string>("url");
				else
				tmpString = info.get<std::string>(lookUps.value(i).toStdString());

				if(lookUps.value(i)=="url")
				tmpString = xmmsc_result_decode_url (NULL,tmpString.c_str());


				curInfo.insert(lookUps.value(i),QVariant(QString::fromUtf8(tmpString.c_str())));
			} catch(Xmms::wrong_type_error& err) {
			tmpInt = info.get<int>(lookUps.value(i).toStdString());
				if(lookUps.value(i)=="duration") {
				QString tmp;
				QTime foo (0,0,0);
				foo=foo.addMSecs(tmpInt);
				if(foo.hour()>0)
					tmp =foo.toString("h:mm:ss");
				else
					tmp =foo.toString("mm:ss");
				curInfo.insert(lookUps.value(i),QVariant(tmp));
				}
				else {
				curInfo.insert(lookUps.value(i),QVariant(tmpInt));
				}
			}
		} else {
			if(lookUps.value(i) == "title")
			curInfo.insert(lookUps.value(i),curInfo.value("url"));
		curInfo.insert(lookUps.value(i),QVariant("Unknown"));
		}
	}
	newItem->setInfo(curInfo);
	newItem->setText(0,(newItem->info("title")).toString());
		if(info.get<int>("status") == 3) {
		newItem->setForeground(0,QBrush(QColor(119,119,119,255),Qt::SolidPattern));
		}
	cache.insert(info.get<int>("id"),newItem);
	emit infoChanged(info.get<int>("id"));
// 	std::cout<<"got info"<<std::endl;
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



