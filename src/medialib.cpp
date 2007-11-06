#ifndef MEDIALIB_CPP
#define MEDIALIB_CPP
#include "medialib.h"
#include <iostream>


MediaLib::MediaLib(DataBackend * conn,  QWidget * parent, Qt::WindowFlags f):QWidget(parent, f)
	{
	xmms = conn;
	complexSearch = false;
	xmms->medialib.broadcastEntryChanged()(Xmms::bind(&MediaLib::mlibChanged, this));
	fromKonfetka = false;
	layout = new QGridLayout();
	delItem = new QShortcut(Qt::Key_Delete,this,SLOT(slotRemove()),SLOT(slotRemove()));

	QStringList Hlabels;
	Hlabels << "Your MediaLib";
	mediaList = new DropTreeWidget(this);
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

	makeColl = new QPushButton();
	makeColl->setIcon(QIcon(":images/playlist"));
	makeColl->setToolTip("Make Collection from Selected Items (Not yet implemented)");	
	makeColl->setFixedSize(32,32);

	searchLine = new QLineEdit();
	searchLabel = new QLabel("Search");
	complexSearchButton = new QPushButton("Simple");
	complexSearchButton->setFixedSize(32,25);

	fileList = new QTreeView();
	dirModel = new QDirModel();
	fileList-> setDragDropMode(QAbstractItemView::DragDrop);

	QStringList filters;
	filters << "*mp3*" << "*m4a*";
	dirModel->setNameFilters(filters);
	dirModel->setFilter(QDir::AllDirs|QDir::Files|QDir::NoDotAndDotDot);
	fileList->setModel(dirModel);
	for(int i =1;i<4;i++)
	fileList->setColumnHidden(i,1);

	buttons = new QVBoxLayout();	
	layout->addWidget(searchLine,0,0,1,2);
	layout->addWidget(mediaList,1,0,1,1);
	layout->addWidget(fileList,1,1,1,1);
	layout->addWidget(complexSearchButton,0,2,1,1,Qt::AlignHCenter);
	buttons->addWidget(add);
	buttons->addWidget(update);
	buttons->addWidget(makeColl);
	layout->addLayout(buttons,1,2,1,1);
	layout->setSpacing(5);


	setLayout(layout);

	fileList->hide();


	connect(add,SIGNAL(clicked()),this,SLOT(toggleFileList()));
	connect(update,SIGNAL(clicked()),this,SLOT(refreshList()));
	connect(searchLine,SIGNAL(textChanged(QString)),this,SLOT(searchMlib(QString)));
	
	connect(mediaList,SIGNAL(itemPressed(QTreeWidgetItem *,int)),this,SLOT(addToMlibDrag(QTreeWidgetItem*,int))); 
	connect(mediaList,SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)),this,SLOT(addToMlibDoubleClick(QTreeWidgetItem*,int)));
	connect(mediaList,SIGNAL(itemClicked(QTreeWidgetItem *,int)),this,SLOT(stopTimerAndClearList()));	

	connect(&doubleClickTimer,SIGNAL(timeout()),this,SLOT(startDrag()));
	
	//New List thingy
	connect(conn->getDataBackendObject(DataBackend::MLIB),SIGNAL(gotListFromServer(QString, QList<QString>)),
					this,SLOT(gotNewList(QString, QList<QString>)));
	connect(conn->getDataBackendObject(DataBackend::MLIB),SIGNAL(infoChanged(int)),
					this,SLOT(infoChanged(int)));
	connect(mediaList,SIGNAL(itemExpanded(QTreeWidgetItem*)),this,SLOT(itemExpanded(QTreeWidgetItem*)));
	visibleMedia = new Xmms::Coll::Universe();
	((MlibData*)(conn->getDataBackendObject(DataBackend::MLIB)))->getListFromServer(visibleMedia,"artist");
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
	delete fileList;
	delete dirModel;
}


