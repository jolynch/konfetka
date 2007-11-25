#ifndef MEDIALIB_CPP
#define MEDIALIB_CPP
#include "medialib.h"
#include <iostream>


MediaLib::MediaLib(DataBackend * c,  QWidget * parent, Qt::WindowFlags f):LayoutPanel(parent, f)
	{
	conn = c;
	mlib = (MlibData*)(conn->getDataBackendObject(DataBackend::MLIB));
	coll = (CollData*)(conn->getDataBackendObject(DataBackend::COLL));
	
	complexSearch = false;
	layout = new QGridLayout();
	delItem = new QShortcut(Qt::Key_Delete,this,SLOT(slotRemove()),SLOT(slotRemove()));

	QStringList Hlabels;
	Hlabels << "Your MediaLib";
	mediaList = new DropTreeWidget(this,c);
	mediaList->setHeaderLabels(Hlabels);
	mediaList->setEditTriggers(QAbstractItemView::NoEditTriggers);
	mediaList->setSelectionMode(QAbstractItemView::ExtendedSelection);
	mediaList->setDragEnabled(true);
	mediaList->setAcceptDrops(true);
	mediaList->setDropIndicatorShown(true);
	mediaList->setObjectName ("mediaList");

	add = new QPushButton();
	add->setIcon(QIcon(":images/file_system"));
	add->setToolTip("Show The File Chooser");
	add->setFixedSize(32,32);

	update = new QPushButton();
	update->setIcon(QIcon(":images/repeat_all"));
	update->setToolTip("Update Your MediaLib");
	update->setFixedSize(32,32);

	QMenu * makeCollMenu = new QMenu();
	makeCollMenu->addAction("Use Visible Items",this,SLOT(useVisible()));
	makeCollMenu->addAction("Use Selected Items",this,SLOT(useSelected()));
	makeColl = new QPushButton();
	makeColl->setIcon(QIcon(":images/playlist"));
	makeColl->setToolTip("Do Stuff With Your Media");	
	makeColl->setFixedSize(32,32);
	makeColl->setMenu(makeCollMenu);

	searchLine = new QLineEdit();
	searchLabel = new QLabel("Search");
	complexSearchButton = new QPushButton("Simple");
	QMenu * menu = new QMenu();
	menu->addAction(QIcon(":images/repeat_all"),"Toggle to Complex",this,SLOT(toggleComplexSearch()));
	menu->addAction(QIcon(":images/plus"),"Add Another Search Term",this,SLOT(addAnotherSearchItem()));
	complexSearchButton->setMenu(menu);
// 	complexSearchButton->setFixedSize(42,25);

	QStringList filters;
	filters << "*mp3*" << "*m4a*";
	for(int i =1;i<4;i++)

	buttons = new QVBoxLayout();	
	layout->addWidget(searchLine,0,0,1,1);
	layout->addWidget(mediaList,1,0,1,1);
	layout->addWidget(complexSearchButton,0,1,1,1,Qt::AlignHCenter);
	buttons->addWidget(add,Qt::AlignHCenter);
	buttons->addWidget(update,Qt::AlignHCenter);
	buttons->addWidget(makeColl,Qt::AlignHCenter);
	layout->addLayout(buttons,1,1,1,1,Qt::AlignHCenter | Qt::AlignTop);
	layout->setSpacing(5);
	layout->setRowStretch(1,1);

	setLayout(layout);

	searchDialog = new ComplexSearchDialog(conn);


	connect(add,SIGNAL(clicked()),this,SLOT(toggleFileList()));
	connect(update,SIGNAL(clicked()),this,SLOT(refreshList()));
	connect(searchLine,SIGNAL(returnPressed()),this,SLOT(searchMlib()));
	
	connect(mediaList,SIGNAL(itemPressed(QTreeWidgetItem *,int)),this,SLOT(addToMlibDrag(QTreeWidgetItem*,int))); 
	connect(mediaList,SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)),this,SLOT(addToMlibDoubleClick(QTreeWidgetItem*,int)));
	connect(mediaList,SIGNAL(itemClicked(QTreeWidgetItem *,int)),this,SLOT(stopTimerAndClearList()));

	connect(&doubleClickTimer,SIGNAL(timeout()),this,SLOT(startDrag()));
	
	//New List thingy
	connect(mlib,SIGNAL(gotListFromServer(QString, QList<QString>)),
					this,SLOT(gotNewList(QString, QList<QString>)));
	connect(mlib,SIGNAL(infoChanged(int)),
					this,SLOT(infoChanged(int)));
	connect(mlib,SIGNAL(updatesDone()),
					this,SLOT(checkIfRefreshIsNeeded()));
	connect(mediaList,SIGNAL(itemExpanded(QTreeWidgetItem*)),this,SLOT(itemExpanded(QTreeWidgetItem*)));

	connect(searchDialog,SIGNAL(newList(QList< QPair <Xmms::Coll::Coll*,Operator> >)),
			this,SLOT(recievedNewList(QList< QPair <Xmms::Coll::Coll*,Operator> >)));
	visibleMedia = new Xmms::Coll::Universe();
	mlib->getListFromServer(visibleMedia,"artist");
	adding=false;


