#ifndef PLAYLISTPANEL__CPP
#define PLAYLISTPANEL__CPP
#include "newplaylistpanel.h"

PlaylistPanel_::PlaylistPanel_(DataBackend * c):QWidget()
	{
	conn=c;
	layout=new QGridLayout();
	playlistView=new QTreeView();
	playlistView->setAlternatingRowColors(true);
	playlistView->setItemsExpandable(false);
	currentPlaylistName="";
	plistBackend=((PlistData *)conn->getDataBackendObject(DataBackend::PLIST));
	layout->addWidget(playlistView,0,0);
	this->setLayout(layout);
	connect(conn,SIGNAL(playlistNameChanged(const std::string&)),this,SLOT(setCurrentName(const std::string &)));
	connect(plistBackend,SIGNAL(playlistReady(std::string,SinglePlaylist *)),this,SLOT(playlistReady(std::string,SinglePlaylist *)));
	}

void PlaylistPanel_::setLayoutSide(bool right_side)
	{
	
	}

void PlaylistPanel_::setCurrentName(std::string name)
	{
	currentPlaylistName=name.c_str();
	currentPlaylist=plistBackend->getPlist(name);
	if(currentPlaylist!=NULL) playlistView->setModel(currentPlaylist);
	}

void PlaylistPanel_::playlistReady(std::string name,SinglePlaylist * plist)
	{
	if(name==currentPlaylistName)
		{
		currentPlaylist=plist;
		playlistView->setModel(currentPlaylist);
		}
	}

#endif
