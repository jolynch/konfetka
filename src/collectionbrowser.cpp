#ifndef COLLECTIONBROWSER_CPP
#define COLLECTIONBROWSER_CPP
#include "collectionbrowser.h"

CollectionBrowser::CollectionBrowser(DataBackend * c,QWidget * parent, Qt::WindowFlags f):LayoutPanel(parent,f)  {
	conn = c;
	mlib = (MlibData*)(conn->getDataBackendObject(DataBackend::MLIB));
	coll = (CollData*)(conn->getDataBackendObject(DataBackend::COLL));
	collNamespace = Xmms::Collection::COLLECTIONS;
	plistNamespace = Xmms::Collection::PLAYLISTS;
	numFetched = 0;
	
	splitter = new QSplitter(this);
	listSplitter = new QSplitter(Qt::Vertical,splitter);
	collList = new QListWidget(listSplitter);
	collList->setDragEnabled(true);
	plistList = new QListWidget(listSplitter);
	plistList->setDragEnabled(true);
	
	collDisplay = new CollTreeWidget(this,conn,splitter);
	QSettings s;
	labels<<"Artist"<<"Album"<<"Title"<<"Genre";
	if(labels.contains("konfetka/collectionbrowserlabels"))
	labels = s.value("konfetka/collectionbrowserlabels").toStringList();
	conn->changeAndSaveQSettings("konfetka/collectionbrowserlabels",labels);
	collDisplay->setHeaderLabels(labels);
	collDisplay->setDragEnabled(true);
	collDisplay->setAcceptDrops(true);
	collDisplay->setDropIndicatorShown(true);
	collDisplay->setSelectionMode(QAbstractItemView::ExtendedSelection);
	splitter->setStretchFactor(1,1);

	dispColl = new QLineEdit();
	dispColl->setEnabled(false);
	QPushButton * createBin = new QPushButton("Create Bin");
	createBin->setToolTip("Create Collection \"Bins\" to organize your music in");
	connect(createBin,SIGNAL(clicked()),this,SLOT(createNewBin()));

	layout = new QGridLayout();
	layout->addWidget(dispColl,0,0);
	layout->addWidget(createBin,0,1);
	layout->addWidget(splitter,1,0,1,2);
	layout->setRowStretch(1,1);

	this->setLayout(layout); 

	mimeData = NULL; drag = NULL;

	connect(collDisplay->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(getNextFew(int)));
	connect(collDisplay,SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)),this,SLOT(addItemToPlist(QTreeWidgetItem*,int)));

	//Drag-Drop
	connect(collList,SIGNAL(itemPressed(QListWidgetItem*)),this,SLOT(startDragList(QListWidgetItem*)));
	connect(plistList,SIGNAL(itemPressed(QListWidgetItem*)),this,SLOT(startDragList(QListWidgetItem*)));
	connect(&waitTimer,SIGNAL(timeout()),this,SLOT(startDrag()));
	connect(collDisplay,SIGNAL(itemClicked(QTreeWidgetItem*,int)),&waitTimer,SLOT(stop()));
	connect(collList,SIGNAL(itemClicked(QListWidgetItem*)),&waitTimer,SLOT(stop()));
	connect(plistList,SIGNAL(itemClicked(QListWidgetItem*)),&waitTimer,SLOT(stop())); 
	//End Drag-Drop

	updateCollList(coll->getCollections());
	updatePlistList(coll->getPlaylists());
// 	updateCollList(coll
	connect(coll,SIGNAL(collectionsChanged(QStringList)),this,SLOT(updateCollList(QStringList)));
	connect(coll,SIGNAL(playlistsChanged(QStringList)),this,SLOT(updatePlistList(QStringList)));
	connect(collList,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),this,SLOT(getCollectionFromItem(QListWidgetItem*)));
	connect(plistList,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),this,SLOT(getCollectionFromItem(QListWidgetItem*)));
	connect(mlib,SIGNAL(infoChanged(int)),this,SLOT(updateInfo(int)));
}

void CollectionBrowser::addIdToView(int id, bool isPriority) {
	QTreeWidgetItem * newItem;
		if(idToItem.contains(id)) {
		newItem = idToItem.value(id);
		if(newItem->text(0)!="")
		numFetched-=1;
		}
		else 
		newItem = new QTreeWidgetItem(collDisplay);
			if(isPriority) {
				for(int i=0;i<labels.size();i++) {
					if(mlib->getInfo(labels[i].toLower(),id).toInt()==-1) {
					newItem->setText(i,"");
					}
					else
					newItem->setText(i,(mlib->getInfo(labels[i].toLower(),id)).toString());
				}	
				
				if(mlib->getInfo("status",id).toString()=="Broken") {
				greyItem(newItem);
				}
			numFetched+=1;
			}
// 	std::cout<<mlib->getInfo(QString("status"),id).toInt()<<std::endl;
	idToItem.insert(id,newItem);
}

