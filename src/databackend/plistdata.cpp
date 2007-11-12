#ifndef PLISTDATA_CPP
#define PLISTATA_CPP
#include "plistdata.h"

SinglePlaylist::SinglePlaylist(DataBackend * c)
	{
	connected=false;
	}

void SinglePlaylist::connectToggle()
	{connected=!connected;}

bool SinglePlaylist::isConnected()
	{return connected;}

PlistData::PlistData(DataBackend * c,QObject * parent):QObject(parent)
	{
	conn=c;
	currentPlist="";
	connect(((CollData *)conn->getDataBackendObject(DataBackend::COLL)),SIGNAL(updated(QString,Xmms::Collection::Namespace)),
		this,SLOT(collectionChanged(QString,Xmms::Collection::Namespace)));
	connect(conn,SIGNAL(playlistNameChanged(const std::string&)),this,SLOT(setCurrentName(const std::string &)));
	conn->playlist.currentActive() (Xmms::bind(&DataBackend::getCurrentPlaylist,conn));
	}

void PlistData::createPlaylist(std::string name)
	{
std::cout<<"creating playlist: "<<name<<std::endl;
	if(plists.contains(name.c_str())) return;
	SinglePlaylist * newPlist=new SinglePlaylist(conn);
	plists.insert(name.c_str(), newPlist);
	refreshPlaylist(newPlist,name);
	}

void PlistData::refreshPlaylist(SinglePlaylist * plist, std::string name)
	{
	if(plist==NULL||plist->isConnected()) return;
	//Refresh
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
		refreshPlaylist(plists[name]);
	}
#endif

