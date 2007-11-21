#ifndef COLLECTIONBROWSER_CPP
#define COLLECTIONBROWSER_CPP
#include "collectionbrowser.h"

CollectionBrowser::CollectionBrowser(DataBackend * c,QWidget * parent, Qt::WindowFlags f):QWidget(parent,f)  {
	conn = c;
	mlib = (MlibData*)(conn->getDataBackendObject(DataBackend::MLIB));
	coll = (CollData*)(conn->getDataBackendObject(DataBackend::COLL));
	collNamespace = conn->collection.COLLECTIONS;

	splitter = new QSplitter(this);
	collDisplay = new QTreeWidget(splitter);
	collList = new QListWidget(splitter);

	layout1 = new QGridLayout();
	layout1->addWidget(splitter,0,0);
	layout1->setRowStretch(0,1);

	layout2  = new QGridLayout();
	layout2->addWidget(splitter,0,0);
	layout2->setRowStretch(0,1);

	this->setLayout(layout1); 
	
	updateCollList(coll->getCollections());
	connect(coll,SIGNAL(collectionsChanged(QStringList)),this,SLOT(updateCollList(QStringList)));
	connect(collList,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),this,SLOT(getCollectionFromItem(QListWidgetItem*)));
}

void CollectionBrowser::setLayoutSide(bool side) {
	if(side)
	this->setLayout(layout1);
	else
	this->setLayout(layout2);
}

void CollectionBrowser::updateCollList(QStringList list) {
	collList->clear();
	collList->addItems(list);
}

void CollectionBrowser::getCollectionFromItem(QListWidgetItem * item) {
	std::cout<<item->text().toStdString()<<std::endl;
	conn->collection.get(item->text().toStdString(),collNamespace)(Xmms::bind(&CollectionBrowser::recievedNewColl,this));
	//  CollResult   get( const std::string& name, Namespace nsname ) const;
}

bool CollectionBrowser::recievedNewColl(const Xmms::Coll::Coll& newColl) {
	std::cout<<"AHCHBADC"<<std::endl;

	return true;
}

#endif