void CollectionBrowser::setLayoutSide(bool side) {
	std::cout<<"SIDE: "<<side<<std::endl;
	if(side)
	splitter->addWidget(listSplitter);
	else
	splitter->addWidget(collDisplay);
}

void CollectionBrowser::resizeEvent(QResizeEvent* event) {
	for(int i=0;i<collDisplay->columnCount();i++) {
		collDisplay->setColumnWidth(i,(collDisplay->width())/(collDisplay->columnCount()));
	}
}

void CollectionBrowser::updateCollList(QStringList list) {
	collList->clear();
	QListWidgetItem * collLabel = new QListWidgetItem("Collections:",collList);
	collLabel->setFlags(Qt::ItemIsEnabled);
	QFont curFont = collLabel->font();
	curFont.setBold(true);
	collLabel->setFont(curFont);
	collList->addItems(list);
}

void CollectionBrowser::updatePlistList(QStringList list) {
	plistList->clear();
	QListWidgetItem * plistLabel = new QListWidgetItem("Playlists:",plistList);
	plistLabel->setFlags(Qt::ItemIsEnabled);
	QFont curFont = plistLabel->font();
	curFont.setBold(true);
	plistLabel->setFont(curFont);
	plistList->addItems(list);
}


void CollectionBrowser::getCollectionFromItem(QListWidgetItem * item) {
	if(collList->row(item) >0) {
		conn->collection.get(item->text().toStdString(),collNamespace)(Xmms::bind(&CollectionBrowser::recievedNewColl,this));
		plistList->setCurrentItem(plistList->item(0));
		currentCollection = item->text();
		currentNamespace = collNamespace;
	}
	else if(plistList->row(item)>0) {
		conn->collection.get(item->text().toStdString(),plistNamespace)(Xmms::bind(&CollectionBrowser::recievedNewColl,this));
		collList->setCurrentItem(collList->item(0));
		currentCollection = item->text();
		currentNamespace = plistNamespace;
	}
	else {
		collDisplay->clear();
	}
}

bool CollectionBrowser::recievedNewColl(const Xmms::Coll::Coll& newColl) {
	dispColl->setText(coll->collAsQString(newColl));
	currentCollectionType = newColl.getType();
	std::list<std::string> order;
// 	order.push_back("artist");
// 	order.push_back("album");
	order.push_back("tracknr");
	conn->collection.queryIds(newColl,order)(Xmms::bind(&CollectionBrowser::updateCollDisplay,this));
	if(newColl.getAttribute("IsBin")=="true") {
		currentCollectionType = 1337; //Its a Bin :-P
	} 
	return true;
}

bool CollectionBrowser::updateCollDisplay(const Xmms::List <int> &list) {
	collDisplay->clear();
	numFetched = 0;
	idToItem.clear();int limit=50;
	Xmms::List_const_iterator_ < int > iter;
	for (iter=list.begin();!iter.equal(list.end()); ++iter){
		if(idToItem.contains(*iter)) continue;
		if(limit>0) { 
		addIdToView(*iter,true);
		}
		else
		addIdToView(*iter,false);
		limit--;
	}
	return true;
}

void CollectionBrowser::updateInfo(int id) {
	if(idToItem.contains(id)) {
	addIdToView(id,true);
	}
}

void CollectionBrowser::getNextFew(int val) {
	int id;
		for(int i = val;i<(val+collDisplay->verticalScrollBar()->pageStep()) && i < (collDisplay->topLevelItemCount());i++) {
		id = idToItem.key(collDisplay->topLevelItem(i));		
			if(mlib->hasInfo(id))
			addIdToView(id,true);
			else
			mlib->getInfo("url",id);
		}
}

void CollectionBrowser::addItemToPlist(QTreeWidgetItem* item,int col) {
	int id = idToItem.key(item);
	conn->playlist.addUrl(mlib->getInfo("url",id).toString().toStdString());
}

void CollectionBrowser::greyItem(QTreeWidgetItem* item) {
	for(int i=0;i<item->columnCount();i++) {
	item->setForeground(i,QBrush(QColor("grey"),Qt::SolidPattern));
	}
}

