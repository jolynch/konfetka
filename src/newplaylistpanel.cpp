#ifndef PLAYLISTPANEL__CPP
#define PLAYLISTPANEL__CPP
#include "newplaylistpanel.h"
Playlist_::Playlist_(DataBackend * c,QWidget * p):QTableView(p)
	{
	conn=c;
	parent=p;
	QHeaderView * vh=verticalHeader();
	vh->hide();
	vh->setResizeMode(QHeaderView::Fixed);
	vh->setDefaultSectionSize(20);
	setVerticalHeader(vh);
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
	this->setSortingEnabled(true);
	QHeaderView * hh=horizontalHeader();
	hh->setHighlightSections(false);
	hh->setMovable(false);
	hh->setSortIndicatorShown(false);
	setHorizontalHeader(hh);
	rightClickMenu=new QMenu();
	rightClickMenu->addAction("Remove selected items",((PlaylistPanel_ *)(parent)),SLOT(deleteSelected()));
	rightClickMenu->addAction("Crop selected",((PlaylistPanel_ *)(parent)),SLOT(cropSelected()));
	rightClickMenu->addAction("Clear playlist",((PlaylistPanel_ *)(parent)),SLOT(clear()));
	rightClickMenu->addSeparator();
	QMenu * sortMenu=new QMenu("Sort");
	sortMenu->addAction("By filename",((PlaylistPanel_ *)(parent)),SLOT(sortByUrl()));
	sortMenu->addAction("By aritst, album, title",((PlaylistPanel_ *)(parent)),SLOT(sortByCommonProperties()));
	rightClickMenu->addMenu(sortMenu);
	rightClickMenu->addAction("Shuffle",((PlaylistPanel_ *)(parent)),SLOT(shuffle()));
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

void Playlist_::dropEvent(QDropEvent *event)
	{
	QModelIndex idx=indexAt(event->pos());
	if(!idx.isValid())
		{
		QPoint tmp=event->pos();
		tmp.setY(tmp.y()+5);
		idx=indexAt(tmp);
		}
	if(event->source()==this)
		{
		if(event->proposedAction()==Qt::MoveAction)
			model()->dropMimeData(event->mimeData(),Qt::CopyAction,-1,-1,idx);
		else if(event->proposedAction()==Qt::CopyAction)
			model()->dropMimeData(event->mimeData(),Qt::MoveAction,-1,-1,idx);
		}
	else
		model()->dropMimeData(event->mimeData(),event->proposedAction(),-1,-1,idx);
	}

void Playlist_::keyPressEvent(QKeyEvent* event)
	{
	if(event->key() == (Qt::Key_Delete)||(event->key()==(Qt::Key_Backspace)))
	((PlaylistPanel_ *)(parent))->deleteSelected();
	}

void Playlist_::contextMenuEvent ( QContextMenuEvent * event )
	{
	rightClickMenu->exec(event->pos());
	}


/*************************/

PlaylistPanel_::PlaylistPanel_(DataBackend * c):LayoutPanel()
	{
	editing=false;
	locked=false;
	repeat_one=false;
	repeat_all=false;
	conn=c;

	centralLayout=new QGridLayout();
		centralLayout->setMargin(0);
		currentPlaylistName="";
		plistBackend=((PlistData *)conn->getDataBackendObject(DataBackend::PLIST));
		connect(conn,SIGNAL(playlistNameChanged(const std::string&)),this,SLOT(setCurrentName(const std::string &)));
		connect(plistBackend,SIGNAL(playlistReady(std::string,SinglePlaylist *)),
						this,SLOT(playlistReady(std::string,SinglePlaylist *)));
		playlistView=new Playlist_(conn,this);
		centralLayout->addWidget(playlistView,1,0,1,2);
		playlistSwitcher=new QComboBox();
		playlistSwitcher->addItems(((CollData *)conn->getDataBackendObject(DataBackend::COLL))->getPlaylists());
		playlistSwitcher->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
		centralLayout->addWidget(playlistSwitcher,0,1);
		connect(((CollData *)conn->getDataBackendObject(DataBackend::COLL)),SIGNAL(playlistsChanged(QStringList)),
						this,SLOT(playlistsChanged(QStringList)));
		connect(playlistSwitcher,SIGNAL(currentIndexChanged(const QString &)),this,SLOT(playlistSelected(QString)));
		playlistModeSwitcher=new QPushButton("Playing:");
		connect(playlistModeSwitcher,SIGNAL(clicked()),this,SLOT(playlistModeSwitched()));
		centralLayout->addWidget(playlistModeSwitcher,0,0);
	
	sidebarLayout=new QGridLayout();
		sidebarLayout->setMargin(0);
		QMenu * addMenu=new QMenu();
		QMenu * newMenu=new QMenu("New");
		newMenu->addAction("Regular playlist",this,SLOT(addRegPlist()));
		newMenu->addAction("Queue",this,SLOT(addQueue()));
		newMenu->addAction("Party shuffle",this,SLOT(addPshuffle()));
		addMenu->addMenu(newMenu);
		addMenu->addSeparator();
		addMenu->addAction("Add from filesystem",this,SLOT(addFromFilesystem()));
		addMenu->addAction("Add from medlia library",this,SLOT(addFromMedialib()));
		addMenu->addAction("Add from collection",this,SLOT(addFromCollections()));
		plusButton=new QPushButton();
		plusButton->setIcon(QIcon(":images/plus.png"));
		plusButton->setMenu(addMenu);
		plusButton->setToolTip("Add to playlist or make new playlist");
		sidebarLayout->addWidget(plusButton,0,0);

		QMenu * removeMenu=new QMenu();
		removeMenu->addAction("Remove selected items",this,SLOT(deleteSelected()));
		removeMenu->addAction("Crop selected",this,SLOT(cropSelected()));
		removeMenu->addAction("Clear playlist",this,SLOT(clear()));
		removeMenu->addSeparator();
		removeMenu->addAction("Delete playlist",this,SLOT(removePlist()));
		minusButton=new QPushButton();
		minusButton->setIcon(QIcon(":images/minus.png"));
		minusButton->setMenu(removeMenu);
		minusButton->setToolTip("Remove from playlist or remove playlist");
		sidebarLayout->addWidget(minusButton,1,0);

		saveButton=new QPushButton();
		saveButton->setToolTip("Save copy as");
		saveButton->setIcon(QIcon(":images/save.png"));
		connect(saveButton,SIGNAL(clicked()),this,SLOT(savePlist()));
		sidebarLayout->addWidget(saveButton,2,0);

		openButton=new QPushButton();
		openButton->setToolTip("Open playlist from disk");
		openButton->setIcon(QIcon(":images/open.png"));
		connect(openButton,SIGNAL(clicked()),this,SLOT(openPlist()));
		sidebarLayout->addWidget(openButton,3,0);

		repeatStateButton=new QPushButton();
		repeatStateButton->setToolTip("Change repetition type");
		setRepIcon();
		connect(repeatStateButton,SIGNAL(clicked()),this,SLOT(changeRepeatState()));
		conn->config.valueGet("playlist.repeat_one")(Xmms::bind(&PlaylistPanel_::setRepOneVal,this));
		conn->config.valueGet("playlist.repeat_all")(Xmms::bind(&PlaylistPanel_::setRepAllVal,this));
		sidebarLayout->addWidget(repeatStateButton,4,0);

		randomPlayButton=new QPushButton();
		randomPlayButton->setToolTip("Toogle random play");
		randomPlayButton->setIcon(QIcon(":images/shuffle.png"));
		randomPlayButton->setCheckable(true);
		connect(randomPlayButton,SIGNAL(clicked()),this,SLOT(toggleRandomPlay()));
		sidebarLayout->addWidget(randomPlayButton,5,0);

	connect(conn,SIGNAL(qsettingsValueChanged(QString,QVariant)),this,SLOT(qsettingValChanged(QString,QVariant)));
	connect(conn,SIGNAL(configValChanged(const Xmms::Dict&)),this,SLOT(xmms2settingChanged(const Xmms::Dict&)));
	this->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
	}

void PlaylistPanel_::setLayoutSide(bool right_side)
	{
	if(right_side)
		{
		rightLayout=new QGridLayout();
		rightLayout->addLayout(centralLayout,0,0,2,1);
		rightLayout->addLayout(sidebarLayout,0,1);
		setLayout(rightLayout);
		}
	else
		{
		leftLayout=new QGridLayout();
		leftLayout->addLayout(sidebarLayout,0,0);
		leftLayout->addLayout(centralLayout,0,1,2,1);
		setLayout(leftLayout);
		}
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

void PlaylistPanel_::qsettingValChanged(QString name,QVariant val)
	{
	if(name=="konfetka/randomPlay")
		randomPlayButton->setChecked(val.toBool());
	}

void PlaylistPanel_::xmms2settingChanged(const Xmms::Dict& change)
	{
	if(change.contains("playlist.repeat_one"))
		setRepOneVal(change.get<std::string>("playlist.repeat_one"));
	if(change.contains("playlist.repeat_all"))
		setRepAllVal(change.get<std::string>("playlist.repeat_all"));
	}

void PlaylistPanel_::addRegPlist()
	{
	Xmms::Coll::Idlist idlist;
	saveCollAs(idlist);
	}

void PlaylistPanel_::addPshuffle()
	{
	Xmms::Coll::PartyShuffle pshuffle(0,10);
	Xmms::Coll::Universe  univ;
	pshuffle.setOperand(univ);
	saveCollAs(pshuffle);
	}

void PlaylistPanel_::addQueue()
	{
	Xmms::Coll::Queue queue(10);
	saveCollAs(queue);
	}

void PlaylistPanel_::addFromFilesystem()
	{
	}

void PlaylistPanel_::addFromMedialib()
	{
	}

void PlaylistPanel_::addFromCollections()
	{
	}

void PlaylistPanel_::deleteSelected()
	{
	QList <uint> selected=playlistView->getSortedSelectedRows();
	for(int i=selected.size()-1; i>-1; i--)
		conn->playlist.removeEntry(selected[i],currentPlaylistName)(Xmms::bind(&DataBackend::scrapResult, conn));
	}

void PlaylistPanel_::cropSelected()
	{
	QList <uint> selected=playlistView->getSortedSelectedRows();
	int size=currentPlaylist->rowCount();
	int offset=selected.size()-1;
	for(int i=size-1; i>=0; i--)
		{
		if(i==selected[offset])
			offset--;
		else
			conn->playlist.removeEntry(i,currentPlaylistName)(Xmms::bind(&DataBackend::scrapResult, conn));
		}
	}

void PlaylistPanel_::clear()
	{conn->playlist.clear(currentPlaylistName)(Xmms::bind(&DataBackend::scrapResult, conn));}

void PlaylistPanel_::removePlist()
	{
	if(playlistSwitcher->count()<=1) return;
	std::string toRemove=currentPlaylistName;
	if(playlistSwitcher->currentIndex()==0) playlistSwitcher->setCurrentIndex(1);
	else playlistSwitcher->setCurrentIndex(0);
	conn->collection.remove(toRemove,Xmms::Collection::PLAYLISTS)(Xmms::bind(&DataBackend::scrapResult, conn));
	}

void PlaylistPanel_::savePlist()
	{
	conn->collection.get(currentPlaylistName,Xmms::Collection::PLAYLISTS)(Xmms::bind(&PlaylistPanel_::saveCollAs,this));
	}

void PlaylistPanel_::openPlist()
	{
	}

void PlaylistPanel_::changeRepeatState()
	{
	if(repeat_one)
		{
		conn->config.valueSet("playlist.repeat_one","0")(Xmms::bind(&XMMS2Interface::scrapResult, conn));
		conn->config.valueSet("playlist.repeat_all","1")(Xmms::bind(&XMMS2Interface::scrapResult, conn));
		return;
		}
	if(repeat_all)
		{
		conn->config.valueSet("playlist.repeat_one","0")(Xmms::bind(&XMMS2Interface::scrapResult, conn));
		conn->config.valueSet("playlist.repeat_all","0")(Xmms::bind(&XMMS2Interface::scrapResult, conn));
		return;
		}
	conn->config.valueSet("playlist.repeat_one","1")(Xmms::bind(&XMMS2Interface::scrapResult, conn));
	conn->config.valueSet("playlist.repeat_all","0")(Xmms::bind(&XMMS2Interface::scrapResult, conn));
	return;
	}

void PlaylistPanel_::toggleRandomPlay()
	{
	QSettings s;
	conn->changeAndSaveQSettings("konfetka/randomPlay",!(s.value("konfetka/randomPlay").toBool()));
	}

void PlaylistPanel_::sortByUrl()
	{
	std::list<std::string> prop;
	prop.push_back("url");
	conn->playlist.sort(prop,currentPlaylistName)(Xmms::bind(&DataBackend::scrapResult, conn));
	}

void PlaylistPanel_::sortByCommonProperties()
	{
	std::list<std::string> prop;
	prop.push_back("artist");
	prop.push_back("album");
	prop.push_back("title");
	conn->playlist.sort(prop,currentPlaylistName)(Xmms::bind(&DataBackend::scrapResult, conn));
	}

void PlaylistPanel_::shuffle()
	{
	conn->playlist.shuffle(currentPlaylistName)(Xmms::bind(&DataBackend::scrapResult, conn));
	}

void PlaylistPanel_::setRepIcon()
	{
	if(repeat_one)
		{
		repeatStateButton->setIcon(QIcon(":images/repeat_track.png"));
		return;
		}
	if(repeat_all)
		{
		repeatStateButton->setIcon(QIcon(":images/repeat_all.png"));
		return;
		}
	repeatStateButton->setIcon(QIcon(":images/repeat_none.png"));
	}

bool PlaylistPanel_::setRepOneVal(std::string val)
	{
	if(val=="0") repeat_one=false;
	if(val=="1") repeat_one=true;
	setRepIcon();
	return false;
	}

bool PlaylistPanel_::setRepAllVal(std::string val)
	{
	if(val=="0") repeat_all=false;
	if(val=="1") repeat_all=true;
	setRepIcon();
	return false;
	}

bool PlaylistPanel_::saveCollAs(const Xmms::Coll::Coll& coll)
	{
	bool ok;
	QString text = QInputDialog::getText(0, "Save as","Save playlist as:", QLineEdit::Normal,QString(), &ok);
	if (ok && !text.isEmpty())
		conn->collection.save(coll,text.toStdString(),Xmms::Collection::PLAYLISTS)
					(Xmms::bind(&XMMS2Interface::scrapResult, conn),Xmms::bind(&PlaylistPanel_::saveError,this));
	return ok;
	}

bool PlaylistPanel_::saveError(const std::string& error)
	{QMessageBox::critical(0, "Could not save playlist", QString("Could not save playlist: ").append(error.c_str()));
	return false;}
#endif
