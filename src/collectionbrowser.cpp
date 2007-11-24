#ifndef COLLECTIONBROWSER_CPP
#define COLLECTIONBROWSER_CPP
#include "collectionbrowser.h"

CollectionBrowser::CollectionBrowser(DataBackend * c,QWidget * parent, Qt::WindowFlags f):QWidget(parent,f)  {
	conn = c;
	mlib = (MlibData*)(conn->getDataBackendObject(DataBackend::MLIB));
	coll = (CollData*)(conn->getDataBackendObject(DataBackend::COLL));
	collNamespace = conn->collection.COLLECTIONS;
	numFetched = 0;

	splitter = new QSplitter(this);
	collDisplay = new QTreeWidget(splitter);
	QSettings s;
	labels<<"Artist"<<"Album"<<"Title"<<"Genre";
	if(labels.contains("konfetka/collectionbrowserlabels"))
	labels = s.value("konfetka/collectionbrowserlabels").toStringList();
	conn->changeAndSaveQSettings("konfetka/collectionbrowserlabels",labels);
	collDisplay->setHeaderLabels(labels);
	collDisplay->setSortingEnabled(true);	

	collList = new QListWidget(splitter);

	layout1 = new QGridLayout();
	layout1->addWidget(splitter,0,0);
	layout1->setRowStretch(0,1);

	layout2  = new QGridLayout();
	layout2->addWidget(splitter,0,0);
	layout2->setRowStretch(0,1);

	this->setLayout(layout1); 
	
	connect(collDisplay->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(getNextFew(int)));
	connect(collDisplay,SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)),this,SLOT(addItemToPlist(QTreeWidgetItem*,int)));

	updateCollList(coll->getCollections());
	connect(coll,SIGNAL(collectionsChanged(QStringList)),this,SLOT(updateCollList(QStringList)));
	connect(collList,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),this,SLOT(getCollectionFromItem(QListWidgetItem*)));
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
					if(mlib->getInfo(labels[i].toLower(),id).toInt()==-1)
					newItem->setText(i,"");
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
	if(side)
	this->setLayout(layout1);
	else
	this->setLayout(layout2);
}

void CollectionBrowser::resizeEvent(QResizeEvent* event) {
	for(int i=0;i<collDisplay->columnCount();i++) {
		collDisplay->setColumnWidth(i,(collDisplay->width())/(collDisplay->columnCount()));
	}
}

void CollectionBrowser::updateCollList(QStringList list) {
	collList->clear();
	collList->addItems(list);
}

void CollectionBrowser::getCollectionFromItem(QListWidgetItem * item) {
// 	std::cout<<item->text().toStdString()<<std::endl;
	conn->collection.get(item->text().toStdString(),collNamespace)(Xmms::bind(&CollectionBrowser::recievedNewColl,this));
}

bool CollectionBrowser::recievedNewColl(const Xmms::Coll::Coll& newColl) {
	conn->collection.queryIds(newColl)(Xmms::bind(&CollectionBrowser::updateCollDisplay,this));
	return true;
}

bool CollectionBrowser::updateCollDisplay(const Xmms::List <uint> &list) {
	collDisplay->clear();
	numFetched = 0;
	idToItem.clear();int limit=50;
	for (list.first();list.isValid(); ++list) {
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
	QScrollBar* ptr = collDisplay->verticalScrollBar();
	std::cout<<"VALUE: "<<val<<" PAGESTP: "<<ptr->pageStep()<<" MIN: "<<ptr->minimum()<<" MAX: "<<ptr->maximum();
	std::cout<<" TLIC: "<<collDisplay->topLevelItemCount()<<std::endl;
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
// 	std::cout<<id<<std::endl;
// 	std::cout<<mlib->getInfo(QString("url"),id).toString().toStdString()<<std::endl;
	conn->playlist.addUrl(mlib->getInfo(QString("url"),id).toString().toStdString());
}

void CollectionBrowser::greyItem(QTreeWidgetItem* item) {
	for(int i=0;i<item->columnCount();i++) {
	item->setForeground(i,QBrush(QColor("grey"),Qt::SolidPattern));
	}
}

#endif

