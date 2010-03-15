#ifndef MEDIALIB_CPP
#define MEDIALIB_CPP
#include "medialib.h"
#include <iostream>


MediaLib::MediaLib(DataBackend * c,  QWidget * parent, Qt::WindowFlags f):LayoutPanel(parent, f)
	{
	conn = c;
	mlib = (MlibData*)(conn->getDataBackendObject(DataBackend::MLIB));
	coll = (CollData*)(conn->getDataBackendObject(DataBackend::COLL));
	
	searchType = SIMPLE;
	layout = new QGridLayout();

	QStringList Hlabels;
	Hlabels << "Your MediaLib";
	mediaList = new DropTreeWidget(this,c);
	mediaList->setHeaderLabels(Hlabels);
	mediaList->setEditTriggers(QAbstractItemView::NoEditTriggers);
	mediaList->setSelectionMode(QAbstractItemView::ExtendedSelection);
	mediaList->setDragEnabled(true);
	mediaList->setAcceptDrops(true);
	mediaList->setObjectName ("mediaList");
	

	loadUniverse= new QPushButton();
	loadUniverse->setIcon(QIcon(":images/xmms2Logo"));
	loadUniverse->setToolTip("Load Universe");
	loadUniverse->setFixedSize(32,32);

	updateAll = new QPushButton();
	updateAll->setIcon(QIcon(":images/repeat_all"));
	updateAll->setToolTip("Update Your MediaLib");
	updateAll->setFixedSize(32,32);

	QMenu * makeCollMenu = new QMenu();
	makeCollMenu->addAction("Use Visible Items",this,SLOT(useVisible()));
	makeCollMenu->addAction("Use Selected Items",this,SLOT(useSelected()));
	makeColl = new QPushButton();
	makeColl->setIcon(QIcon(":images/playlist"));
	makeColl->setToolTip("Do Stuff With Your Media");	
	makeColl->setFixedSize(32,32);
	makeColl->setMenu(makeCollMenu);

	searchLine = new QLineEdit();
	searchToggleButton = new QPushButton("Simple");
	searchToggleButton->setCheckable(true);
	searchToggleButton->setToolTip("Simple search, click for Advanced.");
	connect(searchToggleButton,SIGNAL(clicked()),this,SLOT(toggleSearchType()));

	
	infoMenu = new QMenu("Media Information");
	infoMenu->addAction("Append To Playlist",this,SLOT(appendToPlaylistContextInfo()));
	infoMenu->addAction("Remove Selected Media",this,SLOT(slotRemove()));

	QStringList filters;
	filters << "*mp3*" << "*m4a*";

	buttons = new QVBoxLayout();	
	layout->addWidget(searchLine,0,0,1,1);
	layout->addWidget(mediaList,1,0,1,1);
	layout->addWidget(searchToggleButton,0,1,1,1);
	buttons->addWidget(loadUniverse,Qt::AlignHCenter);
	buttons->addWidget(updateAll,Qt::AlignHCenter);
	buttons->addWidget(makeColl,Qt::AlignHCenter);
	layout->addLayout(buttons,1,1,1,1,Qt::AlignHCenter | Qt::AlignTop);
	layout->setSpacing(5);
	layout->setRowStretch(1,1);

	setLayout(layout);

	connect(loadUniverse,SIGNAL(clicked()),this,SLOT(loadUniv()));
	connect(updateAll,SIGNAL(clicked()),this,SLOT(refreshList()));
	connect(searchLine,SIGNAL(textEdited(QString)),this,SLOT(possiblySearchMlib()));
	connect(searchLine,SIGNAL(returnPressed()),this,SLOT(searchMlib()));
	connect(&searchTimer,SIGNAL(timeout()),this,SLOT(searchMlib()));
	
	//The pressed really just prepares the urllist, look at DropTreeWidget::mimeData for the actual dragging
	connect(mediaList,SIGNAL(itemPressed(QTreeWidgetItem *,int)),this,SLOT(addFromMlibDrag(QTreeWidgetItem*,int))); 
	connect(mediaList,SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)),this,SLOT(addFromMlibDoubleClick(QTreeWidgetItem*,int)));
	connect(mediaList,SIGNAL(removeSelected()),this,SLOT(slotRemove()));

	//New List thingy
	connect(mlib,SIGNAL(infoChanged(int)),
					this,SLOT(infoChanged(int)));
	connect(mlib,SIGNAL(updatesDone()),
					this,SLOT(checkIfRefreshIsNeeded()));
	connect(mlib,SIGNAL(periodicUpdate()),this,SLOT(respondToPeriodicUpdate()));
