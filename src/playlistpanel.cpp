#ifndef PLAYLISTPANEL_CPP
#define PLAYLISTPANEL_CPP
#include "playlistpanel.h"
PlaylistPanel::PlaylistPanel(DataBackend * c,QWidget* parent,Qt::WindowFlags f):QWidget(parent,f)
	{
	conn=c;
	layout=new QGridLayout();
	layout->setSpacing(5);
	addIcon=new QIcon(":images/plus.png");
	playlistIcon=new QIcon(":images/playlist.png");
	removeIcon=new QIcon(":images/minus.png");
	repeatAry=new QIcon*[3];
	currentRepeat=0;
	repeatAry[0]=new QIcon(":images/repeat_none.png");
	repeatAry[2]=new QIcon(":images/repeat_track.png");
	repeatAry[1]=new QIcon(":images/repeat_all.png");
	shuffleIcon=new QIcon(":images/shuffle.png");
	sortIcon=new QIcon(":images/sort.png");

	adding=false;
//	playlist=new PlaylistBackend(conn);
//	plist=new PlaylistFrontend(playlist,this);
	plist=new Playlist(conn, this);
	if(conn->getMainPlaylist() == NULL)
	conn->setMainPlaylist(plist);
//	plist->setModel(playlist);
	add=new  QTreeView(this);
	addModel = new QDirModel;
	QStringList filters;
	filters << "*mp3*" << "*m4a*";
	addModel->setNameFilters(filters);
	addModel->setFilter(QDir::AllDirs|QDir::Files|QDir::NoDotAndDotDot|QDir::NoSymLinks);
	add->setModel(addModel);
	add->hide();
	add->setColumnWidth(0,200);
	play=new QPushButton("Play");
	enqueue=new QPushButton("Enqueue");
	editLib=new QPushButton("Toggle medialib");
	QObject::connect(editLib, SIGNAL(clicked()), parent, SLOT(slotMedialibClicked()));
	QObject::connect(play, SIGNAL(clicked()), this, SLOT(slotAdd()));
	QObject::connect(enqueue, SIGNAL(clicked()), this, SLOT(slotEnqueue()));
	play->hide();
	enqueue->hide();
	editLib->hide();
	addLayout=new QGridLayout();
	addLayout->addWidget(add,0,0,1,3);
	addLayout->addWidget(play,1,0);
	addLayout->addWidget(enqueue,1,1);
	addLayout->addWidget(editLib,1,2);
	addLayout->setSpacing(0);
	toggler=new QPushButton();
	toggler->setIcon(*addIcon);
	toggler->setToolTip("Add to playlist");
	addType=0;
	addPlist=new CollectionChooser(Xmms::Playlist::DEFAULT_PLAYLIST,Xmms::Collection::PLAYLISTS,
							conn, this);
	addColl=new CollectionChooser(Xmms::Playlist::DEFAULT_PLAYLIST,Xmms::Collection::COLLECTIONS,
							conn, this);
	QObject::connect(addPlist, SIGNAL(close()),this,SLOT(toggle()));
	QObject::connect(addColl, SIGNAL(close()),this,SLOT(toggle()));
	addPlist->hide();
	addColl->hide();
	layout->addWidget(addPlist,0,1,10,1);
	layout->addWidget(addColl,0,1,10,1);
	addMenu=new QMenu();
	addMenu->addAction("Add from filesystem", this, SLOT(openFilesystem()));
	addMenu->addAction("Add collection", this, SLOT(openCollections()));
	addMenu->addAction("Add playlist", this, SLOT(openPlaylist()));
	toggler->setMenu(addMenu);
	layout->addWidget(plist,1,1,9,1);
	layout->addLayout(addLayout,0,1,10,1);
	layout->setRowMinimumHeight(9,35);
	toggler->setFixedSize(32,32);
	layout->addWidget(toggler,0,0);
	QObject::connect(conn,SIGNAL(currentPos(const unsigned int&)),
					plist,SLOT(currentPos(const unsigned int&)));
	conn->playlist.currentPos()(Xmms::bind(&DataBackend::curPos,conn));
//	QObject::connect(playlist, SIGNAL(needRefresh()),plist, SLOT(updateT()));
	connect(this,SIGNAL(toggledState(bool,std::string)),this,SLOT(stateToggled(bool,std::string)));
	removeMenu=new QMenu(this);
	removeMenu->addAction("Delete selected",this,SLOT(slotRemove()));
	removeMenu->addAction("Crop selected",this,SLOT(slotCrop()));
	removeMenu->addAction("Clear playlist",this,SLOT(slotClear()));
	removeMenu->addAction("Delete playlist",this,SLOT(slotRemovePlaylist()));
	remove=new QPushButton();
	remove->setToolTip("Delete from playlist");
	remove->setIcon(*removeIcon);
	remove->setMenu(removeMenu);
	remove->setFixedSize(32,32);
	layout->addWidget(remove,1,0);

	repeatType=new QPushButton();
	repeatType->setIcon(*repeatAry[currentRepeat]);
	repeatType->setFixedSize(32,32);
	repeatType->setToolTip("Change repetition type");
	layout->addWidget(repeatType,5,0);
	QObject::connect(conn,SIGNAL(configValChanged(const Xmms::Dict&)),
				this,SLOT(changesResponse(const Xmms::Dict&)));
	conn->config.valueGet("playlist.repeat_all")(Xmms::bind(&PlaylistPanel::ovldrepeat,this));
	conn->config.valueSet("playlist.repeat_one","0")(Xmms::bind(&DataBackend::scrapResult, conn));
	QObject::connect(repeatType, SIGNAL(clicked()), this, SLOT(changeRepeatState()));

	shuffle=new QPushButton();
	shuffleMenu=new QMenu(this);
	shuffleMenu->addAction("Shuffle Playlist",this, SLOT(shuffleAction()));
	shuffleMenu->addAction("Toggle Random Playback",plist,SLOT(playRandomly()));
	shuffle->setToolTip("Shuffle playlist");
	shuffle->setMenu(shuffleMenu);
	shuffle->setIcon(*shuffleIcon);
	shuffle->setFixedSize(32,32);
	layout->addWidget(shuffle,6,0);

	sort=new QPushButton();
	sort->setToolTip("Sort");
	sort->setIcon(*sortIcon);
	sort->setFixedSize(32,32);
	sortMenu=new QMenu(this);
	sortMenu->addAction("Sort by Artist",this,SLOT(sortByArtist()));
	sortMenu->addAction("Sort by Title",this,SLOT(sortByTitle()));
	sortMenu->addAction("Sort by Album",this,SLOT(sortByAlbum()));
	sortMenu->addAction("Sort by Filename",this,SLOT(sortByFilename()));
	sort->setMenu(sortMenu);
	layout->addWidget(sort,7,0);

	showDuplicates = new QPushButton();
	showDuplicates->setToolTip("Show Duplicates");
	showDuplicates->setIcon(QIcon(":images/duplicate"));
	showDuplicates->setFixedSize(32,32);
	QObject::connect(showDuplicates, SIGNAL(clicked()), this, SLOT(slotShowDuplicates()));
	dups= false;
	layout->addWidget(showDuplicates,8,0);

	del = new QShortcut(QKeySequence(Qt::Key_Delete),this,SLOT(slotRemove()),SLOT(slotRemove()));


	reformat=false;
	editing=false;
	pass=false;
	plistToggler=new QPushButton();
	plistToggler->setText("Playing:");
	plistToggler->setFixedWidth(100);
	if(conn->getMainPlaylist() != plist)
	slotChangePlistState();
	
	plistChooser=new QComboBox();
	plistChooser->setInsertPolicy(QComboBox::NoInsert);

	conn->playlist.list() (Xmms::bind(&PlaylistPanel::getPlists, this));

	QObject::connect(plistToggler,SIGNAL(clicked()),this,SLOT(slotChangePlistState()));
	QObject::connect(conn,SIGNAL(playlistNameChanged(const std::string &)),
							this,SLOT(changeCurrent(const std::string &)));
	QObject::connect(conn,SIGNAL(collectionsChanged(const Xmms::Dict&)),
							this,SLOT(dictsChanged(const Xmms::Dict&)));
	
	currentPlist=Xmms::Playlist::DEFAULT_PLAYLIST;
	chooserLayout=new QGridLayout();
	chooserLayout->addWidget(plistToggler,0,0);
	chooserLayout->addWidget(plistChooser,0,1);
	layout->addLayout(chooserLayout,0,1,1,1);
	open=new QPushButton();
	open->setFixedSize(32,32);
	open->setIcon(QIcon(":images/open.png"));
	open->setToolTip("Open playlist from file");
	QObject::connect(open,SIGNAL(clicked()),this,SLOT(openPFile()));
	layout->addWidget(open,4,0);
	save=new QPushButton();
	save->setFixedSize(32,32);
	save->setIcon(QIcon(":images/save.png"));
	save->setToolTip("Save copy as");
	QObject::connect(save,SIGNAL(clicked()),this,SLOT(saveCopy()));
	layout->addWidget(save,3,0);
	newPlist=new QPushButton();
	newPlist->setFixedSize(32,32);
	newPlist->setIcon(QIcon(":images/new.png"));
	newPlist->setToolTip("Create new playlist");
	createMenu=new QMenu();
	createMenu->addAction("Create normal playlist",this, SLOT(makePlist()));
	createMenu->addAction("Create queue",this, SLOT(makeQueue()));
	createMenu->addAction("Create party shuffle",this, SLOT(makePShuffle()));
	newPlist->setMenu(createMenu);
	layout->addWidget(newPlist,2,0);
	infoLayout=new QGridLayout();
	history=new QLabel("History:");
	history->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	history->setFixedWidth(75);
	history->hide();
	infoLayout->addWidget(history,0,3);
	upcoming=new QLabel("Upcoming:");
	upcoming->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	upcoming->setFixedWidth(75);
	upcoming->hide();
	infoLayout->addWidget(upcoming,0,1);
	hist=new QSpinBox();
	hist->hide();
	hist->setFixedWidth(50);
	infoLayout->addWidget(hist,0,4);
	QObject::connect(hist, SIGNAL(valueChanged(int)), this, SLOT(setValues(int)));
	up=new QSpinBox();
	up->setFixedWidth(50);
	up->hide();
	QObject::connect(up, SIGNAL(valueChanged(int)), this, SLOT(setValues(int)));
	infoLayout->addWidget(up,0,2);
	layout->addLayout(infoLayout,9,1,1,1);
	conn->playlist.currentActive() (Xmms::bind(&DataBackend::getCurrentPlaylist,conn));
	this->setLayout(layout);
	}