void CollectionBrowser::startDragList(QListWidgetItem* item) {
	mimeData = new QMimeData();
	QString ns;
		if(collList->row(item) > 0)
		ns = "COLLECTIONS";
		else if(plistList->row(item) > 0)
		ns = "PLAYLISTS";
		else
		return;
	QString collName = item->text();
	std::cout<<ns.toStdString()<<" "<<collName.toStdString()<<std::endl;
	QByteArray encodedData;
	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	stream << collName << ns;
	mimeData->setData("application/x-collname", encodedData);
	waitTimer.start(qApp->doubleClickInterval());	
}

void CollectionBrowser::startDrag() {
	drag = new QDrag(this);
	drag->setMimeData(mimeData);
	waitTimer.stop();
	drag->exec(Qt::CopyAction | Qt::MoveAction);
}

void CollectionBrowser::keyPressEvent(QKeyEvent* event) {
	if(event->key() == (Qt::Key_Delete) && collDisplay->hasFocus())
	removeSelectedItems();
	else if(event->key() == (Qt::Key_Delete) && collList->hasFocus())
	removeSelectedCollections();
	else if(event->key() == (Qt::Key_Delete) && plistList->hasFocus())
	removeSelectedPlaylists();
	else
	event->ignore();
}

//Removes a collection
void CollectionBrowser::removeSelectedCollections() {
	QList<QListWidgetItem*> temp = collList->selectedItems();
		for(int i=0;i<temp.size();i++) {
		conn->collection.remove(temp.value(i)->text().toStdString(),collNamespace)
						(Xmms::bind(&DataBackend::scrapResult, conn));
		}
	collDisplay->clear();
}

//Removes a playlist
void CollectionBrowser::removeSelectedPlaylists() {
	QList<QListWidgetItem*> temp = plistList->selectedItems();
		for(int i=0;i<temp.size();i++) {
		conn->collection.remove(temp.value(i)->text().toStdString(),plistNamespace)
						(Xmms::bind(&DataBackend::scrapResult, conn));
		}
	collDisplay->clear();
}

//Appends an extra NOT collection
void CollectionBrowser::removeSelectedItems(){
	QList<QTreeWidgetItem*> temp = collDisplay->selectedItems();
	QList<int> toRemove;
		for(int i=0;i<temp.size();i++) {
			toRemove.append(idToItem.key(temp.value(i)));
		}
	std::cout<<toRemove.size()<<std::endl;
	if(currentCollectionType == 1337) {
		removeFromBin(toRemove);
	}
}

QMimeData* CollectionBrowser::getMimeInfo(const QList<QTreeWidgetItem *> items) {
	QList<QUrl> resultList;
	QMimeData* mimeData = new QMimeData();
	foreach(QTreeWidgetItem* item,items) {
		QString path = mlib->getInfo("url",idToItem.key(item)).toString();
		resultList.append(QUrl(path));
	}
	mimeData->setUrls(resultList);
	return mimeData;
}

int CollectionBrowser::getCurrentType() {
	return currentCollectionType;
}

QString CollectionBrowser::getCurrentName() {
	return currentCollection;
}

Xmms::Collection::Namespace CollectionBrowser::getCurrentNamespace() {
	return currentNamespace;
}



void CollectionBrowser::createNewBin() {
	bool ok;
	QString text = QInputDialog::getText(this, "Bin Name","What would you like to call your new Bin?",
		       		            QLineEdit::Normal,NULL, &ok);
	if(!ok) return;
	Xmms::Coll::Idlist newBin;
	Xmms::Coll::Union inBin;
	inBin.addOperand(newBin);
	Xmms::Coll::Intersection overall;
	Xmms::Coll::Idlist newBinComplement;
	Xmms::Coll::Complement notInBin(newBinComplement);
	overall.addOperand(inBin);
	overall.addOperand(notInBin);
	overall.setAttribute("IsBin","true");
	//Untill they get collection editing all done Idlists will be used
	//newBin.setAttribute("IsBin","true");
	conn->collection.save(overall,text.toStdString(),collNamespace); 	
}
/* Untill the cpp bindings are ready
bool CollectionBrowser::appendListToBin(QList<int> list,const Xmms::Coll::Coll& newColl) {
	if(currentCollectionType != 1337) return false;
	//Untill they get stuff fixed
	//qDebug()<<"Before"<<coll->collAsQString(newColl);
	Xmms::CollPtr ptr;
	Xmms::Coll::OperandIterator temp = newColl.getOperandIterator();
	for(temp.first();temp.valid();temp.next()) {
		if((*temp)->getType()==Xmms::Coll::UNION) {
			Xmms::Coll::OperandIterator temp2 = (*temp)->getOperandIterator();
			for(temp2.first();temp2.valid();temp2.next()){
				if((*temp2)->getType()==Xmms::Coll::IDLIST) {
				ptr = *temp2; 
				break;
				}
			}
			break;
		}
	}
	for(int i=0;i<list.size();i++) {
		ptr->append(list[i]);
	}
	conn->collection.save(newColl,currentCollection.toStdString(),currentNamespace);
	return true;
}

bool CollectionBrowser::appendCollToBin(const Xmms::Coll::Coll& coll) {
	if(currentCollectionType != 1337) return false;
	
	return true;
}

bool CollectionBrowser::removeFromBin(QList<int> list,const Xmms::Coll::Coll& coll) {
	if(currentCollectionType != 1337) return false;
	
	return true;
}
*/	