// 	connect(mediaList,SIGNAL(itemExpanded(QTreeWidgetItem*)),this,SLOT(itemExpanded(QTreeWidgetItem*)));

	connect(conn,SIGNAL(qsettingsValueChanged(QString,QVariant)),this,SLOT(respondToConfigChange(QString,QVariant)));

	visibleMedia = new Xmms::Coll::Universe();
	baseMedia = visibleMedia;
	lastArtist = NULL;
	lastAlbum = NULL;
	loadCollection(baseMedia);
	adding=false; dblClickAdd = true;
	searchTags<<"artist"<<"album"<<"title"<<"url"<<"genre"<<"id";

// 	loadFromFile();
}

MediaLib::~MediaLib() {
	delete mediaList;
	delete loadUniverse;
	delete updateAll;
	delete makeColl;
	delete searchLine;
	delete searchToggleButton;
}

void MediaLib::init() {
	lastArtist = NULL;
	lastAlbum = NULL;
	mediaList->clear();
	idToSongItem.clear();
	songItemToId.clear();
}

void MediaLib::toggleSearchType() {
	if(searchType == SIMPLE) {
		searchType = PARSE;
		searchToggleButton->setToolTip("Simply type <query> and press enter. <query> is\nof the same syntax as cli: \"xmms2 mlib search <query>\"");
		searchToggleButton->setText("Advanced");
	}
	else {
		searchType = SIMPLE;
		searchToggleButton->setToolTip("Simple search, click for Advanced.");
		searchToggleButton->setText("Simple");
		
	}
}

void MediaLib::loadCollection(Xmms::Coll::Coll* collToLoad) {
	init(); //make sure that we don't leave anything from last time around;
	std::list<std::string> order;
	order.push_back("artist");order.push_back("album");order.push_back("title");
	std::list<std::string> fetch;
	fetch.push_back("id");fetch.push_back("title");fetch.push_back("artist");fetch.push_back("album");fetch.push_back("picture_front");
	std::list<std::string> group;
	group.push_back("artist");group.push_back("album");group.push_back("title");
	conn->collection.queryInfos(*collToLoad,fetch,order,0,0, group)(Xmms::bind(&MediaLib::loadCollectionCallback,this));
}

bool MediaLib::loadCollectionCallback(const Xmms::List <Xmms::Dict> &list) {
	QString art,alb,titl;
	QTreeWidgetItem * temp;
	bool diffArt;
	bool diffAlb;
	Xmms::List_const_iterator_ < Xmms::Dict > iter;
	for (iter=list.begin();!iter.equal(list.end()); ++iter){
		if(iter->contains("artist")) {
			art = conn->convertBoostVariantToQString((*iter)["artist"]);
			if(lastArtist == NULL || lastArtist->text(0)!=art) {
				temp = new QTreeWidgetItem();
				temp->setText(0,art);
				mediaList->addTopLevelItem(temp);
				lastArtist = temp;
				diffArt = true;
			}
			else
				diffArt = false;
			if(iter->contains("album"))
				alb = conn->convertBoostVariantToQString((*iter)["album"]);
			else
				alb = "Unknown Album";
			if(lastAlbum == NULL || lastAlbum->text(0)!=alb || diffArt) {
				temp = new QTreeWidgetItem();
				temp->setText(0,alb);
				lastArtist->addChild(temp);
				lastAlbum = temp;
				diffAlb = true;
			}
			if(iter->contains("title")) 
				titl = conn->convertBoostVariantToQString((*iter)["title"]);
			else
				titl = "Unknown Title";
			temp  = new QTreeWidgetItem();
			temp->setText(0,titl);
			lastAlbum->addChild(temp);
			idToSongItem.insert(iter->get<int>("id"),temp);
			songItemToId.insert(temp,iter->get<int>("id"));
		}
	}
	return true;
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
	else {
// 	std::cout<<"uding selected"<<std::endl;
	Xmms::Coll::Union * tmp = new Xmms::Coll::Union(*selectedAsColl());
	coll->createCollection(*tmp,name.toStdString(),conn->collection.COLLECTIONS);
	}
}

