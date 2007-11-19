#ifndef PLAYLISTPANEL__CPP
#define PLAYLISTPANEL__CPP
#include "newplaylistpanel.h"

PlaylistPanel_::PlaylistPanel_(DataBackend * c):QWidget()
	{
	editing=false;
	locked=false;
	conn=c;

	centralLayout=new QGridLayout();
		centralLayout->setMargin(0);
		currentPlaylistName="";
		plistBackend=((PlistData *)conn->getDataBackendObject(DataBackend::PLIST));
		connect(conn,SIGNAL(playlistNameChanged(const std::string&)),this,SLOT(setCurrentName(const std::string &)));
		connect(plistBackend,SIGNAL(playlistReady(std::string,SinglePlaylist *)),
						this,SLOT(playlistReady(std::string,SinglePlaylist *)));
	
		playlistView=new QTreeView();
		playlistView->setAlternatingRowColors(true);
		playlistView->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
		connect(playlistView,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(doubleClicked(const QModelIndex &)));
		centralLayout->addWidget(playlistView,1,0,1,2);
		playlistSwitcher=new QComboBox();
		playlistSwitcher->addItems(((CollData *)conn->getDataBackendObject(DataBackend::COLL))->getPlaylists());
		playlistSwitcher->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
		centralLayout->addWidget(playlistSwitcher,0,1);
		conn->playlist.currentActive() (Xmms::bind(&DataBackend::getCurrentPlaylist,conn));
		connect(((CollData *)conn->getDataBackendObject(DataBackend::COLL)),SIGNAL(playlistsChanged(QStringList)),
						this,SLOT(playlistsChanged(QStringList)));
		playlistModeSwitcher=new QPushButton("Playing:");
		connect(playlistModeSwitcher,SIGNAL(clicked()),this,SLOT(playlistModeSwitched()));
		centralLayout->addWidget(playlistModeSwitcher,0,0);

		this->setLayout(centralLayout);

	this->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
	}

void PlaylistPanel_::setLayoutSide(bool right_side)
	{
	
	}

void PlaylistPanel_::doubleClicked(const QModelIndex & index)
	{
	conn->playlist.setNext(index.row())(Xmms::bind(&DataBackend::scrapResultI, conn));
	conn->playback.tickle()(Xmms::bind(&DataBackend::scrapResult, conn));
	}

void PlaylistPanel_::playlistSelected(QString name)
	{
	if(locked) return;
	currentPlaylistName=name.toStdString();
	currentPlaylist=plistBackend->getPlist(currentPlaylistName);
	if(currentPlaylist!=NULL) playlistView->setModel(currentPlaylist);
	if(!editing)
		{
		locked=true;
		conn->playlist.load(currentPlaylistName)(Xmms::bind(&DataBackend::scrapResult, conn));
		locked=false;
		}
	}

void PlaylistPanel_::playlistModeSwitched()
	{
	if(editing)
		{
		playlistModeSwitcher->setText("Playing:");
		editing=false;
		}
	else
		{
		playlistModeSwitcher->setText("Editing:");
		editing=true;
		}
	}

void PlaylistPanel_::playlistsChanged(QStringList newList)
	{
	locked=true;
	playlistSwitcher->clear();
	playlistSwitcher->addItems(newList);
	locked=false;
	int tmp=playlistSwitcher->findText(currentPlaylistName.c_str());
	if(tmp==-1) tmp=0;
	playlistSwitcher->setCurrentIndex(tmp);
	}

void PlaylistPanel_::setCurrentName(std::string name)
	{
	if(editing||locked) return;
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