//The next three functions look more complicated then they really are (thanks C), unfortunatelly the cpp bindings are kind of
//not working right now, so I had to use c. Remember a bin looks like Intersection(Union(InBin,_other_collections_),Complement(NotInBin))
//So all the for loops and such just find the correct part of the bin to change

xmmsc_coll_t* CollectionBrowser::findPartOfBin(xmmsc_coll_t* bin, int type) {//0=InBin idlist, 1=InBin union, 2=NotInBin idlist
//Disabled to force compile 08/20/09 - AF.
/*	if(currentCollectionType!=1337 || collList->currentItem()==NULL || collList->currentRow()<=0) return NULL;

	xmmsc_coll_t *op;
	for(xmmsc_coll_operand_list_first(bin);xmmsc_coll_operand_list_valid(bin);xmmsc_coll_operand_list_next(bin)) {
                if(xmmsc_coll_operand_list_entry(bin, &op)) {
			if(xmmsc_coll_get_type(op)==XMMS_COLLECTION_TYPE_UNION) {
				if(type ==1) return op;
			xmmsc_coll_t *op1;
			for(xmmsc_coll_operand_list_first(op);xmmsc_coll_operand_list_valid(op);xmmsc_coll_operand_list_next(op)){
				if(xmmsc_coll_operand_list_entry(op,&op1)) {
					if(xmmsc_coll_get_type(op1)==XMMS_COLLECTION_TYPE_IDLIST && type == 0) {
						return op1;
					}
				}
			}	
			}
			else if(xmmsc_coll_get_type(op)==XMMS_COLLECTION_TYPE_COMPLEMENT) {
			xmmsc_coll_t *op1;
				if(xmmsc_coll_operand_list_first(op)&&xmmsc_coll_operand_list_entry(op,&op1)) {
					if(xmmsc_coll_get_type(op1)==XMMS_COLLECTION_TYPE_IDLIST && type == 2) {
						return op1;
					}
				}
			}
		}
	}*/
	return NULL;
}

void CollectionBrowser::appendListToBin(QList<int> list) {
//Disabled to force compile 08/20/09 - AF.
/*	if(currentCollectionType!=1337) return;

	xmmsc_result_t* res = xmmsc_coll_get (conn->getConnection(),currentCollection.toStdString().c_str(),"Collections");
	xmmsc_result_wait (res);
	xmmsc_coll_t* new_coll;
	xmmsc_result_get_collection (res, &new_coll);

	xmmsc_coll_t* op = findPartOfBin(new_coll,0);
	for(int i=0;i<list.size();i++) {
		xmmsc_coll_idlist_append(op,list[i]);	
	}
	
	if(collList->currentItem()!=NULL && collList->currentRow()>0) {
		xmmsc_result_wait(xmmsc_coll_save(conn->getConnection(),new_coll,currentCollection.toStdString().c_str(),"Collections"));
		getCollectionFromItem(collList->currentItem());
	}
	xmmsc_coll_unref(new_coll);
*/
}

