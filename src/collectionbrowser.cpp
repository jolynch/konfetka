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
	collDisplay = new QTreeWidget(splitter);
	QSettings s;
	labels<<"Artist"<<"Album"<<"Title"<<"Genre";
	if(labels.contains("konfetka/collectionbrowserlabels"))
	labels = s.value("konfetka/collectionbrowserlabels").toStringList();
	conn->changeAndSaveQSettings("konfetka/collectionbrowserlabels",labels);
	collDisplay->setHeaderLabels(labels);
	collDisplay->setDragEnabled(true);
	collDisplay->setSelectionMode(QAbstractItemView::ExtendedSelection);
	splitter->setStretchFactor(0,1);

	listSplitter = new QSplitter(Qt::Vertical,splitter);
	collList = new QListWidget(listSplitter);
	collList->setDragEnabled(true);
	plistList = new QListWidget(listSplitter);
	plistList->setDragEnabled(true);

	dispColl = new QLineEdit();

	layout1 = new QGridLayout();
	layout1->addWidget(dispColl,0,0);
	layout1->addWidget(splitter,1,0);
	layout1->setRowStretch(1,1);

	this->setLayout(layout1); 

	mimeData = NULL; drag = NULL;

	connect(collDisplay->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(getNextFew(int)));
	connect(collDisplay,SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)),this,SLOT(addItemToPlist(QTreeWidgetItem*,int)));

	//Drag-Drop
	connect(collDisplay,SIGNAL(itemPressed(QTreeWidgetItem*,int)),this,SLOT(startDragTree(QTreeWidgetItem*,int)));
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
				
				if(mlib->getInfo(QString("status"),id).toInt()==3) {
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
	splitter->addWidget(collDisplay);
	else
	splitter->addWidget(listSplitter);
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
	if(collList->row(item) >=0)
	conn->collection.get(item->text().toStdString(),collNamespace)(Xmms::bind(&CollectionBrowser::recievedNewColl,this));
	else if(plistList->row(item)>=0)
	conn->collection.get(item->text().toStdString(),plistNamespace)(Xmms::bind(&CollectionBrowser::recievedNewColl,this));
	else return;
}

bool CollectionBrowser::recievedNewColl(const Xmms::Coll::Coll& newColl) {
	dispColl->setText(coll->collAsQString(newColl));
// 	std::cout<<collTypes[newColl.getType()].toStdString()<<std::endl;
// 	if(newColl.getType()<3 && newColl.getType()>0) {
// 	Xmms::Coll::OperandIterator temp = newColl.getOperandIterator();
// 		for(temp.first();temp.valid();temp.next()) {
// 		std::cout<<collTypes[(*temp)->getType()].toStdString()<<std::endl;
// 		std::cout<<(*temp)->getAttribute("field")<<" "<<(*temp)->getAttribute("value")<<std::endl;
// 		}	
// 	}
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
			mlib->getInfo(QString("url"),id);
		}
}

void CollectionBrowser::addItemToPlist(QTreeWidgetItem* item,int col) {
	int id = idToItem.key(item);
	conn->playlist.addUrl(mlib->getInfo(QString("url"),id).toString().toStdString());
}

void CollectionBrowser::greyItem(QTreeWidgetItem* item) {
	for(int i=0;i<item->columnCount();i++) {
	item->setForeground(i,QBrush(QColor("grey"),Qt::SolidPattern));
	}
}

void CollectionBrowser::startDragTree(QTreeWidgetItem* item,int col) {
	std::cout<<item->text(0).toStdString()<<std::endl;
	QList<QTreeWidgetItem*> sel = collDisplay->selectedItems(); 
	QList<QUrl> resultList;
	mimeData = new QMimeData();
	foreach(QTreeWidgetItem* item,sel) {
		QString path = mlib->getInfo(QString("url"),idToItem.key(item)).toString();
		resultList.append(QUrl(path));
	}
	mimeData->setUrls(resultList);
	waitTimer.start(qApp->doubleClickInterval());
	/*foreach (QTreeWidgetItem* item, sel) {
		QString text = data(index, Qt::DisplayRole).toString
	stream << text;
         }
	}*/

// 	mimeData->setData("application/x-collname",
	
}

void CollectionBrowser::startDragList(QListWidgetItem* item) {
	std::cout<<item->text().toStdString()<<std::endl;
}

void CollectionBrowser::startDrag() {
	drag = new QDrag(this);
	drag->setMimeData(mimeData);
	waitTimer.stop();
	drag->exec(Qt::CopyAction | Qt::MoveAction);
}

void CollectionBrowser::mousePressEvent(QMouseEvent * event) {
	std::cout<<"PRESSED"<<std::endl;
	event->ignore();
}

#endif

