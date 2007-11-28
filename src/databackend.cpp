#ifndef DATABACKEND_CPP
#define DATABACKEND_CPP
#include "databackend.h"


#include "databackend/plistdata.h"
#include "databackend/mlibdata.h"
#include "databackend/colldata.h"


DataBackend::DataBackend(QObject * parent, std::string name):XMMS2Interface(parent,name)//,QObject(parent)
	{
	mlibData=new MlibData(this);
	collData=new CollData(this);
	plistData=new PlistData(this);

	currId=0;
	QObject::connect(((XMMS2Interface *)this),SIGNAL(currentId(int)),this,SLOT(currentIdChanged(int)));
	QObject::connect(((MlibData *)mlibData),SIGNAL(infoChanged(int)),this,SLOT(infoChanged(int)));
	}

QObject * DataBackend::getDataBackendObject(DataBackendType type)
	{
	switch(type)
		{
		case COLL:return collData;
		case PLIST:return plistData;
		case MLIB:return mlibData;
		}
	return NULL;
	}

void DataBackend::emitInitialQSettings()
	{
	QSettings s;
	QStringList l=s.allKeys();
	for(int i=0; i<l.size(); i++)
		if(l[i].contains("konfetka"))
			emit qsettingsValueChanged(l[i],s.value(l[i]));
	}

void DataBackend::changeAndSaveQSettings(QString name, QVariant newValue)
	{
	QSettings s;
	s.setValue(name,newValue);
	emit qsettingsValueChanged(name, newValue);
	}

void DataBackend::currentIdChanged(int newId)
	{
	if(newId!=currId)
		currId=newId;
	}

void DataBackend::infoChanged(int id)
	{
	if(id==currId)
		this->playback.currentID()(Xmms::bind(&XMMS2Interface::newSongResponse, this));
	}

#endif