void CollectionBrowser::appendCollToBin(const Xmms::Coll::Coll& coll) {
//Disabled to force compile 08/20/09 - AF.
/*
	if(currentCollectionType!=1337) return;

	xmmsc_coll_t * to_append = coll.getColl();
	xmmsc_result_t* res = xmmsc_coll_get (conn->getConnection(),currentCollection.toStdString().c_str(),"Collections");
	xmmsc_result_wait (res);
	xmmsc_coll_t* new_coll;
	xmmsc_result_get_collection (res, &new_coll);

	xmmsc_coll_t* op = findPartOfBin(new_coll,1);
	xmmsc_coll_add_operand(op,to_append);

	if(collList->currentItem()!=NULL && collList->currentRow()>0) {
		xmmsc_result_wait(xmmsc_coll_save(conn->getConnection(),new_coll,currentCollection.toStdString().c_str(),"Collections"));
		getCollectionFromItem(collList->currentItem());
	}
	xmmsc_coll_unref(new_coll);
*/
}

void CollectionBrowser::removeFromBin(QList<int> list) {
//Disabled to force compile 08/20/09 - AF.
/*
	if(currentCollectionType!=1337) return;

	xmmsc_result_t* res = xmmsc_coll_get (conn->getConnection(),currentCollection.toStdString().c_str(),"Collections");
	xmmsc_result_wait (res);
	xmmsc_coll_t* new_coll;
	xmmsc_result_get_collection (res, &new_coll);

	xmmsc_coll_t* op = findPartOfBin(new_coll,2);
	for(int i=0;i<list.size();i++) {
		xmmsc_coll_idlist_append(op,list[i]);	
	}
	if(collList->currentItem()!=NULL && collList->currentRow()>0) {
		xmmsc_result_wait(xmmsc_coll_save(conn->getConnection(),new_coll,currentCollection.toStdString().c_str(),"Collections"));
		getCollectionFromItem(collList->currentItem());
	}
	xmmsc_coll_unref(new_coll);
*/
}

CollTreeWidget::CollTreeWidget(CollectionBrowser* l,DataBackend * c,QWidget * parent):QTreeWidget(parent) {
	lib = l;
	conn = c;
}

CollTreeWidget::~CollTreeWidget() {
}

bool CollTreeWidget::lookupId(bool toAdd,const int val) {
	listOfIDs<<val;
	numIDs-=1;
	if(numIDs == 0)
		lib->appendListToBin(listOfIDs);
	return true;
}

bool CollTreeWidget::lookupColl(const Xmms::Coll::Coll& coll) {
	lib->appendCollToBin(coll);
	return true;
}

void CollTreeWidget::dropEvent(QDropEvent *event) {
	if(lib->getCurrentType()==1337) { //Its a dynamic Bin
		if(event->mimeData()->hasUrls()) {
			QList<QUrl> list = event->mimeData()->urls();
			bool toAppend = true;
			numIDs = list.size();
			listOfIDs.clear();
			for(int i=0;i<list.size();i++) {
				conn->medialib.getID(conn->encodeUrl(list[i].toString()).toStdString())
					(boost::bind(&CollTreeWidget::lookupId,this,toAppend,_1));
			}
		}
		else if(event->mimeData()->hasFormat("application/x-collname")) {
			QDataStream in(event->mimeData()->data("application/x-collname"));
			QString name,ns;
			in>>name>>ns;
			conn->collection.get(name.toStdString(),Xmms::Collection::COLLECTIONS)(Xmms::bind(&CollTreeWidget::lookupColl,this));
		}
		//conn->collection.get(lib->getCurrentName().toStdString(),lib->getCurrentNamespace())
		//		(boost::bind(&CollectionBrowser::appendListToBin,lib,list,_1));
		

		//qDebug()<<lib->curStruct->getType();
	}
	
	
	return;
	QByteArray encodedData = event->mimeData()->data("application/x-collstruct");
	int temp = encodedData.toInt();
	Xmms::Coll::Coll* tmp = (Xmms::Coll::Coll*)temp;
	std::cout<<tmp->getType()<<std::endl;	
	lib->appendNewCollection(tmp);
// 	std::cout<<encodedData.number(16)<<std::endl;
}

void CollTreeWidget::dragMoveEvent ( QDragMoveEvent * event ) {

}

void CollTreeWidget::dragEnterEvent(QDragEnterEvent *event) {
	if(event->mimeData()->hasUrls() || event->mimeData()->hasFormat("application/x-collname"))
		event->acceptProposedAction();
}


QStringList CollTreeWidget::mimeTypes() const {
	QStringList types;
	types << "text/uri-list" << "application/x-collname" <<"application/x-collstruct";
	return types;
}

QMimeData* CollTreeWidget::mimeData(const QList<QTreeWidgetItem *> items) const {
	return lib->getMimeInfo(items);
}


#endif

