#ifndef PLAYLISTPANEL__CPP
#define PLAYLISTPANEL__CPP
#include "newplaylistpanel.h"
Playlist_::Playlist_(DataBackend * c):QTableView()
	{
	conn=c;
	QHeaderView * vh=verticalHeader();
	vh->hide();
	vh->setResizeMode(QHeaderView::Fixed);
	vh->setDefaultSectionSize(20);
	setVerticalHeader(vh);
	QHeaderView * hh=horizontalHeader();
	setHorizontalHeader(hh);
	this->setWordWrap(false);
	this->setDragDropOverwriteMode(false);
	this->setAutoScroll(true);
	this->setAlternatingRowColors(false);
	this->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
	this->setSelectionBehavior(QAbstractItemView::SelectRows);
	this->setSelectionMode(QAbstractItemView::ExtendedSelection);
	this->setDragEnabled(true);
	this->setAcceptDrops(true);
	this->setDropIndicatorShown(true);
	this->setDragDropMode(QAbstractItemView::DragDrop);
	connect(this,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(doubleClicked(const QModelIndex &)));
	}

void Playlist_::setModelAndDelegate(SinglePlaylist * model)
	{
	this->setModel(model);
	this->setItemDelegate(model->getDelegate());
	}

void Playlist_::doubleClicked(const QModelIndex & index)
	{
	conn->playlist.setNext(index.row())(Xmms::bind(&DataBackend::scrapResultI, conn));
	conn->playback.tickle()(Xmms::bind(&DataBackend::scrapResult, conn));
	}

QList <uint> Playlist_::getSortedSelectedRows()
	{
	QList <uint> out;
	QModelIndexList indexes=selectedIndexes();
	foreach (QModelIndex index, indexes)
		{
		if(!out.contains(index.row()))
			out.append(index.row());
		}
	qSort(out);
	return out;
	}

/*************************/

PlaylistPanel_::PlaylistPanel_(DataBackend * c):LayoutPanel()
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
		playlistView=new Playlist_(conn);
		centralLayout->addWidget(playlistView,1,0,1,2);
		playlistSwitcher=new QComboBox();
		playlistSwitcher->addItems(((CollData *)conn->getDataBackendObject(DataBackend::COLL))->getPlaylists());
		playlistSwitcher->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
		connect(playlistSwitcher,SIGNAL(currentIndexChanged(const QString &)),this,SLOT(playlistSelected(QString)));
		centralLayout->addWidget(playlistSwitcher,0,1);
		connect(((CollData *)conn->getDataBackendObject(DataBackend::COLL)),SIGNAL(playlistsChanged(QStringList)),
						this,SLOT(playlistsChanged(QStringList)));
		playlistModeSwitcher=new QPushButton("Playing:");
		connect(playlistModeSwitcher,SIGNAL(clicked()),this,SLOT(playlistModeSwitched()));
		centralLayout->addWidget(playlistModeSwitcher,0,0);

		this->setLayout(centralLayout);

	del = new QShortcut(QKeySequence(Qt::Key_Delete),this,SLOT(deleteSelected()),SLOT(deleteSelected()));
	this->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
	}

void PlaylistPanel_::setLayoutSide(bool right_side)
	{
std::cout<<"ping layoutside: "<<right_side<<std::endl;
	}

void PlaylistPanel_::playlistSelected(QString name)
	{
	if(locked) return;
	currentPlaylistName=name.toStdString();
	currentPlaylist=plistBackend->getPlist(currentPlaylistName);
	if(currentPlaylist!=NULL) playlistView->setModelAndDelegate(currentPlaylist);
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
		((PlaylistDelegate *)playlistView->itemDelegate())->setEditing(editing);
		conn->playlist.load(currentPlaylistName)(Xmms::bind(&DataBackend::scrapResult, conn));
		}
	else
		{
		playlistModeSwitcher->setText("Editing:");
		editing=true;
		((PlaylistDelegate *)playlistView->itemDelegate())->setEditing(editing);
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
	conn->playlist.currentActive() (Xmms::bind(&DataBackend::getCurrentPlaylist,conn));
	}

void PlaylistPanel_::setCurrentName(std::string name)
	{
	if(editing||locked) return;
	editing=true;
	playlistSwitcher->setCurrentIndex(playlistSwitcher->findText(name.c_str()));
	editing=false;
	}

void PlaylistPanel_::playlistReady(std::string name,SinglePlaylist * plist)
	{
	if(name==currentPlaylistName)
		{
		currentPlaylist=plist;
		playlistView->setModelAndDelegate(currentPlaylist);
		}
	}

void PlaylistPanel_::deleteSelected()
	{
	QList <uint> selected=playlistView->getSortedSelectedRows();
	for(int i=selected.size()-1; i>-1; i--)
		conn->playlist.removeEntry(selected[i],currentPlaylistName)(Xmms::bind(&DataBackend::scrapResult, conn));
	}

#endif
