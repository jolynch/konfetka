#ifndef DATABACKEND_CPP
#define DATABACKEND_CPP
#include "databackend.h"


#include "databackend/plistdata.h"
#include "databackend/mlibdata.h"
#include "databackend/colldata.h"


DataBackend::DataBackend(QObject * parent, std::string name):XMMS2Interface(parent,name) {
	mlibData=new MlibData(this);
	collData=new CollData(this);
	plistData=new PlistData(this);

	QSettings s;
	random=false;
	if(!s.contains("konfetka/randomPlay")) s.setValue("konfetka/randomPlay",false);

	currId=0;
	QObject::connect(((XMMS2Interface *)this),SIGNAL(currentId(int)),this,SLOT(currentIdChanged(int)));
	QObject::connect(((MlibData *)mlibData),SIGNAL(infoChanged(int)),this,SLOT(infoChanged(int)));
	QObject::connect(this,SIGNAL(qsettingsValueChanged(QString,QVariant)),this,SLOT(getRandom(QString,QVariant)));
}

QObject * DataBackend::getDataBackendObject(DataBackendType type) {
	switch(type)
		{
		case COLL:return collData;
		case PLIST:return plistData;
		case MLIB:return mlibData;
		}
	return NULL;
}

void DataBackend::emitInitialQSettings() {
	QSettings s;
	QStringList l=s.allKeys();
	for(int i=0; i<l.size(); i++)
		if(l[i].contains("konfetka"))
			emit qsettingsValueChanged(l[i],s.value(l[i]));
}

void DataBackend::changeAndSaveQSettings(QString name, QVariant newValue) {
	QSettings s;
	s.setValue(name,newValue);
	emit qsettingsValueChanged(name, newValue);
}

void DataBackend::currentIdChanged(int newId) {
	if(newId!=currId)
		currId=newId;
}

void DataBackend::infoChanged(int id) {
	if(id==currId)
		this->playback.currentID()(Xmms::bind(&XMMS2Interface::newSongResponse, this));
}

void DataBackend::playNextSong()
	{
	if(!random)
		{
		this->playlist.setNextRel(1)(Xmms::bind(&XMMS2Interface::scrapResultI, this));
		this->playback.tickle()(Xmms::bind(&XMMS2Interface::scrapResult, this));
		return;
		}
	}

void DataBackend::playPreviousSong()
	{
	if(!random)
		{
		this->playlist.setNextRel(-1)(Xmms::bind(&XMMS2Interface::scrapResultI, this));
		this->playback.tickle()(Xmms::bind(&XMMS2Interface::scrapResult, this));
		return;
		}
	}

void DataBackend::getRandom(QString name, QVariant newValue)
	{if(name=="konfetka/randomPlay")
		random=newValue.toBool();}
#endif