// 	loadFromFile();
}

MediaLib::~MediaLib() {
	delete mediaList;
	delete add;
	delete update;
	delete makeColl;
	delete searchLine;
	delete searchLabel;
}

void MediaLib::useSelected() {
	QStringList options;
	bool ok;
	options<<"Collection"<<"Party Shuffle"<<"Queue";
	QString val = QInputDialog::getItem(this, "Do Stuff","Create a ",options, 0, false, &ok);
	if(!ok) return;
	if(val =="Collection")
	newColl(SELECTED);
}

void MediaLib::useVisible() {
	QStringList options;
	bool ok;
	options<<"Collection"<<"Party Shuffle"<<"Queue";
	QString val = QInputDialog::getItem(this, "Do Stuff","Create a ",options, 0, false, &ok);
	if(!ok) return;
	if(val =="Collection")
	newColl(VISIBLE);
}


void MediaLib::newColl(SourceType type) {
	bool ok = true;
	QString name = "";
		while(name=="" && ok) {
		name = QInputDialog::getText(this,"Name?",
							"What name would you like the collection to be saved as",
							QLineEdit::Normal,"", &ok);
		}
		if(!ok) return;

	QString tmp;
	if(type == SELECTED)
	tmp = "selected";
	else
	tmp = "visible";

	int val = QMessageBox::information(this,"Save To Collection",
			"Are you sure that you want to save\nthe "+tmp+" items to the collection: "+name,
			QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes);
	if(val != QMessageBox::Yes) return;
	
	if(type == VISIBLE)
	coll->createCollection(*visibleMedia,name.toStdString(),conn->collection.COLLECTIONS);
	else
	coll->createCollection(*selectedAsColl(),name.toStdString(),conn->collection.COLLECTIONS);
}


void MediaLib::gotNewList(QString property, QList<QString> info) {
	if(property == "artist")
	artistList(info);
	else
	std::cout<<"Don't support anything but artist right now"<<std::endl;
}

void MediaLib::artistList(QList<QString> info) {
// 	mediaList->clear();
// 	((MlibData*)(conn->getDataBackendObject(DataBackend::MLIB)))->clearCache();
	while (mediaList->topLevelItemCount() > 0)
	mediaList->takeTopLevelItem(0); 

		for(int i=0;i<info.size();i++) {
		QTreeWidgetItem * newItem = new QTreeWidgetItem();
		newItem->setText(0,info.value(i));
		mediaList->addTopLevelItem(newItem);
		QTreeWidgetItem * waitItem = new QTreeWidgetItem(newItem);
		waitItem->setText(0,"...");
		}
	mediaList->sortItems(0,Qt::AscendingOrder);
	Xmms::Coll::Has artColl(*visibleMedia,"artist");
	Xmms::Coll::Complement albumItems;
	albumItems.setOperand(artColl);
	conn->collection.queryIds(albumItems)(Xmms::bind(&MediaLib::handleUnknown,this));
}

bool MediaLib::handleUnknown(const Xmms::List <uint> &list) {
		for (list.first();list.isValid(); ++list) {
			QTreeWidgetItem * un = new QTreeWidgetItem();
			un->setText(0,"Unknown");
			mediaList->addTopLevelItem(un);
			QTreeWidgetItem * waitItem = new QTreeWidgetItem(un);
			waitItem->setText(0,"...");
			return true;
		}
	return true;
}

