#ifndef MEDIALIB_CPP
#define MEDIALIB_CPP
#include "medialib.h"
#include <iostream>


MediaLib::MediaLib(DataBackend * conn,  QWidget * parent, Qt::WindowFlags f):QWidget(parent, f)
	{
	mlibHashPath=Xmms::getUserConfDir().c_str();
	mlibHashPath.append("/clients/konfetka/mlibStore.txt");

	xmms = conn;
	complexSearch = false;
	xmms->medialib.broadcastEntryChanged()(Xmms::bind(&MediaLib::mlibChanged, this));
	fromKonfetka = false;
	layout = new QGridLayout();
	lookUps<<"artist"<<"album"<<"url"<<"encodedurl"<< "title"<<"genre"<<"duration"<<"timesplayed"<<"rating"<<"lastplayed"<<"id";
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

	bar = new QProgressBar;
	bar->setRange(0,100);
	bar->setOrientation ( Qt::Horizontal );
	bar->hide();
	
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
	layout->addWidget(bar,2,0,1,2);
	layout->addLayout(buttons,1,2,1,1);
	layout->setSpacing(5);


	setLayout(layout);

	fileList->hide();


	connect(add,SIGNAL(clicked()),this,SLOT(toggleFileList()));
	connect(update,SIGNAL(clicked()),this,SLOT(getSongList()));
	connect(searchLine,SIGNAL(textChanged(QString)),this,SLOT(searchMlib(QString)));
	
	connect(mediaList,SIGNAL(itemPressed(QTreeWidgetItem *,int)),this,SLOT(addToMlibDrag(QTreeWidgetItem*,int))); 
	connect(mediaList,SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)),this,SLOT(addToMlibDoubleClick(QTreeWidgetItem*,int)));
	connect(mediaList,SIGNAL(itemClicked(QTreeWidgetItem *,int)),this,SLOT(stopTimerAndClearList()));	

	connect(&insertTimer,SIGNAL(timeout()),this,SLOT(insertAnyways()));
	connect(&doubleClickTimer,SIGNAL(timeout()),this,SLOT(startDrag()));
	
	//New List thingy
	connect(conn->getDataBackendObject(DataBackend::MLIB),SIGNAL(gotListFromServer(QString, QList<QString>)),
					this,SLOT(gotNewList(QString, QList<QString>)));
	connect(conn->getDataBackendObject(DataBackend::MLIB),SIGNAL(infoChanged(int)),
					this,SLOT(infoChanged(int)));
	connect(mediaList,SIGNAL(itemExpanded(QTreeWidgetItem*)),this,SLOT(itemExpanded(QTreeWidgetItem*)));
	visibleMedia = new Xmms::Coll::Universe();
	((MlibData*)(conn->getDataBackendObject(DataBackend::MLIB)))->getListFromServer(visibleMedia,"artist");
	updating=false;adding=false;


// 	loadFromFile();
}

