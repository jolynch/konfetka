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
	
	collDisplay = new CollTreeWidget(this,splitter);
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

	layout1 = new QGridLayout();
	layout1->addWidget(dispColl,0,0);
	layout1->addWidget(splitter,1,0);
	layout1->setRowStretch(1,1);

	this->setLayout(layout1); 

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
// 	std::cout<<item->text().toStdString()<<std::endl;
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
	Xmms::Coll::Reference* temp = new Xmms::Coll::Reference(currentCollection.toStdString(),currentNamespace);
	currentCollectionStructure = temp;
	conn->collection.queryIds(newColl)(Xmms::bind(&CollectionBrowser::updateCollDisplay,this));
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
	Xmms::Coll::Idlist newArguments;
		for(int i=0;i<temp.size();i++) {
			newArguments.append(idToItem.key(temp.value(i)));
		}
	std::cout<<newArguments.size()<<std::endl;
	std::cout<<currentCollection.toStdString()<<" "<<currentNamespace<<std::endl;
	Xmms::Coll::Reference * ref = new Xmms::Coll::Reference(currentCollection.toStdString(),currentNamespace);
	Xmms::Coll::Intersection * newRef = new Xmms::Coll::Intersection();
	Xmms::Coll::Complement * comp = new Xmms::Coll::Complement(newArguments);
	newRef->addOperand(*ref);
	newRef->addOperand(*comp);
	conn->collection.save(*newRef,currentCollection.toStdString(),currentNamespace);
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

void CollectionBrowser::appendNewCollection(Xmms::Coll::Coll* tmp) {
	Xmms::Coll::Union* un = new Xmms::Coll::Union();
		if(currentCollectionStructure->getType() == Xmms::Coll::UNION) {
		Xmms::Coll::OperandIterator temp = currentCollectionStructure->getOperandIterator();
				for(temp.first();temp.valid();temp.next()) {
				un->addOperand(**temp);
// 				tmpStr = collAsQString((**temp));
// 				result += tmpStr;
// 				temp.next();
// 					if(temp.valid())
// 					result+= " OR ";
				}
		}
// 	un->addOperand(*currentCollectionStructure);
	un->addOperand(*tmp);
		try{
		conn->collection.save(*un,currentCollection.toStdString(),currentNamespace);
		} catch (Xmms::result_error c) { std::cout<<"bled"<<std::endl;}
}

CollTreeWidget::CollTreeWidget(CollectionBrowser* l,QWidget * parent):QTreeWidget(parent) {
	lib = l;
}

CollTreeWidget::~CollTreeWidget() {
}


void CollTreeWidget::dropEvent(QDropEvent *event) {
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
	if(event->mimeData()->hasFormat("application/x-collstruct"))
	event->acceptProposedAction();
// 	else
// 	event->ignore();
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

