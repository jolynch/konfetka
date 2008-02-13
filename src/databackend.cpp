#ifndef DATABACKEND_CPP
#define DATABACKEND_CPP
#include "databackend.h"


#include "databackend/plistdata.h"
#include "databackend/mlibdata.h"
#include "databackend/colldata.h"
#include "databackend/panelcontroller_.h"


DataBackend::DataBackend(QObject * parent, std::string name):XMMS2Interface(parent,name) {
	mlibData=new MlibData(this);
	collData=new CollData(this);
	plistData=new PlistData(this);
	//panelController=new PanelController_(this);

	QSettings s;
	random=false;
	if(!s.contains("konfetka/randomPlay")) s.setValue("konfetka/randomPlay",false);

	currId=0; curr=0;
	QObject::connect(((XMMS2Interface *)this),SIGNAL(currentId(int)),this,SLOT(currentIdChanged(int)));
	QObject::connect(((XMMS2Interface *)this),SIGNAL(currentPos(uint)),this,SLOT(currentPosChanged(uint)));
	QObject::connect(((MlibData *)mlibData),SIGNAL(infoChanged(int)),this,SLOT(infoChanged(int)));
	QObject::connect(this,SIGNAL(qsettingsValueChanged(QString,QVariant)),this,SLOT(getRandom(QString,QVariant)));
}

QObject * DataBackend::getDataBackendObject(DataBackendType type) {
	switch(type)
		{
		case COLL:return collData;
		case PLIST:return plistData;
		case MLIB:return mlibData;
		case PANEL:return panelController;
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

void DataBackend::currentPosChanged(uint newPos){
	if(newPos!=curr)
		{
		curr=newPos;
		}
	}

void DataBackend::currentIdChanged(int newId) {
	if(newId!=currId)
		{
		currId=newId;
		}
}

void DataBackend::infoChanged(int id) {
	if(id==currId)
		this->playback.currentID()(Xmms::bind(&XMMS2Interface::newSongResponse, this));
}


void DataBackend::fillUpcoming()
	{
	upcoming.clear();
	srand(time(NULL));
	for(int i=0; i<UPCOMING; i++)
		addSongIdToQueue();
	}

void DataBackend::addSongIdToQueue()
	{
	if(((PlistData *)plistData)->getPlist()==NULL) return;
	int l=((PlistData *)plistData)->getPlist()->rowCount();
	if(l==0)
		{
		if(upcoming.isEmpty())
			upcoming.enqueue(0);
		return;
		}
	if(l<UPCOMING)
		{
		upcoming.enqueue(rand()%l);
		return;
		}
	int n=rand()%l;
	while(upcoming.contains(n))
		{
		n=rand()%l;
		}
	upcoming.enqueue(n);
	}

void DataBackend::clearRandomData()
	{
	history.clear();
	fillUpcoming();
	}

void DataBackend::checkValidity(QQueue <uint> v)
	{
	int n=v.head();
	int l=((PlistData *)plistData)->getPlist()->rowCount();
	if(l==0) return;
	while(n<0||n>=l)
		{
		addSongIdToQueue();
		v.dequeue();
		n=v.head();
		}
	}

void DataBackend::checkValidity(QStack <uint>  v)
	{
	if(v.isEmpty()) return;
	int n=v.top();
	int l=((PlistData *)plistData)->getPlist()->rowCount();
	if(l==0) return;
	while(n<0||n>=l)
		{
		v.pop();
		if(!v.isEmpty()) n=v.top();
		else break;
		}
	}

void DataBackend::playNextSong()
	{
	if(!random)
		{
		this->playlist.setNextRel(1)(Xmms::bind(&XMMS2Interface::scrapResultI, this));
		this->playback.tickle()(Xmms::bind(&XMMS2Interface::scrapResult, this));
		return;
		}
	else
		{
		history.push(curr);
		if(upcoming.isEmpty()) fillUpcoming();
		checkValidity(upcoming);
		this->playlist.setNext(upcoming.head())(Xmms::bind(&XMMS2Interface::scrapResultI, this));
		this->playback.tickle()(Xmms::bind(&XMMS2Interface::scrapResult, this));
		addSongIdToQueue();
		upcoming.dequeue();
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
	else
		{
		checkValidity(history);
		if(!history.isEmpty())
			{
			this->playlist.setNext(history.top())(Xmms::bind(&XMMS2Interface::scrapResultI, this));
			history.pop();
			}
		else
			{
			if(curr>0)
				this->playlist.setNext(curr-1)(Xmms::bind(&XMMS2Interface::scrapResultI, this));
			else
				this->playlist.setNext(curr)(Xmms::bind(&XMMS2Interface::scrapResultI, this));
			}
		this->playback.tickle()(Xmms::bind(&XMMS2Interface::scrapResult, this));
		return;
		}
	}

void DataBackend::playlistChanged(QString name)
	{
	clearRandomData();
	}

void DataBackend::getRandom(QString name, QVariant newValue)
	{
	if(name=="konfetka/randomPlay")
		{
		random=newValue.toBool();
		clearRandomData();
		}
	}
#endif