bool PlaylistPanel::ovldrepeat(std::string val)
	{
	this->changeRepeatState(val);
	return false;
	}

void PlaylistPanel::setLayoutSide(bool right_side)
	{
	}

void PlaylistPanel::stateToggled(bool edit,std::string name) {
	if(name == currentPlist) {
		if(editing!=edit) editing = edit;
		if(plist->isEditing() != edit) plist->toggleEdit();
	return;
	}
	
	if(!editing)
	slotChangePlistState();
	
}

void PlaylistPanel::changeRepeatState(std::string val)
	{
	if(val!="-1")
		{
		int value=atoi(val.c_str());
		currentRepeat=value;
		repeatType->setIcon(*repeatAry[currentRepeat]);
		return;
		}
	if(currentRepeat==2)
		{
		conn->config.valueSet("playlist.repeat_one","0")
						(Xmms::bind(&DataBackend::scrapResult, conn));
		currentRepeat=1;
		conn->config.valueSet("playlist.repeat_all","1")
						(Xmms::bind(&DataBackend::scrapResult, conn));
		return;
		}
	if(currentRepeat==0)
		{
		conn->config.valueSet("playlist.repeat_one","1")
						(Xmms::bind(&DataBackend::scrapResult, conn));
		repeatType->setIcon(*repeatAry[2]);
		currentRepeat=2;
		}
	if(currentRepeat==1)
		{
		conn->config.valueSet("playlist.repeat_all","0")
						(Xmms::bind(&DataBackend::scrapResult, conn));
		repeatType->setIcon(*repeatAry[0]);
		currentRepeat=0;
		}
	}