void MediaLib::infoChanged(int id) {
	if(idToSongItem.contains(id) && idToSongItem.value(id)!=NULL) {
		QVariant tmp = mlib->getInfo(QString("title"),id);
		idToSongItem.value(id)->setText(0,tmp.toString());
			if(mlib->getInfo("status",id).toString() == "Broken") {
				for(int i=0;i<idToSongItem.value(id)->columnCount();i++) {
				idToSongItem.value(id)->setForeground(i,QBrush(QColor("grey"),Qt::SolidPattern));
			}
		}
	}
	else {
	idStack.push(id);
	}
}

void MediaLib::respondToPeriodicUpdate() {
	
	if(!(hasFocus() || mediaList->hasFocus())) {
// 	refreshList();
	}
}

void MediaLib::slotRemove() {
	if(!mediaList->hasFocus()) return;
	QList<QTreeWidgetItem *> what;
	what = mediaList->selectedItems();
	Xmms::Coll::Union * media = selectedAsColl();
	
	conn->collection.queryIds(*media)(Xmms::bind(&MediaLib::removeIds,this));
// 	doesn't actually remove squat, simply closes the parent's node
	removeNodes(what);
}

void MediaLib::removeNodes(QList<QTreeWidgetItem*> list) {
	QTreeWidgetItem* curItem;
// 	std::cout<<list.size()<<std::endl;
		for(int i=0;i<list.size();i++) {
			curItem = list.value(i);
			switch (getItemType(curItem)) {
				case ARTIST: {
					for(int i=0;i<curItem->childCount();i++) {
					if(!list.contains(curItem->child(i)))
					list.append(curItem->child(i));
					}
					break;
				}
				case ALBUM: {
					for(int i=0;i<curItem->childCount();i++) {
					if(!list.contains(curItem->child(i)))
					list.append(curItem->child(i));
					}
					break;
				}
				case SONG: {
					QTreeWidgetItem* ptr = curItem->parent();
					QTreeWidgetItem* ptr2 = curItem->parent()->parent();
					idToSongItem.remove(idToSongItem.key(curItem));
					if(ptr!=NULL)
					delete ptr->takeChild(ptr->indexOfChild(curItem));
					if(ptr->childCount()==0 && ptr2!=NULL)
					delete ptr2->takeChild(ptr2->indexOfChild(ptr));
					if(ptr2!=NULL && ptr2->childCount()==0)
					delete mediaList->takeTopLevelItem(mediaList->indexOfTopLevelItem(ptr2));
// 					std::cout<<list.value(i)->text(0).toStdString()<<std::endl;
					break;
				}
				default: {
					QTreeWidgetItem* tmp = curItem->parent();
					while(tmp!=NULL) {
					delete tmp;
					curItem = tmp;
					tmp = curItem->parent();
					}
				}
			}
		}
}
	
bool MediaLib::removeIds(const Xmms::List <int> &list) {
	Xmms::List_const_iterator_ < int > iter;
	for (iter=list.begin();!iter.equal(list.end()); ++iter){
// 	std::cout<<"Would be removing "<<*list<<std::endl;
	conn->medialib.removeEntry(*iter)(Xmms::bind(&DataBackend::scrapResult, conn));
	}
	return true;
}

void MediaLib::respondToConfigChange(QString name,QVariant value) {
	if(name == "konfetka/mlibSearchValues") {
		QStringList temp = value.toStringList();
		searchTags = temp;
	}
	else if(name == "konfetka/mlibDblClick") {
		dblClickAdd = value.toBool();
	}
	
}
	
void MediaLib::refreshList() {
	loadCollection(visibleMedia);
}


void MediaLib::checkIfRefreshIsNeeded() {
	int id;
	QTreeWidgetItem* temp;
		while(!idStack.isEmpty()) {
		id = idStack.pop();
			if(idToSongItem.contains(id)) {
			temp = idToSongItem.value(id);
			temp->setText(0,mlib->getInfo("title",id).toString());
			temp->parent()->setText(0,mlib->getInfo("album",id).toString());
			temp->parent()->parent()->setText(0,mlib->getInfo("artist",id).toString());
			}
		update();
	}
}


void MediaLib::loadUniv() {
	Xmms::Coll::Coll* temp = new Xmms::Coll::Universe();
	loadCollection(temp);
}

