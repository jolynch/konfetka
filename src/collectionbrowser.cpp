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
		qDebug()<<"fetching";
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
	conn->collection.queryIds(newColl)(Xmms::bind(&CollectionBrowser::updateCollDisplay,this));
	if(newColl.getAttribute("IsBin")=="true") {
		currentCollectionType = 1337; //Its a Bin :-P
	} 
	return true;
}

bool CollectionBrowser::updateCollDisplay(const Xmms::List <uint> &list) {
	collDisplay->clear();
	numFetched = 0;
	idToItem.clear();int limit=50;
	for (list.first();list.isValid(); ++list) {
		if(idToItem.contains(*list)) continue;
		if(limit>0) { 
		addIdToView(*list,true);
		}
		else
		addIdToView(*list,false);
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
	QList<uint> toRemove;
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
	qDebug()<<ok<<text;
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
bool CollectionBrowser::appendListToBin(QList<uint> list,const Xmms::Coll::Coll& newColl) {
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

bool CollectionBrowser::removeFromBin(QList<uint> list,const Xmms::Coll::Coll& coll) {
	if(currentCollectionType != 1337) return false;
	
	return true;
}
*/	

void CollectionBrowser::appendListToBin(QList<uint> list) {
	if(currentCollectionType!=1337) return;

	xmmsc_result_t* res = xmmsc_coll_get (conn->getConnection(),currentCollection.toStdString().c_str(),"Collections");  
	xmmsc_result_wait (res);
	xmmsc_coll_t* new_coll;
	xmmsc_result_get_collection (res, &new_coll);
	xmmsc_coll_t *op;
	for(xmmsc_coll_operand_list_first(new_coll);xmmsc_coll_operand_list_valid(new_coll);xmmsc_coll_operand_list_next(new_coll)) {
                if(xmmsc_coll_operand_list_entry(new_coll, &op)) {
			if(xmmsc_coll_get_type(op)==XMMS_COLLECTION_TYPE_UNION) {
			xmmsc_coll_t *op1;
			for(xmmsc_coll_operand_list_first(op);xmmsc_coll_operand_list_valid(op);xmmsc_coll_operand_list_next(op)){
				if(xmmsc_coll_operand_list_entry(op,&op1)) {
					if(xmmsc_coll_get_type(op1)==XMMS_COLLECTION_TYPE_IDLIST) {
					qDebug()<<"Found idlist";
						for(int i=0;i<list.size();i++) {
							xmmsc_coll_idlist_append(op1,list[i]);	
						}
					}
				}
			}	
			}
		}
	}
	xmmsc_result_wait(xmmsc_coll_save(conn->getConnection(),new_coll,currentCollection.toStdString().c_str(),"Collections"));
	xmmsc_result_unref (res);	
	if(collList->currentItem()!=NULL && collList->currentRow()>0)
		 getCollectionFromItem(collList->currentItem());
}

void CollectionBrowser::appendCollToBin(Xmms::CollPtr coll) {
	if(currentCollectionType!=1337) return;

	if(collList->currentItem()!=NULL && collList->currentRow()>0)
		getCollectionFromItem(collList->currentItem());

}

void CollectionBrowser::removeFromBin(QList<uint> list) {
	if(currentCollectionType!=1337) return;
	
	xmmsc_result_t* res = xmmsc_coll_get (conn->getConnection(),currentCollection.toStdString().c_str(),"Collections");  
	xmmsc_result_wait (res);
	xmmsc_coll_t* new_coll;
	xmmsc_result_get_collection (res, &new_coll);
	xmmsc_coll_t *op;
	for(xmmsc_coll_operand_list_first(new_coll);xmmsc_coll_operand_list_valid(new_coll);xmmsc_coll_operand_list_next(new_coll)) {
                if(xmmsc_coll_operand_list_entry(new_coll, &op)) {
			if(xmmsc_coll_get_type(op)==XMMS_COLLECTION_TYPE_COMPLEMENT) {
			xmmsc_coll_t *op1;
				if(xmmsc_coll_operand_list_first(op)&&xmmsc_coll_operand_list_entry(op,&op1)) {
					if(xmmsc_coll_get_type(op1)==XMMS_COLLECTION_TYPE_IDLIST) {
						for(int i=0;i<list.size();i++) {
							xmmsc_coll_idlist_append(op1,list[i]);	
						}
					}
				}
			}
		}
	}
	xmmsc_result_wait(xmmsc_coll_save(conn->getConnection(),new_coll,currentCollection.toStdString().c_str(),"Collections"));
	xmmsc_result_unref (res);
	if(collList->currentItem()!=NULL && collList->currentRow()>0)
		getCollectionFromItem(collList->currentItem());
}

CollTreeWidget::CollTreeWidget(CollectionBrowser* l,DataBackend * c,QWidget * parent):QTreeWidget(parent) {
	lib = l;
	conn = c;
}

CollTreeWidget::~CollTreeWidget() {
}

bool CollTreeWidget::lookupId(bool toAdd,const uint val) {
	listOfIDs<<val;
	numIDs-=1;
	if(numIDs == 0)
		lib->appendListToBin(listOfIDs);
	return true;
}

void CollTreeWidget::dropEvent(QDropEvent *event) {
	qDebug()<<"Drop";
	qDebug()<<lib->getCurrentType();
	if(lib->getCurrentType()==1337) { //Its a dynamic Bin
		if(event->mimeData()->hasUrls()) {
			qDebug()<<"Bin";
			QList<QUrl> list = event->mimeData()->urls();
			bool toAppend = true;
			numIDs = list.size();
			listOfIDs.clear();
			for(int i=0;i<list.size();i++) {
				conn->medialib.getID(conn->encodeUrl(list[i].toString()).toStdString())
					(boost::bind(&CollTreeWidget::lookupId,this,toAppend,_1));
			}
			//QList<uint> list;
			//list<<78<<71<<77;
			//lib->appendListToBin(list);
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
	if(event->mimeData()->hasUrls())
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