void PlaylistPanel::openFilesystem() {addType=0; toggle();}
void PlaylistPanel::openCollections() {addType=2; toggle();}
void PlaylistPanel::openPlaylist() {addType=1; toggle();}

void PlaylistPanel::toggle()
	{
	if(adding)
		{
		add->hide();
		plist->show();
		plistToggler->show();
		plistChooser->show();
		if(type==XMMS_COLLECTION_TYPE_QUEUE)
			{
			history->show();
			hist->show();
			}
		if(type==XMMS_COLLECTION_TYPE_PARTYSHUFFLE)
			{
			history->show();
			hist->show();
			up->show();
			upcoming->show();
			}
		toggler->setIcon(*addIcon);
		toggler->setToolTip("Add to playlist");
		toggler->disconnect();
		toggler->setMenu(addMenu);
		play->hide();
		enqueue->hide();
		editLib->hide();
		addPlist->hide();
		addColl->hide();
		adding=false;
		return;
		}
	else
		{
		plist->hide();
		plistToggler->hide();
		plistChooser->hide();
		history->hide();
		hist->hide();
		upcoming->hide();
		up->hide();
		switch(addType)
			{
			case 0:
				add->show();
				addModel->refresh();
				play->show();
				enqueue->show();
				editLib->show();
				break;
			case 1:
				addPlist->setPlist(currentPlist);
				addPlist->show();
				break;
			case 2:
				addColl->setPlist(currentPlist);
				addColl->show();
				break;
			};
		toggler->setIcon(*playlistIcon);
		toggler->setToolTip("Return to playlist");
		QObject::connect(toggler, SIGNAL(clicked()), this, SLOT(toggle()));
		toggler->setMenu(NULL);
		adding=true;
		return;
		}
	}