void MediaLib::itemExpanded(QTreeWidgetItem* item){
	if(item->parent()==NULL) {
		if(item->childCount()==1 && item->child(0)->text(0)=="...")
		getAlbumList(item);
	}
	else {
	getSongInfo(item);
	}
}

void MediaLib::getAlbumList(QTreeWidgetItem* item) {
// 	std::cout<<item->text(0).toStdString()<<std::endl;
	if(item->text(0)!="Unknown") {
		Xmms::Coll::Equals albumItems(*visibleMedia,"artist",item->text(0).toStdString(),true);
		std::list<std::string> what;
		what.push_back("album");
		what.push_back("title");

		conn->collection.queryInfos(albumItems,what)(boost::bind(&MediaLib::gotAlbums,this,item,_1));
	}
	else {	
		Xmms::Coll::Has artColl(*visibleMedia,"artist");
		Xmms::Coll::Complement albumItems;
		albumItems.setOperand(artColl);
		
		std::list<std::string> what;
		what.push_back("album");
		what.push_back("title");

		conn->collection.queryInfos(albumItems,what)(boost::bind(&MediaLib::gotAlbums,this,item,_1));
	}
}

bool MediaLib::gotAlbums(QTreeWidgetItem* artist,const Xmms::List <Xmms::Dict> &list) {
	artist->takeChildren();
	QString tmp;
	QList<QString> artistList;
		for (list.first();list.isValid(); ++list) {
			if(list->contains("album")) {
			tmp = QString((list->get<std::string>("album")).c_str());
			}
			else if(list->contains("title")) {
			tmp = "Unknown";
			}
			else {
			continue;
			}
	
			if(artistList.contains(tmp)) continue;
			artistList.append(tmp);
			QTreeWidgetItem * newAlbum = new QTreeWidgetItem(artist);
			newAlbum->setText(0,tmp);
			QTreeWidgetItem * tempSong = new QTreeWidgetItem(newAlbum);
			tempSong->setText(0,"...");
		}
		
		if(artist->childCount()==0) {	
		QTreeWidgetItem * newAlbum = new QTreeWidgetItem(artist);
		newAlbum->setText(0,"Unknown");
		QTreeWidgetItem * tempSong = new QTreeWidgetItem(newAlbum);
		tempSong->setText(0,"...");
		}
	return true;
}

void MediaLib::getSongInfo(QTreeWidgetItem* item){
	if(item->parent()==NULL) return;
		Xmms::Coll::Coll * albumItems;
		if(item->parent()->text(0)!="Unknown")
		albumItems = new Xmms::Coll::Equals(*visibleMedia,"artist",item->parent()->text(0).toStdString(),true);
		else {
		Xmms::Coll::Has artColl(*visibleMedia,"artist");
		albumItems = new Xmms::Coll::Complement();
		albumItems->setOperand(artColl);
		}
		
		if(item->text(0)!="Unknown") {
		Xmms::Coll::Equals::Equals songItems(*albumItems,"album",item->text(0).toStdString(),true);
		conn->collection.queryIds(songItems)(boost::bind(&MediaLib::gotSongs,this,item,_1));
		}
		else {
		Xmms::Coll::Has artColl(*visibleMedia,"album");
		Xmms::Coll::Complement notColl;
		notColl.setOperand(artColl);
		Xmms::Coll::Intersection songItems;
		songItems.addOperand(*albumItems);
		songItems.addOperand(notColl);
		conn->collection.queryIds(songItems)(boost::bind(&MediaLib::gotSongs,this,item,_1));
		}
}

bool MediaLib::gotSongs(QTreeWidgetItem* album,const Xmms::List <uint> &list) {
// 	std::cout<<album->text(0).toStdString()<<std::endl;
	album->takeChildren();
	QTreeWidgetItem * temp;
	QVariant tmp; QString title = "";
		for (list.first();list.isValid(); ++list) {
			tmp = mlib->getInfo(QString("title"),*list);
			if(tmp.toInt() == -1) {
			title = "";
			}
			else {
			title = tmp.toString();
			}
			temp = new QTreeWidgetItem();
			temp->setText(0,title);
			std::cout<<*list << title.toStdString()<<std::endl;
			album->addChild(temp);
			idToSongItem.insert(*list,temp);
		}
		
		if(album->childCount()==0) {
// 		delete album;
		}
	album->sortChildren(0,Qt::AscendingOrder);
	return true;
}