void MediaLib::gotNewList(QString property, QList<QString> info) {
	if(property == "artist")
	artistList(info);
	else
	std::cout<<"Don't support anything but artist right now"<<std::endl;
}
void MediaLib::artistList(QList<QString> info) {
	mediaList->clear();
		for(int i=0;i<info.size();i++) {
		QTreeWidgetItem * newItem = new QTreeWidgetItem();
		newItem->setText(0,info.value(i));
		mediaList->addTopLevelItem(newItem);
		QTreeWidgetItem * waitItem = new QTreeWidgetItem(newItem);
		waitItem->setText(0,"...");
		}
	mediaList->sortItems(0,Qt::AscendingOrder);
	std::cout<<"MLIB TEST DONE"<<std::endl;
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
	Xmms::Coll::Equals albumItems(*visibleMedia,"artist",item->text(0).toStdString(),true);

	std::list<std::string> what;
 	what.push_back("album");
	what.push_back("url");

	xmms->collection.queryInfos(albumItems,what)(boost::bind(&MediaLib::gotAlbums,this,item,_1));
}

bool MediaLib::gotAlbums(QTreeWidgetItem* artist,const Xmms::List <Xmms::Dict> &list) {
	artist->takeChildren();
	QString tmp;
	bool unknownFound=false;
		for (list.first();list.isValid(); ++list) {
			if(list->contains("album")) {
			tmp = QString((list->get<std::string>("album")).c_str());
			}
			else if(list->contains("url")&&!unknownFound) {
			tmp = "Unknown";
			unknownFound=true;
			}
			else {
			continue;
			}
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
	Xmms::Coll::Equals::Equals albumItems(*visibleMedia,"artist",item->parent()->text(0).toStdString(),true);
		if(item->text(0)!="Unknown") {
		Xmms::Coll::Equals::Equals songItems(albumItems,"album",item->text(0).toStdString(),true);
		xmms->collection.queryIds(songItems)(boost::bind(&MediaLib::gotSongs,this,item,_1));
		}
		else {
		Xmms::Coll::Has artColl(*visibleMedia,"album");
		Xmms::Coll::Complement notColl;
		notColl.setOperand(artColl);
		Xmms::Coll::Intersection songItems;
		songItems.addOperand(albumItems);
		songItems.addOperand(notColl);
		xmms->collection.queryIds(songItems)(boost::bind(&MediaLib::gotSongs,this,item,_1));
		}
}

bool MediaLib::gotSongs(QTreeWidgetItem* album,const Xmms::List <uint> &list) {
	std::cout<<album->text(0).toStdString()<<std::endl;
	album->takeChildren();
	MediaItem* temp;
		for (list.first();list.isValid(); ++list) {
			temp = ((MlibData*)(xmms->getDataBackendObject(DataBackend::MLIB)))->getItem(*list);
			if(temp == NULL) {
			temp = new MediaItem();
			temp->setText(0,"");
			album->addChild(temp);
			}
			else {
			album->addChild(temp);
			}
			idToMediaItem.insert(*list,temp);
		}
		
		if(album->childCount()==0) {
		delete album;
		}
	return true;
}

void MediaLib::infoChanged(int id) {
	if(idToMediaItem.contains(id)) {
		idToMediaItem.value(id)->parent()->addChild(((MlibData*)(xmms->getDataBackendObject(DataBackend::MLIB)))->getItem(id));
		delete(idToMediaItem.value(id));
		idToMediaItem.value(id)->parent()->sortChildren(0,Qt::AscendingOrder);
	}
}

void MediaLib::slotRemove() {/*
	if(!mediaList->hasFocus()) return;
	QList<QTreeWidgetItem *> selected;
	QList<MediaItem*> relevant;
	selected = mediaList->selectedItems();
		for(int i=0;i<selected.size();i++) {
		relevant = relevant + (selectedMediaItems(selected.value(i)));
		}
	relevant.removeAll(NULL);*/
}


bool MediaLib::mlibChanged(const unsigned int& id){
	//Make sure that this song is in the list, but if this is an update ... dont
	adding=true;
		if(fromKonfetka) {
		}
		else {
		numDone = 0; total = 1;
		}
	return 1;
}

void MediaLib::insertAnyways() {
	total = numDone;
	fromKonfetka=false;
	numDone = 0;
}

void MediaLib::refreshList() {

}

//Can't get this darned thing to work.
// bool MediaLib::getArtists(const Xmms::List <Xmms::Dict> &list) {
// 	std::cout<<"here"<<std::endl;
// 	std::string str;
// 	int i =0;
// 	for (list.first();list.isValid(); ++list) {
// 	std::cout<<i<<std::endl; i+=1;
// 	str = list->get<std::string>("artist");
// 	std::cout<<str<<std::endl;
// 	}
// 	return 1;
// }

void MediaLib::toggleFileList() {
	if(fileList->isVisible()) {
	fileList->hide();
	add->setToolTip("Show The File Chooser");
	}
	else {
	fileList->show();
	add->setToolTip("Hide The File Chooser");
	}
}

void MediaLib::newColl() {
}

void MediaLib::searchMlib(QString text) {
	
}

void MediaLib::formatNode(QTreeWidgetItem * item,QFile * file,int depth) {
	QTextStream out(file);
	if(item->childCount()==0) {
	MediaItem* newItem = dynamic_cast<MediaItem *> (item);
		if(newItem!=NULL) {
		out<< depth;
		QHash<QString,QVariant> songInfo = newItem->allInfo();
		QHash<QString, QVariant>::const_iterator i;
			for(i=songInfo.constBegin(); i != songInfo.constEnd(); ++i) {
				out<<" "<<i.key()<<":"<<i.value().toString()<<"|";
			}
		out<<"\n";
		}
	}
	else {
	out<< depth<<" "<<item->text(0)<<"\n";
	}
	out.flush();
		for(int i=0;i<item->childCount();i++) {
		formatNode(item->child(i),file,depth+1);
		}
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
				moreItems.append((((MediaItem*)(what.value(i)))->info("id")).toUInt());
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
	QList<QTreeWidgetItem *> what;
// 	QList<MediaItem*> relevant;
	what =mediaList->selectedItems();
	Xmms::Coll::Union * media = selectedAsColl();
	
	std::list<std::string> tmp;
 	tmp.push_back("url");
	xmms->collection.queryInfos(*media,tmp)(Xmms::bind(&MediaLib::addToPlaylistFromCollectionDrag,this));
// 	relevant.removeAll(NULL);
// 	urlList.clear();
// 		for(int j=0;j<relevant.size();j++) {
// 		QString tmp = relevant.value(j)->info("url").toString();
// 		urlList.append(QUrl(tmp));
// 		}
// 	doubleClickTimer.start(qApp->doubleClickInterval());
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
	doubleClickTimer.stop();
}

void MediaLib::startDrag() {
	drag = new QDrag(this);
	mimeData = new QMimeData;
	mimeData->setUrls(urlList);
	drag->setMimeData(mimeData);
	drag->setPixmap(*new QPixmap(":images/volume_button_sound"));
	std::cout<<urlList.size()<<std::endl;
	urlList.clear();
	drag->start();
	doubleClickTimer.stop();
}

void MediaLib::stopTimerAndClearList() {
	doubleClickTimer.stop();
	urlList.clear();
}

DropTreeWidget::DropTreeWidget(MediaLib* newLib):QTreeWidget() {
	setDragDropMode(QAbstractItemView::DropOnly);
	setAcceptDrops(true);
	lib = newLib;
	path = new QString();
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
	lib->fromKonfetka = true;
	lib->xmms->medialib.addEntry(file.toStdString())(Xmms::bind(&DataBackend::scrapResult, lib->xmms));
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
	if(event->source() == lib->fileList)
	event->acceptProposedAction();
}

//Nothing to do here
void DropTreeWidget::dragMoveEvent ( QDragMoveEvent * event) {
}


//Time to actually do some shite
void DropTreeWidget::dropEvent(QDropEvent *event){
// //std::cout<<"dragging"<<std::endl;
	lib->total = 0;
	lib->numDone=0;
	path->clear();
	QList<QUrl> list = event->mimeData()->urls();
	
	path->append(list.value(0).toString());
	
	if(path->startsWith("file://")) {
	path->remove(0,7);
	}

	QModelIndex indice(this->lib->dirModel->index(*path));
		if(!this->lib->dirModel->isDir(indice)) {
			if(!path->startsWith("file://"))
			path->prepend("file://");
// 		//std::cout<<path->toStdString()<<std::endl;
		lib->fromKonfetka = true;
		lib->total=1;
		lib->xmms->medialib.addEntry(path->toStdString())(Xmms::bind(&DataBackend::scrapResult, lib->xmms));
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
	
		//lib->xmms->medialib.pathImport(path->toStdString())(Xmms::bind(&MediaLib::doneWithTask, lib));
		}
}


#endif