void PlaylistPanel::changesResponse(const Xmms::Dict& valChanged)
	{
	if(valChanged.contains("playlist.repeat_one"))
		{
		std::string type=valChanged.get<std::string>("playlist.repeat_one");
		if(type=="1")
			this->changeRepeatState("2");
		else
			conn->config.valueGet("playlist.repeat_all")
							(Xmms::bind(&PlaylistPanel::ovldrepeat,this));
		return;
		}
	if(valChanged.contains("playlist.repeat_all"))
		{
		if(currentRepeat!=2)
			this->changeRepeatState(valChanged.get<std::string>("playlist.repeat_all"));
		return;
		}
	}

void PlaylistPanel::update(int width, int height)
	{
	layout->setColumnMinimumWidth(1,width-45);
// 	plist->updateT();
	for(int i=0; i<plist->columnCount(); i++)
		plist->setColumnWidth(i,((width-45)/plist->columnCount())-10);
	if(type==XMMS_COLLECTION_TYPE_PARTYSHUFFLE)
		infoLayout->setColumnMinimumWidth(0,width-315);
	if(type==XMMS_COLLECTION_TYPE_QUEUE)
		infoLayout->setColumnMinimumWidth(0,width-180);
	}

void PlaylistPanel::shuffleAction()
	{
	conn->playlist.shuffle(currentPlist)(Xmms::bind(&DataBackend::scrapResult,conn));
	}

void PlaylistPanel::sortByArtist()
	{
	std::list<std::string> foo(1,"artist");
	conn->playlist.sort(foo,currentPlist)(Xmms::bind(&DataBackend::scrapResult,conn));
	}

void PlaylistPanel::sortByTitle()
	{
	std::list<std::string> foo(1,"title");
	conn->playlist.sort(foo,currentPlist)(Xmms::bind(&DataBackend::scrapResult,conn));
	}

void PlaylistPanel::sortByAlbum()
	{
	std::list<std::string> foo(1,"album");
	conn->playlist.sort(foo,currentPlist)(Xmms::bind(&DataBackend::scrapResult,conn));
	}

void PlaylistPanel::sortByFilename()
	{
	std::list<std::string> foo(1,"url");
	conn->playlist.sort(foo,currentPlist)(Xmms::bind(&DataBackend::scrapResult,conn));
	}