void MediaLib::infoChanged(int id) {
	if(idToSongItem.contains(id) && idToSongItem.value(id)!=NULL) {
		QVariant tmp = mlib->getInfo(QString("title"),id);
		idToSongItem.value(id)->setText(0,tmp.toString());
	}
	else {
	idStack.push(id);
	}
}

void MediaLib::slotRemove() {
	if(!mediaList->hasFocus()) return;
	QList<QTreeWidgetItem *> what;
	what = mediaList->selectedItems();
	Xmms::Coll::Union * media = selectedAsColl();
	
	conn->collection.queryIds(*media)(Xmms::bind(&MediaLib::removeIds,this));
	removeNodes(what);
}

void MediaLib::removeNodes(QList<QTreeWidgetItem*> list) {
	QTreeWidgetItem* curItem;
		for(int i=0;i<list.size();i++) {
			curItem = list.value(i);
			switch (getItemType(curItem)) {
				case ARTIST: {
					for(int i=0;i<curItem->childCount();i++)
					list.append(curItem->child(i));
					break;
				}
				case ALBUM: {
					for(int i=0;i<curItem->childCount();i++)
					list.append(curItem->child(i));
					break;
				}
				case SONG: {
					QTreeWidgetItem* ptr = curItem->parent();
					QTreeWidgetItem* ptr2 = curItem->parent()->parent();
					idToSongItem.remove(idToSongItem.key(curItem));
					delete ptr->takeChild(ptr->indexOfChild(curItem));
					if(ptr->childCount()==0)
					delete ptr2->takeChild(ptr2->indexOfChild(ptr));
					if(ptr2->childCount()==0)
					delete mediaList->takeTopLevelItem(mediaList->indexOfTopLevelItem(ptr2));
					break;
				}
			}
		}
}
	
bool MediaLib::removeIds(const Xmms::List <uint> &list) {
	for (list.first();list.isValid(); ++list) {
// 	std::cout<<"Would be removing "<<*list<<std::endl;
	conn->medialib.removeEntry(*list)(Xmms::bind(&DataBackend::scrapResult, conn));
	}
	return true;
}
	
void MediaLib::refreshList() {
	mlib->getListFromServer(visibleMedia,"artist");
}


void MediaLib::checkIfRefreshIsNeeded() {
	uint id;
// 	bool needToUpdate=false;
// 	QTreeWidgetItem* artist;
// 	QTreeWidgetItem* album;
		while(!idStack.isEmpty()) {
		id = idStack.pop();
// 		if(!idToSongItem.contains(id) || idToSongItem.value(id)==NULL) continue;
// 		QTreeWidgetItem * itm = idToSongItem.value(id);
// 		album = itm->parent();
// 		artist = album->parent();
// 		
// 		bool needToUpdate = false;
// 		QString artist = itm->info("artist").toString();
// 		QString album = itm->info("album").toString();
// 		QTreeWidgetItem* artistNode = NULL;
// 		QList<QTreeWidgetItem*> listArtist = mediaList->findItems(artist,Qt::MatchExactly);
// 			for(int i=0;i<listArtist.size();i++) {
// 				if(listArtist.value(i)->parent()==NULL) {
// 					artistNode = listArtist.value(i);
// 					if(listArtist.value(i)->isExpanded())
// 					needToUpdate = true;
// 					break;
// 				}
// 			}
// 			if(artistNode == NULL)
// 			needToUpdate = true;
// 		QList<QTreeWidgetItem*> listAlbum = mediaList->findItems(album,Qt::MatchExactly);
// 			for(int i=0;i<listAlbum.size();i++) {
// 				if(listAlbum.value(i)->parent()==NULL && listAlbum.value(i)->isExpanded()) {
// 					needToUpdate = true;
// 					break;
// 				}
// 			}
// 		if(needToUpdate) {
// 		((MlibData*)(conn->getDataBackendObject(DataBackend::MLIB)))->getListFromServer(visibleMedia,"artist");
// 		while(!idStack.isEmpty())
// 		idStack.pop();
// 		return;
// 		}
	}
	mlib->getListFromServer(visibleMedia,"artist");
}