//Basically makes it so that the mlib search line waits 400 milliseconds and if no
//new input comes, it assumes the user wants the text to be searched
//note that if the user is doing a complex search (Parse) then pressing return is
//required
void MediaLib::possiblySearchMlib() {
	if(searchTimer.isActive()) 
		searchTimer.stop();
	if(searchType == SIMPLE)
	searchTimer.start(400);
}

//Takes advantage of the great Xmms::Collection::parse call, which is like crack
//mixed with steroids in its coolness, go theefer
void MediaLib::searchMlib() {
	searchTimer.stop();
	std::string text = searchLine->text().trimmed().toStdString();
	Xmms::Coll::Union allMatches;
		if(searchType == PARSE) {
			try {
			Xmms::CollPtr c = conn->collection.parse(text);
			allMatches.addOperand(*c);
			} catch (Xmms::collection_parsing_error c) {
			return;
			}
			visibleMedia = new Xmms::Coll::Union(allMatches);
			loadCollection(visibleMedia);
			return;
		}
	//FIXME: OR is a little iffy ... don't know why
	std::string query = "";
	foreach(QString val,searchTags) {
		val = val.toLower();
		val.remove(" ");
	query = val.toStdString()+"~"+text;
		if(text!="" && !(QString::fromUtf8(text.c_str()).endsWith("\\ "))) { //the \ creates a seg
			try {
				Xmms::CollPtr c = conn->collection.parse(query);
				allMatches.addOperand(*c);
			} catch(Xmms::collection_parsing_error c) {
				qDebug()<<"Could not parse query";
			}
		}
	}
		if(text=="")
		visibleMedia = baseMedia;
		else
		visibleMedia = new Xmms::Coll::Union(allMatches);
	qDebug()<<"Loading";
	loadCollection(visibleMedia);
}

//This just makes it easy to find whether a treeitem is toplevel(artist), midlevel(album) or lowlevel(song)
ItemType MediaLib::getItemType(QTreeWidgetItem* item) {
	if(item->parent() == NULL)
	return ARTIST;
	else if(item->parent()->parent()==NULL && item->childCount()>0)
	return ALBUM;
	else if(item->childCount()==0 && item->parent()->parent()!=NULL)
	return SONG;
	else
	return INVALID;
}

Xmms::Coll::Union* MediaLib::selectedAsColl() {
	QList<QTreeWidgetItem *> what;
	what =mediaList->selectedItems();
	Xmms::Coll::Union * media = new Xmms::Coll::Union();
		for(int i=0;i<what.size();i++) {
			switch (getItemType(what.value(i))) {
				case ARTIST: {
// 				std::cout<<"Artist "<<(what.value(i))->text(0).toStdString()<<std::endl;
				Xmms::Coll::Equals moreItems(*visibleMedia,"artist",(what.value(i))->text(0).toStdString(),true);
				media->addOperand(moreItems);
				break;
				}
				case ALBUM: {
// 				std::cout<<"Album "<<(what.value(i))->text(0).toStdString()<<std::endl;
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
					Xmms::Coll::Equals moreItems(*visibleMedia,"album",(what.value(i))->text(0).toStdString(),true);
					Xmms::Coll::Intersection albAndArt;
					albAndArt.addOperand(someItems);
					albAndArt.addOperand(moreItems);
					media->addOperand(albAndArt);
					}
					
				break;
				}
				case SONG: {
// 				std::cout<<"Song "<<(what.value(i))->text(0).toStdString()<<std::endl;
				Xmms::Coll::Idlist moreItems;
				moreItems.append(idToSongItem.key(what.value(i)));
				media->addOperand(moreItems);
				break;
				}
			}
		}
	return media;
}

void MediaLib::addFromMlibDrag(QTreeWidgetItem*,int) {
	urlList.clear();
	Xmms::Coll::Union * media = selectedAsColl();
	std::list<std::string> tmp;
 	tmp.push_back("url");
	conn->collection.queryInfos(*media,tmp)(Xmms::bind(&MediaLib::addToPlaylistFromCollectionDrag,this));
}

bool MediaLib::addToPlaylistFromCollectionDrag(const Xmms::List <Xmms::Dict> &list) {
	std::string tmpString;
	Xmms::List_const_iterator_ < Xmms::Dict > iter;
	for (iter=list.begin();!iter.equal(list.end()); ++iter){
		if(iter->contains("url")) {
		tmpString = iter->get<std::string>("url");
		//tmpString = QString::fromUtf8 (xmmsc_result_decode_url (NULL,tmpString.c_str())).toStdString();
		urlList.append(QUrl(QString(tmpString.c_str())));
// 		std::cout<<tmpString<<std::endl;
		} 
	}
	return true;
}