MediaLib::~MediaLib() {
	saveToFile();
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
void MediaLib::albumList(QTreeWidgetItem* artist,QList<QString> info) {
}

void MediaLib::songList(QTreeWidgetItem* album,QList<QString> info){
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
	Xmms::Coll::Equals::Equals albumItems(*visibleMedia,"artist",item->text(0).toStdString(),true);

	std::list<std::string> what;
 	what.push_back("album");

	xmms->collection.queryInfos(albumItems,what)(boost::bind(&MediaLib::gotAlbums,this,item,_1));
}

bool MediaLib::gotAlbums(QTreeWidgetItem* artist,const Xmms::List <Xmms::Dict> &list) {
	artist->takeChildren();
		for (list.first();list.isValid(); ++list) {
			if(list->contains("album")) {
			QTreeWidgetItem * newAlbum = new QTreeWidgetItem(artist);
			newAlbum->setText(0,(QString((list->get<std::string>("album")).c_str())));
			QTreeWidgetItem * tempSong = new QTreeWidgetItem(newAlbum);
			tempSong->setText(0,"...");
			}
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
	return true;
}

void MediaLib::infoChanged(int id) {
	if(idToMediaItem.contains(id)) {
		idToMediaItem.value(id)->parent()->addChild(((MlibData*)(xmms->getDataBackendObject(DataBackend::MLIB)))->getItem(id));
		delete(idToMediaItem.value(id));
		idToMediaItem.value(id)->parent()->sortChildren(0,Qt::AscendingOrder);
	}
}

void MediaLib::slotRemove() {
	if(!mediaList->hasFocus()) return;
	QList<QTreeWidgetItem *> what;
	what = mediaList->selectedItems();
		for(int i=0;i<what.size();i++) {
		if(!what.value(i)->isHidden()) {
		process(what.value(i),2);
		removeNode(what.value(i));
		}
	}
	saveToFile();
}

bool MediaLib::mlibChanged(const unsigned int& id){
	//Make sure that this song is in the list, but if this is an update ... dont
	adding=true;
	if(!updating) {
		if(fromKonfetka) {
		pushId(id);
		}
		else {
		numDone = 0; total = 1;
		xmms->medialib.getInfo(id)(Xmms::bind(&MediaLib:: getMediaInfo,this));
		}
	}
	return 1;
}

void MediaLib::pushId(int id) {
	if(!insertIds.contains(id)) {
	insertIds.push(id);
	insertTimer.stop();
	insertTimer.start(2000);
	numDone+=1;
// 	//std::cout<<numDone<<" "<<total<<std::endl;
	}
	if(numDone == total) {
	numDone = 0;
	fromKonfetka=false;
	insertStackedIds();
	}
}

void MediaLib::insertStackedIds() {
	while(!insertIds.isEmpty()) {
	xmms->medialib.getInfo(insertIds.pop())(Xmms::bind(&MediaLib:: getMediaInfo,this));
	}
}

void MediaLib::insertAnyways() {
	total = numDone;
	fromKonfetka=false;
	numDone = 0;
	insertTimer.stop();
	insertStackedIds();
}

void MediaLib::getSongList() {
	updating=true;
	mediaList->setSortingEnabled (0);
	Xmms::Coll::Universe mlib;
	
	//Thanks to Esperanza for the order char array (im too lazy to think of it myself ...)
	static const char *o[] = {"artist", "album", "tracknr", "id"};
	std::list<std::string> order (o, o+4);
	
	xmms->collection.queryIds(mlib,order)(Xmms::bind (&MediaLib::getSongIds,this));
	//This might be a faster way of doing it, I dont know....
// 	std::list<std::string> what;
// 	what.push_back("artist");
// 	std::cout<<"getting"<<std::endl;
// 	xmms->collection.queryInfos(mlib,what,what,0,0,what)(Xmms::bind (&MediaLib::getArtists,this));

	//Decaprecated by collections
	//xmms->medialib.select("select * from Media where key='artist' or key = 'album'")(Xmms::bind (&MediaLib::getSongIds<<this));
	mediaList->setUpdatesEnabled(true);
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

bool MediaLib::getSongIds(const Xmms::List<unsigned int> &list) {
	numDone=0;
	total=0;
	mediaList->clear();
	mediaList->setUpdatesEnabled(0);
	artists.clear();
	titles.clear();	
	songs.clear();

	for (list.first();list.isValid(); ++list) {
	total+=1;
	}

	for (list.first();list.isValid(); ++list) {
	xmms->medialib.getInfo(*list)(Xmms::bind(&MediaLib:: getMediaInfo,this));
	}
	return 1;
}

bool MediaLib::getMediaInfo(const Xmms::PropDict& info) {
	return true;
	MediaItem * newItem = new MediaItem();
	std::cout<<sizeof(*newItem)<<std::endl;
	QHash<QString,QVariant> curInfo;
	int tmpInt;
	std::string tmpString;
	
	for(int i=0;i<lookUps.size();i++) {
		if(info.contains(lookUps.value(i).toStdString()) || lookUps.value(i)=="encodedurl") {
			try {
				if(lookUps.value(i)=="encodedurl")
				tmpString = info.get<std::string>("url");
				else
				tmpString = info.get<std::string>(lookUps.value(i).toStdString());

				if(lookUps.value(i)=="url")
				tmpString = QString::fromUtf8 (xmmsc_result_decode_url (NULL,tmpString.c_str())).toStdString();
			curInfo.insert(lookUps.value(i),QVariant(tmpString.c_str()));
			} catch(Xmms::wrong_type_error& err) {
			tmpInt = info.get<int>(lookUps.value(i).toStdString());
				if(lookUps.value(i)=="duration") {
				QString tmp;
				QTime foo (0,0,0);
				foo=foo.addMSecs(tmpInt);
				if(foo.hour()>0)
					tmp =foo.toString("h:mm:ss");
				else
					tmp =foo.toString("mm:ss");
				curInfo.insert(lookUps.value(i),QVariant(tmp));
				}
				else {
				curInfo.insert(lookUps.value(i),QVariant(tmpInt));
				}
			}
		} else {
			if(lookUps.value(i) == "title")
			curInfo.insert(lookUps.value(i),curInfo.value("url"));
		curInfo.insert(lookUps.value(i),QVariant("Unknown"));
		}
	}
	newItem->setInfo(curInfo);
	songs.append(newItem);
	if(songs.size()==total && update) {
		QTimer::singleShot(100, this, SLOT(insertNextFew()));
	}
	else
	insert(newItem);
	return 1;
}

void MediaLib::insertNextFew() {
// 	qApp->processEvents();
	int step = total/100;
	int goTo = numDone + step;
	while(numDone<songs.size() && numDone <= goTo) {
		insert(songs.value(numDone));
		numDone+=1;
	showProgress(numDone,total);
		if(numDone==songs.size()-1) {
		showProgress(total,total);
		return;
		}
	} 
	if(numDone<songs.size()) {
		QTimer::singleShot(100, this, SLOT(insertNextFew()));
	}
		
}

void MediaLib::insert(MediaItem* itm) {
	QString artist = itm->info("artist").toString().trimmed();
	QString album = itm->info("album").toString().trimmed();
	QString title = itm->info("title").toString().trimmed();

	QString combo = artist + album + title;
	if(!adding) {
		if(titles.contains(combo)) {
// 		numDone+=1;
// 		showProgress(numDone,total);
		return;
		}
	}
	if(adding && titles.contains(combo)) {
// 	//std::cout<<artist.toStdString()<<std::endl;
	MediaItem* oldItem = titles.value(combo);
	if(oldItem)
	oldItem->setInfo(itm->allInfo());
	return;
	}
	titles.insert(combo,itm);

	QTreeWidgetItem * treeArt = NULL;
	QTreeWidgetItem * treeAlb = NULL;
		if(artists.contains(artist) && artists.value(artist)) {
		treeArt=artists.value(artist);
// 		//std::cout<<"using artist"<<std::endl;
		}
		else {
		treeArt = new QTreeWidgetItem(mediaList);
		treeArt->setText(0,artist);
		}
		
		for(int i=0;i<treeArt->childCount();i++) {
			if(treeArt->child(i)->text(0)==album)
			treeAlb=treeArt->child(i);
		}
		if(treeAlb==NULL) {
		treeAlb = new QTreeWidgetItem(treeArt);
		treeAlb->setText(0,album);
		}
	itm->setText(0,title);
	treeAlb->insertChild(0,itm);
	if(!update)
	songs.append(itm);
	artists.insert(artist,treeArt);
// 	numDone+=1;
// 	if(!adding);
// 	showProgress(numDone,total);
	adding=false;
}

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

void MediaLib::showProgress(int num,int len){
	bar->show();
	bar->setValue((int)(100.0*((double)num/(double)len)));

	if(num>=len) {
	numDone=0;
	total=0;
	mediaList->setUpdatesEnabled(1);
	mediaList->setSortingEnabled (0);
	mediaList->sortItems(0,Qt::AscendingOrder);
	saveToFile();
	updating=false;
	bar->hide();
	}
}

void MediaLib::searchMlib(QString text) {
	QFont f = QApplication::font();


	QTreeWidgetItem* curItem;
	for(int i=0;i<matchedSongs.size();i++) {
		for(int j=0;j<matchedSongs.value(i)->columnCount();j++) {
		matchedSongs.value(i)->setFont(j,f);
		}
		curItem = matchedSongs.value(i)->parent();
			while(curItem!=NULL) {
				curItem->setFont(0,f);
				curItem=curItem->parent();
			}
			
	}

	if(text.isEmpty()) {
	for(int i=0;i < mediaList->topLevelItemCount();i++) {
	mediaList->topLevelItem(i)->setHidden(0);
	}
	matchedSongs.clear();
	return;
	}

	f.setBold(true);
	for(int i=0;i < mediaList->topLevelItemCount();i++) {
	mediaList->topLevelItem(i)->setHidden(1);
	}
	QString search;
	if(text.indexOf(":") != -1) {
	search = text.mid(0,text.indexOf(":"));
	text = text.mid(text.indexOf(":")+1).trimmed();
	}
	else {
	search = "all";
	}
	for(int i=0;i<songs.size();i++) {
		if(songs.value(i)->matches(search,text)) {
			songs.value(i)->setFont(0,f);
			songs.value(i)->setHidden(0);
			matchedSongs.append(songs.value(i));
			curItem = songs.value(i)->parent();
			while(curItem!=NULL) {
				curItem->setFont(0,f);
				curItem->setHidden(0);
				curItem=curItem->parent();
			}
		}
	}

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


void MediaLib::saveToFile() {
	QFile file(mlibHashPath);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
		{
		std::cout<<mlibHashPath.toStdString()<<std::endl;
		return;
		}
		QTreeWidgetItem * curItem;
		for(int i= 0;i < mediaList->topLevelItemCount();i++) {
		curItem = mediaList->topLevelItem(i);
		formatNode(curItem,&file,1);
		}
	file.flush(); file.close();
}

MediaItem* MediaLib::loadItem(QString str) {
	MediaItem * newItem = new MediaItem();
	QStringList info = str.split("|");
	QString tmp;QString key; QString val;
	
	//the item was stored with | seperating everything, so split on it and then find the info
	try {
	QHash<QString,QVariant> hash;
	for(int i=0;i<info.size();i++) {
	tmp= info.value(i);
		if(i==0) {
		tmp = info.value(i).mid(2);
		}
		key = tmp.mid(0,tmp.indexOf(":")).trimmed();
		val = tmp.mid(tmp.indexOf(":")+1).trimmed();
		
		if(!key.isEmpty()) {
		hash.insert(key,QVariant(val));
		}
	}
	newItem->setText(0,hash.value("title").toString());
	newItem->setInfo(hash);
	songs.append(newItem);
	}catch(...){}
	return newItem;
}

void MediaLib::loadFromFile() {
	QFile file(mlibHashPath);
		if(!file.open(QIODevice::ReadWrite)) {
		return;
		}
	
	QTreeWidgetItem * topLevel;
	QTreeWidgetItem * secondLevel;
	MediaItem * thirdLevel;
	
	while (!file.atEnd()) {
		try {
		QTreeWidgetItem *item;
		QString line = file.readLine();
			if(line.startsWith("1 ")) {
			topLevel = new QTreeWidgetItem(mediaList);
			topLevel->setText(0,line.simplified().mid(2));
			mediaList->addTopLevelItem(item);
			}
			else if(line.startsWith("2 ")) {
			secondLevel = new QTreeWidgetItem(topLevel);
			secondLevel->setText(0,line.simplified().mid(2));
			}
			else {
			thirdLevel = loadItem(line);
			secondLevel->insertChild(0,thirdLevel);
			titles.insert(topLevel->text(0)+secondLevel->text(0)+thirdLevel->text(0),thirdLevel);
			}
		} catch(...){}
	}
}

void MediaLib::removeNode(QTreeWidgetItem * val) {
	if(val->parent()==NULL) {
	artists.remove(val->text(0));
	delete val;
	val=0;
	return;
	}
			
	
	if(val->childCount()==0) {
		MediaItem * tmp = (MediaItem*)val;
		if(matchedSongs.contains(tmp))
		matchedSongs.removeAt(matchedSongs.indexOf(tmp));
	}

	QTreeWidgetItem * itm1 = val->parent();
		if(itm1!=NULL && itm1->childCount()==1) {
		QTreeWidgetItem * itm2 = itm1->parent();
			if(itm2!=NULL && itm2->childCount()==1) {
			delete itm2;
			itm2=0;
			return;
			}
			else {
			delete itm1;
			itm1=0;
			return;
			}
		}
	delete val;
	val=0;

}

void MediaLib::process(QTreeWidgetItem* itm,int type) {
	if(itm == NULL) return;
	if(itm->childCount()==0) {
	MediaItem* newItem = dynamic_cast<MediaItem *> (itm);
		if(newItem!=NULL && type==0) {
		QString tmp = newItem->info("url").toString();
		urlList.append(QUrl(tmp));
		}
		else if(newItem!=NULL && type==1) {
		}
		else if(newItem!=NULL && type==2){
		xmms->medialib.removeEntry(newItem->info("id").toInt())(Xmms::bind(&DataBackend::scrapResult,xmms));	
		songs.removeAt(songs.indexOf(newItem));
			if(matchedSongs.contains(newItem))
			matchedSongs.removeAt(matchedSongs.indexOf(newItem));
			QString combo = newItem->info("artist").toString() + newItem->info("album").toString() + newItem->info("title").toString(); 
			if(titles.contains(combo))
			titles.remove(combo);
		}
	}
	else {
		for(int i=0;i<itm->childCount();i++) {
		process(itm->child(i),type);
		}
	}
}

void MediaLib::addToMlibDrag(QTreeWidgetItem*,int) {
	urlList.clear();
	QList<QTreeWidgetItem *> what;
	what =mediaList->selectedItems();
	for(int i=0;i<what.size();i++) {
	if(!what.value(i)->isHidden())
	process(what.value(i),0);
	}
	
	doubleClickTimer.start(qApp->doubleClickInterval());
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