void MediaLib::toggleFileList() {
	add->setToolTip("Hide The File Chooser");
}

void MediaLib::searchMlib() {
	std::string text = searchLine->text().toStdString();
	Xmms::Coll::Universe univ;
	Xmms::Coll::Union allMatches;
		Xmms::Coll::Match matchedArt(univ,"artist","%"+text+"%");
		allMatches.addOperand(matchedArt);
		Xmms::Coll::Match matchedAlb(univ,"album","%"+text+"%");
		allMatches.addOperand(matchedAlb);
		Xmms::Coll::Match matchedTit(univ,"title","%"+text+"%");
		allMatches.addOperand(matchedTit);
		Xmms::Coll::Match matchedUrl(univ,"url","%"+text+"%");
		allMatches.addOperand(matchedUrl);
		Xmms::Coll::Match matchedGenre(univ,"genre","%"+text+"%");
		allMatches.addOperand(matchedGenre);
		Xmms::Coll::Match matchedID(univ,"id","%"+text+"%");
		allMatches.addOperand(matchedID);
	delete visibleMedia;
	visibleMedia = new Xmms::Coll::Union(allMatches);
	mlib->getListFromServer(visibleMedia,"artist");
}
	
ItemType MediaLib::getItemType(QTreeWidgetItem* item) {
	if(item->parent() == NULL)
	return ARTIST;
	else if(item->childCount()==0)
	return SONG;
	else
	return ALBUM;
}

Xmms::Coll::Union* MediaLib::selectedAsColl() {
	QList<QTreeWidgetItem *> what;
	what =mediaList->selectedItems();
	Xmms::Coll::Union * media = new Xmms::Coll::Union();
		for(int i=0;i<what.size();i++) {
			switch (getItemType(what.value(i))) {
				case ARTIST: {
				Xmms::Coll::Equals moreItems(*visibleMedia,"artist",(what.value(i))->text(0).toStdString(),true);
				media->addOperand(moreItems);
				break;
				}
				case ALBUM: {
				Xmms::Coll::Equals someItems(*visibleMedia,"artist",
								(what.value(i))->parent()->text(0).toStdString(),true);	
					if(what.value(i)->text(0)=="Unknown") {
					Xmms::Coll::Has artColl(*visibleMedia,"album");
					Xmms::Coll::Complement notColl;
					notColl.setOperand(artColl);
					Xmms::Coll::Intersection songItems;
					songItems.addOperand(someItems);
					songItems.addOperand(notColl);
					media->addOperand(songItems);
					}
					else {
					Xmms::Coll::Equals moreItems(someItems,"album",(what.value(i))->text(0).toStdString(),true);
					media->addOperand(moreItems);
					}
// 				std::cout<<"ALBUM"<<std::endl;
				break;
				}
				case SONG: {
				Xmms::Coll::Idlist moreItems;
				moreItems.append(idToSongItem.key(what.value(i)));
				media->addOperand(moreItems);
// 				std::cout<<"SONG"<<std::endl;
				break;
				}
			}
		}
	return media;
}

void MediaLib::addToMlibDrag(QTreeWidgetItem*,int) {
	urlList.clear();
	Xmms::Coll::Union * media = selectedAsColl();
	
	std::list<std::string> tmp;
 	tmp.push_back("url");
	conn->collection.queryInfos(*media,tmp)(Xmms::bind(&MediaLib::addToPlaylistFromCollectionDrag,this));
}

bool MediaLib::addToPlaylistFromCollectionDrag(const Xmms::List <Xmms::Dict> &list) {
	std::string tmpString;
	for (list.first();list.isValid(); ++list) {
		if(list->contains("url")) {
		tmpString = list->get<std::string>("url");
		tmpString = QString::fromUtf8 (xmmsc_result_decode_url (NULL,tmpString.c_str())).toStdString();
		urlList.append(QUrl(QString(tmpString.c_str())));
		}
	}
// 		for(int i=0;i<urlList.size();i++)
// 		std::cout<<urlList.value(i).toString().toStdString()<<std::endl;
	doubleClickTimer.start(qApp->doubleClickInterval());
	return true;
}