void MediaLib::addFromMlibDoubleClick(QTreeWidgetItem * item,int) {
	if(!dblClickAdd) return;
		for(int i=0;i<urlList.size();i++) {
		conn->playlist.addUrl(urlList.value(i).toString().toStdString());
		}
	urlList.clear();
}

void MediaLib::setLayoutSide(bool right_side) { //true=right, false=left
}

void MediaLib::contextMenuEvent(QContextMenuEvent *event) {
		if(!mediaList->hasFocus()) 
		return;
// 	std::cout<<"AHH"<<std::endl;
//	QTreeWidgetItem * itm = mediaList->currentItem();
//	std::cout<<itm->text(0).toStdString()<<std::endl;
	infoMenu->exec(event->globalPos());
}

void MediaLib::appendToPlaylistContextInfo() {
	std::list<std::string> order;
	foreach(QString str, coll->getImportOrder()) {
	order.push_back(str.toStdString());
	}
	conn->playlist.addCollection(*selectedAsColl(),order);
}

QMimeData* MediaLib::getCurrentMimeData() {
	QMimeData * mimeData = new QMimeData;
	mimeData->setUrls(urlList);
// 	std::cout<<urlList.size()<<std::endl;
	
	QByteArray encodedData;
// 	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	Xmms::Coll::Coll* tmp = new Xmms::Coll::Union(*selectedAsColl());
// 	std::cout<<tmp<<std::endl;
// 	stream << tmp;	
// 	encodedData.append((char*)tmp);
	encodedData =QByteArray::number((qlonglong)tmp);
// 	std::cout<<tmp<<std::endl;

	mimeData->setData("application/x-collstruct", encodedData);
	return mimeData;
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
	setFocusPolicy(Qt::StrongFocus);
	
}

DropTreeWidget::~DropTreeWidget() {
//CODE THIS!!!
}

void DropTreeWidget::dragEnterEvent(QDragEnterEvent *event) {
	if((event->mimeData()->hasUrls() || event->mimeData()->hasFormat("application/x-collname")) && event->source()!=this)
	event->acceptProposedAction();
	else
	event->ignore();
}

//Nothing to do here
void DropTreeWidget::dragMoveEvent ( QDragMoveEvent * event) {
}


//Time to actually do some shite
void DropTreeWidget::dropEvent(QDropEvent *event){
	if(event->source()==lib ||event->source()==this) {
	event->ignore();
	return;
	}
// //std::cout<<"dragging"<<std::endl;
	if(event->mimeData()->hasUrls()) {
		path->clear();
		QList<QUrl> list = event->mimeData()->urls();
		
		path->append(list.value(0).toString());
		
		if(path->startsWith("file://")) {
		path->remove(0,7);
		}
	
		QModelIndex indice(dirModel->index(*path));
		if(!path->startsWith("file://"))
		path->prepend("file://");
			if(!dirModel->isDir(indice)) {
			conn->medialib.addEntry(path->toStdString());
			}
			else {
			conn->medialib.pathImport(path->toStdString());
			}
	}
	else if(event->mimeData()->hasFormat("application/x-collname")) {
	QByteArray encodedData = event->mimeData()->data("application/x-collname");
	QDataStream stream(&encodedData, QIODevice::ReadOnly);
	QString title;
	QString ns;
	stream >> title;
	stream >> ns;	
		if(ns=="COLLECTIONS") {
		Xmms::Coll::Reference* temp = new Xmms::Coll::Reference(title.toStdString(),Xmms::Collection::COLLECTIONS);
		lib->loadCollection(temp);
		}
	}
	
}

void DropTreeWidget::keyPressEvent ( QKeyEvent * event ) {
	if(event->key() == (Qt::Key_Delete))
	emit removeSelected();
	else
	event->ignore();
}

QStringList DropTreeWidget::mimeTypes() const {
	QStringList types;
	types << "text/uri-list" << "application/x-collname" <<"application/x-collstruct";
	return types;
}

QMimeData* DropTreeWidget::mimeData(const QList<QTreeWidgetItem *> items) const {
	return lib->getCurrentMimeData();
}
#endif