void PlaylistPanel::slotAdd()
	{
	if(!adding) return;
	unsigned int foo=plist->topLevelItemCount();
	slotEnqueue();
	if(editing) return;
	conn->playback.stop()(Xmms::bind(&DataBackend::scrapResult, conn));
	conn->playback.tickle()(Xmms::bind(&DataBackend::scrapResult, conn));
	conn->playlist.setNext(foo)(Xmms::bind(&DataBackend::scrapResultI, conn));
	conn->playback.tickle()(Xmms::bind(&DataBackend::scrapResult, conn));
	conn->playback.start()(Xmms::bind(&DataBackend::scrapResult, conn));
	}

void PlaylistPanel::slotEnqueue()
	{
	if(!adding) return;
	QModelIndex index=add->selectionModel()->currentIndex();
	QString path=addModel->filePath(index);
	path.prepend("file://");
	if(addModel->isDir(index))
		{
		conn->playlist.addRecursive(path.toStdString(),currentPlist)
								(Xmms::bind(&DataBackend::scrapResult,conn));
		}
	else
		{
			conn->playlist.addUrl(path.toStdString(),currentPlist)
								(Xmms::bind(&DataBackend::scrapResult,conn));
		}
	toggle();
	}

void PlaylistPanel::slotCrop()
	{
	if(adding) return;
	QItemSelectionModel * model=plist->selectionModel();
	QModelIndexList oldList=model->selectedRows();
	QList<int> list;
	for(int i=0; i<oldList.size(); ++i)
		{
		list<<oldList.at(i).row();
		}
	qSort(list.begin(), list.end());
	int Size=list.size();
	for(int i=0; i<Size; ++i)
		{
		conn->playlist.moveEntry(list.at(i),i,currentPlist)
							(Xmms::bind(&DataBackend::scrapResult, conn));
		}
	for(int i=Size; i<plist->topLevelItemCount(); ++i)
		{
		conn->playlist.removeEntry(Size,currentPlist)
						(Xmms::bind(&DataBackend::scrapResult, conn));
		}
	if(editing) return;
	if(plist->topLevelItemCount()==0)
		conn->playback.stop()(Xmms::bind(&DataBackend::scrapResult, conn));
	}

void PlaylistPanel::slotRemove()
	{
	if(editing) {
		plist->removeCheckedItems();
	}
	if(!plist->hasFocus()) return;
	if(adding) return;
	QItemSelectionModel * model=plist->selectionModel();
	QModelIndexList oldList=model->selectedRows();
	QList<int> list;
	for(int i=0; i<oldList.size(); ++i)
		{
		list<<oldList.at(i).row();
		}
	qSort(list.begin(), list.end());
	for(int i=list.size()-1; i>=0; --i)
		{
		conn->playlist.removeEntry(list.at(i),currentPlist)
							(Xmms::bind(&DataBackend::scrapResult, conn));
		}
// 	if(plist->back()->rowCount(plist->back()->index(0,0))==0)
	if(plist->topLevelItemCount()==0)
		conn->playback.stop()(Xmms::bind(&DataBackend::scrapResult, conn));
	}

void PlaylistPanel::slotClear()
	{
	if(adding) return;
	plist->currentPos(0);
	conn->playlist.clear(currentPlist)(Xmms::bind(&DataBackend::scrapResult, conn));
	}

bool PlaylistPanel::getPlists(const Xmms::List<std::string>& list)
	{
	std::cout<<std::endl<<"BEGIN OUTPUT COLLDATA TEST, these are the playlists:"<<std::endl;
	QStringList foo=((CollData *)conn->getDataBackendObject(DataBackend::COLL))->getPlaylists();
	for(int i=0; i<foo.size(); i++)
		std::cout<<foo[i].toStdString()<<std::endl;
	std::cout<<"END COLLDATA TEST. Test located in PlaylistPanel::getPlists"<<std::endl<<std::endl;
	std::string tmp=currentPlist;
	reformat=true;
	for(int i=plistChooser->count()-1; i>=0; i--)
		plistChooser->removeItem(i);
	for (list.first (); list.isValid (); ++list) {
		if((*list).substr(0,1)=="_") continue;
		plistChooser->addItem((*list).c_str());
	}
		
	QObject::connect(plistChooser,SIGNAL(currentIndexChanged(int)),
					this,SLOT(slotChangePlist(int)));
	reformat=false;
	int idx = plistChooser->findText(QString(tmp.c_str()));
	if(idx==-1) return true;
	plistChooser->setCurrentIndex(idx);
	if(idx==0)slotChangePlist(0);
	return true;
	}