void MediaLib::addToMlibDoubleClick(QTreeWidgetItem * item,int) {
//		for(int i=0;i<urlList.size();i++) {
//		//std::cout<<urlList.value(i).toString().toStdString()<<std::endl;
//		}
	urlList.clear();
// 	Xmms::Coll::Union * media = selectedAsColl();
	
	doubleClickTimer.stop();
}

void MediaLib::startDrag() {
	QDrag * drag = new QDrag(this);
	QMimeData * mimeData = new QMimeData;
	mimeData->setUrls(urlList);
	drag->setMimeData(mimeData);
	drag->setPixmap(QPixmap(":images/volume_button_sound"));
// 	std::cout<<urlList.size()<<std::endl;
	urlList.clear();
	drag->start();
	doubleClickTimer.stop();
}

void MediaLib::stopTimerAndClearList() {
	doubleClickTimer.stop();
	urlList.clear();
}

void MediaLib::toggleComplexSearch() {
	complexSearch = ! complexSearch;
	QString toggleTo;
	if(complexSearch) {
	toggleTo = "Toggle to Simple";
	complexSearchButton->setText("Complex");
	searchLine->setEnabled(false);
	searchLine->setText("Hello World");
	addAnotherSearchItem();
	}
	else {
	toggleTo = "Toggle to Complex";
	complexSearchButton->setText("Simple");
	searchLine->clear();
	searchLine->setEnabled(true);
	searchDialog->clearItems();
		if(visibleMedia!=NULL)
		delete visibleMedia;
		visibleMedia = new Xmms::Coll::Universe();
		mlib->getListFromServer(visibleMedia,"artist");
	}

	QMenu * menu = new QMenu();
	menu->addAction(QIcon(":images/repeat_all"),toggleTo,this,SLOT(toggleComplexSearch()));
	menu->addAction(QIcon(":images/plus"),"Add Another Search Term",this,SLOT(addAnotherSearchItem()));
	complexSearchButton->setMenu(menu);
}

void MediaLib::addAnotherSearchItem() {
	if(complexSearch)
	searchDialog->exec();
}

void MediaLib::recievedNewList(QList< QPair <Xmms::Coll::Coll*,Operator> > newList) {
	Xmms::Coll::Coll* val = NULL;
	
	for(int i =0; i < newList.size();i++) {
		QPair <Xmms::Coll::Coll*,Operator> temp = newList.value(i);
		
		if(temp.second == opor) {
		Xmms::Coll::Union* tmp = new Xmms::Coll::Union();
		tmp->addOperand(*temp.first);
		if(val!=NULL)
		tmp->addOperand(*val);
		val = tmp;
		}
		else {
		Xmms::Coll::Intersection* tmp = new Xmms::Coll::Intersection();
		tmp->addOperand(*temp.first);
		if(val!=NULL)
		tmp->addOperand(*val);
		val = tmp;
		}
	}

	delete visibleMedia;
	visibleMedia = val;
	mlib->getListFromServer(visibleMedia,"artist");
}

void MediaLib::setLayoutSide(bool right_side) { //true=right, false=left
}

DropTreeWidget::DropTreeWidget(MediaLib* newLib,DataBackend* c):QTreeWidget() {
	setDragDropMode(QAbstractItemView::DropOnly);
	setAcceptDrops(true);
	lib = newLib;
	path = new QString();
	conn = c;
	QStringList filters;
	filters << "*mp3*" << "*m4a*";
	dirModel = new QDirModel();
	dirModel->setNameFilters(filters);
	dirModel->setFilter(QDir::AllDirs|QDir::Files|QDir::NoDotAndDotDot);
	
}

//pathImport does not _work_ completely ... so I wrote my own :-)
void DropTreeWidget::recurAdd(QString file,bool isDir) {
	if(isDir) {
	QDir dir(file);
	QStringList filters;
	filters << "*mp3*" << "*m4a*";
	dir.setNameFilters(filters);
	dir.setFilter(QDir::AllDirs|QDir::Files|QDir::NoDotAndDotDot);
	QFileInfoList list(dir.entryInfoList());
		for(int i=0;i<list.size();i++) {
		recurAdd(list.value(i).filePath(),list.value(i).isDir());
		}
	}
	else {
	
	if(!file.startsWith("file://"))
	file.prepend("file://");
// 	//std::cout<<file.toStdString()<<std::endl;
	conn->medialib.addEntry(file.toStdString())(Xmms::bind(&DataBackend::scrapResult, conn));
	}

}

