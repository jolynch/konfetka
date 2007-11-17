#ifndef PLISTDATA_CPP
#define PLISTDATA_CPP
#include "plistdata.h"

SinglePlaylist::SinglePlaylist(DataBackend * c,QStringList hv_)
	{
	conn=c; humanReadableHeader=hv_;
	parseHumanReadableHeader();
	connected=false;
	connect(((MlibData *)conn->getDataBackendObject(DataBackend::MLIB)),SIGNAL(infoChanged(int)),this,SLOT(infoChanged(int)));
	}

void SinglePlaylist::parseHumanReadableHeader()
	{
	header.clear();
	for(int i=0; i<humanReadableHeader.size(); i++)
		{
		QString lbl=humanReadableHeader[i].toLower();
		if(lbl=="filename") lbl=QString("url");
		if(lbl=="track") lbl=QString("tracknr");
		if(lbl=="time") lbl=QString("duration");
		header.append(lbl);
		}
	this->reset();
	}

void SinglePlaylist::connectToggle()
	{connected=!connected;}

bool SinglePlaylist::isConnected()
	{return connected;}

void SinglePlaylist::setHeader(QStringList newVal)
	{
	humanReadableHeader=newVal;
	parseHumanReadableHeader();
	}

QModelIndex SinglePlaylist::index ( int row, int column, const QModelIndex & parent) const
	{
	if(row<0||column<0) return QModelIndex();
	if(rowCount()<=0||columnCount()<=0) return QModelIndex();
	if(parent.isValid()) return QModelIndex();
	return createIndex(row,column,ids[row]);
	}

QModelIndex SinglePlaylist::parent ( const QModelIndex & index ) const
	{return QModelIndex();}

QVariant SinglePlaylist::data ( const QModelIndex & index, int role) const
	{
	if(role!=Qt::DisplayRole) return QVariant();
	MlibData * mlib=(MlibData *)(conn->getDataBackendObject(DataBackend::MLIB));
	return mlib->getInfo(header[index.column()],index.internalId());
	}

int SinglePlaylist::rowCount ( const QModelIndex & parent) const
	{
	return ids.size();
	}


//bool SinglePlaylist::hasChildren ( const QModelIndex & parent) const
//	{return false;}

int SinglePlaylist::columnCount ( const QModelIndex & parent) const
	{return header.size();}

bool SinglePlaylist::setInitialPlist(const Xmms::List< unsigned int > &list)
	{
	ids.clear();
	for (list.first (); list.isValid (); ++list)
		{ids.append(*list);}
	this->reset();
	return true;
	}

void SinglePlaylist::infoChanged(int id)
	{
	if(!ids.contains(id)) return;
	for(int i=ids.indexOf(id); i!=-1; i=ids.indexOf(id,i+1))
		dataChanged(index(i,0),index(i,header.size()-1));
	}

/************************************!*********************************/

PlistData::PlistData(DataBackend * c,QObject * parent):QObject(parent)
	{
	conn=c;
	currentPlist="";
	connect(((CollData *)conn->getDataBackendObject(DataBackend::COLL)),SIGNAL(updated(QString,Xmms::Collection::Namespace)),
		this,SLOT(collectionChanged(QString,Xmms::Collection::Namespace)));
	connect(conn,SIGNAL(playlistNameChanged(const std::string&)),this,SLOT(setCurrentName(const std::string &)));
	connect(conn,SIGNAL(qsettingsValueChanged(QString,QVariant)),this,SLOT(qsettingsValChanged(QString,QVariant)));
	conn->playlist.currentActive() (Xmms::bind(&DataBackend::getCurrentPlaylist,conn));
	}

void PlistData::createPlaylist(std::string name)
	{
std::cout<<"creating playlist: "<<name<<std::endl;
	if(plists.contains(name.c_str())) return;
	SinglePlaylist * newPlist=new SinglePlaylist(conn,headerVals);
	plists.insert(name.c_str(), newPlist);
	refreshPlaylist(newPlist,name);
	}

void PlistData::refreshPlaylist(SinglePlaylist * plist, std::string name)
	{
	if(plist==NULL||plist->isConnected()) return;
	conn->playlist.listEntries(name) (Xmms::bind (&SinglePlaylist::setInitialPlist, plist));
	if(name!="")
		emit playlistReady(name,plist);
	}

void PlistData::connectToServer(SinglePlaylist * plist)
	{
	plist->connectToggle();
	}

void PlistData::disconnectFromServer(SinglePlaylist * plist)
	{
	plist->connectToggle();
	}


SinglePlaylist * PlistData::getPlist (std::string plist)
	{
	if(plist=="")
		if(currentPlist=="")
			return NULL;
		else
			plist=currentPlist;
	if(plists.contains(plist.c_str()))
		return plists[plist.c_str()];
	else
		{
		createPlaylist(plist);
		return plists[plist.c_str()];
		}
	return NULL;
	}


QString PlistData::getCurrentName()
	{return currentPlist.c_str();}


void PlistData::setCurrentName(const std::string & name)
	{
std::cout<<"current playlist: "<<name<<std::endl;
	refreshPlaylist(getPlist(name));
	connectToServer(getPlist(name));
	currentPlist=name;
	}


void PlistData::collectionChanged(QString name, Xmms::Collection::Namespace ns)
	{
	if(ns!=Xmms::Collection::PLAYLISTS)
		return;
	if(plists.contains(name))
		refreshPlaylist(plists[name],name.toStdString());
	}

void PlistData::qsettingsValChanged(QString name,QVariant newVal)
	{
	if(name=="konfetka/playlistValues")
		{
		QStringList val=newVal.toStringList();
		headerVals=val;
		QStringList keys=plists.keys();
		for(int i=0; i<keys.size(); i++)
			plists[keys[i]]->setHeader(val);
		}
	}
#endif