void PlaylistPanel::slotChangePlist(int index)
	{
	QString text = plistChooser->itemText(index );
// 	//std::cout<<"CURRENT"<<currentPlist<<std::endl;
	emit(needRefresh());
	conn->collection.get(text.toStdString(),Xmms::Collection::PLAYLISTS)
						(Xmms::bind(&PlaylistPanel::getType,this));
	if(adding) return;
	if(!editing&&!reformat)
	conn->playlist.load(text.toStdString())
					(Xmms::bind(&DataBackend::scrapResult, conn));
	else
		currentPlist=text.toStdString();
// 	slotChangePlistState();
	plist->getCurrentName(text.toStdString());
// 	slotChangePlistState();
	}

void PlaylistPanel::slotChangePlistState()
	{
	if(adding) return;
	plist->toggleEdit();
	if(editing)
		{
		plistToggler->setText("Playing:");
		editing=false;
		conn->playlist.load(plistChooser->currentText().toStdString())
							(Xmms::bind(&DataBackend::scrapResult, conn));
		}
	else
		{
		plistToggler->setText("Editing:");
		editing=true;
		}

	emit toggledState(plist->isEditing(),currentPlist);
	}

void PlaylistPanel::changeCurrent(const std::string & name)
	{
	if(name==currentPlist) return;
	currentPlist=name;
	if(editing) return;
	int idx = plistChooser->findText(QString(name.c_str()));
	if(idx==-1) return;
	plistChooser->setCurrentIndex(idx);
	}

void PlaylistPanel::dictsChanged(const Xmms::Dict& change)
	{
	if(change.get<int>("type")!=XMMS_COLLECTION_CHANGED_UPDATE)
		conn->playlist.list() (Xmms::bind(&PlaylistPanel::getPlists, this));
	}

void PlaylistPanel::slotRemovePlaylist()
	{
	int idx = plistChooser->findText(QString(currentPlist.c_str()));
	plistChooser->removeItem(idx);
	plistChooser->setCurrentIndex(0);
	conn->collection.remove(currentPlist,Xmms::Collection::PLAYLISTS)
						(Xmms::bind(&DataBackend::scrapResult, conn));
	}

void PlaylistPanel::saveCopy()
	{
	conn->collection.get(currentPlist,Xmms::Collection::PLAYLISTS)
						(Xmms::bind(&PlaylistPanel::saveThis,this));
	}

void PlaylistPanel::openPFile()
	{
	QString fileName=QFileDialog::getOpenFileName(0, tr("Open Playlist File"),"/",
											tr("Playlists(*.m3u *.pls)"));
	fileName.prepend("file://");
	conn->collection.idlistFromPlaylistFile(fileName.toStdString())
									(Xmms::bind(&PlaylistPanel::saveThis,this),
									Xmms::bind(&PlaylistPanel::openError,this));
	}

bool PlaylistPanel::saveThis(const Xmms::Coll::Coll& is)
	{
	bool ok;
	QString text = QInputDialog::getText(0, "Save as",
									"Save playlist as:", QLineEdit::Normal,
									QString(), &ok);
	if (ok && !text.isEmpty())
		conn->collection.save(is,text.toStdString(),Xmms::Collection::PLAYLISTS)
								(Xmms::bind(&DataBackend::scrapResult, conn),
									Xmms::bind(&PlaylistPanel::openError,this));;
	return true;
	}

bool PlaylistPanel::openError(const std::string& error)
	{
	QMessageBox::critical(0, "Could not open playlist", QString("Could not open playlist: ").
															append(error.c_str()));
	return true;
	}

void PlaylistPanel::makePlist()
	{saveThis(Xmms::Coll::Idlist());}

void PlaylistPanel::makePShuffle()
	{
	Xmms::Coll::PartyShuffle *pshuffle=new Xmms::Coll::PartyShuffle();
	pshuffle->setAttribute("history","0");
	pshuffle->setAttribute("upcoming","10");
	Xmms::Coll::Universe * univ=new Xmms::Coll::Universe();
	pshuffle->setOperand(*univ);
	saveThis(*pshuffle);
	}