void DropTreeWidget::numSongs(QString path) {
	QDir dir(path);
	QStringList filters;
	filters << "*mp3*" << "*m4a*";
	dir.setNameFilters(filters);
	dir.setFilter(QDir::AllDirs|QDir::Files|QDir::NoDotAndDotDot);
	QFileInfoList list(dir.entryInfoList());

	for(int i=0;i<list.size();i++) {
		if(list.value(i).isDir()) 
		numSongs(list.value(i).filePath());
		else
		lib->total += 1;
	}

}

DropTreeWidget::~DropTreeWidget() {
//CODE THIS!!!
}

void DropTreeWidget::dragEnterEvent(QDragEnterEvent *event) {
	event->acceptProposedAction();
}

//Nothing to do here
void DropTreeWidget::dragMoveEvent ( QDragMoveEvent * event) {
}


//Time to actually do some shite
void DropTreeWidget::dropEvent(QDropEvent *event){
// //std::cout<<"dragging"<<std::endl;
	path->clear();
	QList<QUrl> list = event->mimeData()->urls();
	
	path->append(list.value(0).toString());
	
	if(path->startsWith("file://")) {
	path->remove(0,7);
	}

	QModelIndex indice(dirModel->index(*path));
		if(!dirModel->isDir(indice)) {
			if(!path->startsWith("file://"))
			path->prepend("file://");
// 		//std::cout<<path->toStdString()<<std::endl;
		conn->medialib.addEntry(path->toStdString())(Xmms::bind(&DataBackend::scrapResult, conn));
		}
		else {
		numSongs(*path);
		QDir dir(*path);
		QStringList filters;
		filters << "*mp3*" << "*m4a*";
		dir.setNameFilters(filters);
		dir.setFilter(QDir::AllDirs|QDir::Files|QDir::NoDotAndDotDot);
		QFileInfoList list(dir.entryInfoList());
		

		for(int i=0;i<list.size();i++) {
		recurAdd(list.value(i).filePath(),list.value(i).isDir());
		}
	
		//conn->medialib.pathImport(path->toStdString())(Xmms::bind(&MediaLib::doneWithTask, lib));
		}
}

ComplexSearchDialog::ComplexSearchDialog(DataBackend* conn) {
	QStringList temp;
	this->conn = conn;
	itemList = new QTreeWidget();
	temp<<"Attribute"<<"Operator"<<"Value"<<"Appendage Type"<<"Not?";
	itemList->setHeaderLabels(temp);

	tagLabel = new QLabel("Attribute:");
	tag = new QComboBox();
	temp.clear();
	temp = ((MlibData*)(conn->getDataBackendObject(DataBackend::MLIB)))->getStandardTags();
	tag->addItems(temp);

	operLabel = new QLabel("Operator:");
	oper = new QComboBox();
	temp.clear();
	temp<<"="<<"matches"<<"has tag"<<"<"<<">"<<"<="<<">=";
	oper->addItems(temp);
	
	valueLabel = new QLabel("Value:");
	value = new QLineEdit();	
	
	appendageTypeLabel = new QLabel("Appendage Type:");
	appendageType = new QComboBox();
	temp.clear();
	temp<<"AND"<<"OR";
	appendageType->addItems(temp);
	
	notCheck = new QCheckBox("Inverse");

	add = new QPushButton("Append");
	
	buttons = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	buttons->setCenterButtons(1);
	
	delItem = new QShortcut(Qt::Key_Delete,this,SLOT(removeOperand()),SLOT(removeOperand()));

	connect(buttons,SIGNAL(accepted()),this,SLOT(accept()));
	connect(buttons,SIGNAL(rejected()),this,SLOT(reject()));
	connect(add,SIGNAL(clicked()),this,SLOT(addOperand()));

	layout = new QGridLayout();
	
	layout->addWidget(itemList,0,0,5,2);
	layout->addWidget(tagLabel,0,2);
	layout->addWidget(tag,0,3);
	layout->addWidget(operLabel,1,2);
	layout->addWidget(oper,1,3);
	layout->addWidget(valueLabel,2,2);
	layout->addWidget(value,2,3);
	layout->addWidget(appendageTypeLabel,3,2);
	layout->addWidget(appendageType,3,3);
	layout->addWidget(notCheck,4,2);
	layout->addWidget(add,4,3);
	layout->addWidget(buttons,5,0);
	this->setLayout(layout);

}