void PlaylistPanel::makeQueue()
	{
	Xmms::Coll::Queue* queue=new Xmms::Coll::Queue();
	queue->setAttribute("history","0");
	saveThis(*queue);
	}

void PlaylistPanel::setValues(int val)
	{
	if(pass) return;
	conn->collection.get(currentPlist,Xmms::Collection::PLAYLISTS)
						(Xmms::bind(&PlaylistPanel::setVals,this));
	}

bool PlaylistPanel::getType(const Xmms::Coll::Coll& co)
	{
	pass=true;
	type=co.getType();
	plist->hide();
	layout->removeWidget(plist);
	if(type==XMMS_COLLECTION_TYPE_PARTYSHUFFLE)
		{
		try{hist->setValue(atoi(co.getAttribute("history").c_str()));}
		catch(...)
			{
			Xmms::Coll::Coll * col=const_cast<Xmms::Coll::Coll *> (&co);
			col->setAttribute("history", "0");
			hist->setValue(0);
			conn->collection.save(*col,currentPlist,Xmms::Collection::PLAYLISTS)
							(Xmms::bind(&DataBackend::scrapResult, conn));
			}
		try{up->setValue(atoi(co.getAttribute("upcoming").c_str()));}
		catch(...)
			{
			Xmms::Coll::Coll * col=const_cast<Xmms::Coll::Coll *> (&co);
			col->setAttribute("upcoming", "10");
			up->setValue(10);
			conn->collection.save(*col,currentPlist,Xmms::Collection::PLAYLISTS)
							(Xmms::bind(&DataBackend::scrapResult, conn));
			}
		layout->addWidget(plist,1,1,8,1);
		plist->show();
		history->show();
		hist->show();
		up->show();
		upcoming->show();
		}
	if(type==XMMS_COLLECTION_TYPE_QUEUE)
		{
		try{hist->setValue(atoi(co.getAttribute("history").c_str()));}
		catch(...)
			{
			Xmms::Coll::Coll * col=const_cast<Xmms::Coll::Coll *> (&co);
			col->setAttribute("history", "0");
			hist->setValue(0);
			conn->collection.save(*col,currentPlist,Xmms::Collection::PLAYLISTS)
							(Xmms::bind(&DataBackend::scrapResult, conn));
			}
		layout->addWidget(plist,1,1,8,1);
		plist->show();
		history->show();
		hist->show();
		up->hide();
		upcoming->hide();
		}
	if(type==XMMS_COLLECTION_TYPE_IDLIST)
		{
		layout->addWidget(plist,1,1,9,1);
		plist->show();
		history->hide();
		hist->hide();
		up->hide();
		upcoming->hide();
		}
	pass=false;
	return true;
	}

bool PlaylistPanel::setVals(const Xmms::Coll::Coll& co)
	{
	Xmms::Coll::Coll * col=const_cast<Xmms::Coll::Coll *> (&co);
	std::stringstream ss;
	std::string str;
	if(type==XMMS_COLLECTION_TYPE_PARTYSHUFFLE)
		{
		ss<<up->value(); ss>>str;
		col->setAttribute("upcoming", str);
		ss.clear();
		ss<<hist->value(); ss>>str;
		col->setAttribute("history", str);
		}
	if(type==XMMS_COLLECTION_TYPE_QUEUE)
		{
		ss<<hist->value(); ss>>str;
		col->setAttribute("history", str);
		}
	conn->collection.save(*col,currentPlist,Xmms::Collection::PLAYLISTS)
							(Xmms::bind(&DataBackend::scrapResult, conn));
	return true;
	}

//Joe
void PlaylistPanel::updateHeaders(QList<QString> list) {
	plist->setHeader(list);
}
void PlaylistPanel::slotShowDuplicates() {
// 	if(!plist->isEditing())
	slotChangePlistState();

	if(!dups) {
	plist->showDuplicates();
	dups = true;
	}
	else {
	plist->resetColors();
	dups=false;
	}
}

//!ANTON god damnit why'd you abstract the playlist...
Playlist* PlaylistPanel::returnPlaylist() {
return plist;
}
//Done

#endif