void ComplexSearchDialog::addOperand() {
	if(value->text()=="" && oper->currentText()!="has tag") {
		std::cout<<oper->currentText().toStdString()<<std::endl;
		return;
	}
	else {
		QTreeWidgetItem* item = new QTreeWidgetItem;
		item->setText(0,tag->currentText());
		item->setText(1,oper->currentText());
			if(oper->currentText()!="has tag") {
			item->setText(2,value->text());
			}
		item->setText(3,appendageType->currentText());
			if(notCheck->checkState()==Qt::Checked)
			item->setText(4,"1");
			else
			item->setText(4,"0");

		itemList->addTopLevelItem(item);
			Operator temp;
			if(appendageType->currentText()=="OR")
			temp = opor;
			else
			temp = opand;
		complexSearchItems.append(QPair <Xmms::Coll::Coll*, Operator> (newColl(tag->currentText(),oper->currentText(),
		value->text(),notCheck->checkState()==Qt::Checked),temp));
	}

}

void ComplexSearchDialog::removeOperand() {
	std::cout<< complexSearchItems.size()<<std::endl;
	QList<QTreeWidgetItem*> toDel = itemList->selectedItems();
	QList< QPair <Xmms::Coll::Coll*,Operator> > removeVals;
	int i;
		for(i=0;i<toDel.size();i++) {
		removeVals.append(complexSearchItems.value(itemList->indexOfTopLevelItem(toDel.value(i))));
		delete itemList->takeTopLevelItem(itemList->indexOfTopLevelItem(toDel.value(i)));
		}
		for(i=0;i<removeVals.size();i++) {
		complexSearchItems.removeAll(removeVals.value(i));
		}
	std::cout<< complexSearchItems.size()<<std::endl;
}

void ComplexSearchDialog::clearItems() {
	itemList->clear();
	complexSearchItems.clear();
}

Xmms::Coll::Coll* ComplexSearchDialog::newColl(QString attr,QString oper,QString val,bool notFlag) {
	Xmms::Coll::Universe univ;
	Xmms::Coll::Coll* resultColl;
		
		if(oper == "=") {
		resultColl = new Xmms::Coll::Equals(univ,attr.toStdString(),val.toStdString(),true);
		}
		else if(oper == "matches") {
		std::string tmp = val.toStdString();
		tmp = "%"  + tmp + "%";
		resultColl = new Xmms::Coll::Match(univ,attr.toStdString(),tmp,false);
		}
		else if(oper == "has tag") {
		resultColl = new Xmms::Coll::Has(univ,attr.toStdString());
		}
		else if(oper == "<") {
		resultColl = new Xmms::Coll::Smaller(univ,attr.toStdString(),val.toStdString());
		}
		else if(oper ==">") {
		resultColl = new Xmms::Coll::Greater(univ,attr.toStdString(),val.toStdString());
		}
		else if(oper =="<=") {
		Xmms::Coll::Smaller tmp1(univ,attr.toStdString(),val.toStdString());
		Xmms::Coll::Equals tmp2(univ,attr.toStdString(),val.toStdString());
		Xmms::Coll::Union * un = new Xmms::Coll::Union();
		un->addOperand(tmp1); un->addOperand(tmp2);
		resultColl = un;
		}
		else if(oper == ">=") {
		Xmms::Coll::Greater tmp1(univ,attr.toStdString(),val.toStdString());
		Xmms::Coll::Equals tmp2(univ,attr.toStdString(),val.toStdString());
		Xmms::Coll::Union * un = new Xmms::Coll::Union();
		un->addOperand(tmp1); un->addOperand(tmp2);
		resultColl = un;
		}
		else return NULL;
	
		if(notFlag) {
		Xmms::Coll::Complement* comp = new Xmms::Coll::Complement();
		comp->setOperand(*resultColl);
		return comp;
		}

	return resultColl;
}

void ComplexSearchDialog::accept() {
	emit newList(complexSearchItems);
	QDialog::accept();
}

#endif


