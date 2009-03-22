#ifndef SHOUTCAST_CPP
#define SHOUTCAST_CPP
#include "shoutcast.h"


Shoutcast::Shoutcast(DataBackend * c, QWidget * parent, Qt::WindowFlags f):WebService(c,parent,f) {
	conn = c;
	plist=((PlistData *)conn->getDataBackendObject(DataBackend::PLIST));

	layout = new QGridLayout();
	
	//tree = new DragTreeWidget(this);
	tree = new QTreeWidget(this);
	tree->setHeaderLabel("Shoutcast Streams");
	//tree->setDragEnabled(true);
	tree->setDragEnabled(false);
	tree->setAcceptDrops(false);
	tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
	tree->setSelectionMode(QAbstractItemView::SingleSelection);
// 	tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
	tree->setObjectName ("shoutcast");
	
	connect(tree,SIGNAL(itemExpanded(QTreeWidgetItem*)),this,SLOT(fetchGenre(QTreeWidgetItem*)));
	refresh = new QPushButton(QIcon(":images/repeat_all"),"Fetch Genre List",this);
	refresh->setToolTip("Fetches entire genre list from shoutcast.com. Sometimes shoutcast gives\n\"Service Unavailable\" errors, so if you don't get the list the first time\nhit the button again.");
	addStream = new QPushButton(QIcon(":images/plus"),"Add Streams",this);
	search = new QLineEdit("Station, Genre");
	
	searchOptions = new QGroupBox("Additional Search Options");
	QGridLayout * boxLayout = new QGridLayout();	
	bitrate = new QLineEdit("*");
	streamtype = new QLineEdit("*");
	genre = new QLineEdit("*");
	limit= new QLineEdit("*");
	boxLayout->addWidget(new QLabel("Bit-Rate:"),0,0);
	boxLayout->addWidget(bitrate,0,1);
	boxLayout->addWidget(new QLabel("Stream Type:"),0,2);
	boxLayout->addWidget(streamtype,0,3);
	boxLayout->addWidget(new QLabel("Genre:"),0,4);
	boxLayout->addWidget(genre,0,5);
	boxLayout->addWidget(new QLabel("Limit:"),0,6);
	boxLayout->addWidget(limit,0,7);
	searchOptions->setLayout(boxLayout);
	

	QStringList options;
	options << "Append to Playlist"<<"New Playlist"<<"New Collection";
	addMethod = new QComboBox();
	addMethod->setToolTip(tr("Action to take with the selected stream"));
	addMethod->addItems(options);
	
	connect(refresh,SIGNAL(clicked()),this,SLOT(fetchGenres()));
	connect(addStream,SIGNAL(clicked()),this,SLOT(processStream()));
	connect(search,SIGNAL(returnPressed()),this,SLOT(fetchQuery()));
	
	manager = new QNetworkAccessManager(this);
	searchButton = new QPushButton("Advanced Search");
	searchButton->setCheckable(true);
	connect(searchButton,SIGNAL(clicked()),this,SLOT(toggleOptions()));
		
		
	QPushButton * searchActionButton = new QPushButton("Search");
	connect(searchActionButton,SIGNAL(clicked()),this,SLOT(fetchQuery()));
		
	layout->addWidget(searchActionButton,4,0,1,1);
	layout->addWidget(search,4,1,1,3);
	layout->addWidget(searchButton,4,4);
	layout->addWidget(searchOptions,5,0,1,5);
	layout->addWidget(tree,0,0,4,4);
	layout->addWidget(refresh,0,4);
	layout->addWidget(new QLabel(""),1,4);
	layout->addWidget(addMethod,2,4);
	layout->addWidget(addStream,3,4);
	setLayout(layout);
	toggleOptions(1); //Init no advanced

}

Shoutcast::~Shoutcast() {
	delete info;
	delete tree;
	delete refresh;
}

QString Shoutcast::getInfo() {
	return QString("Browse the thousands of streams on Shoutcast");
}

QString Shoutcast::getServiceName() {
	return QString("Stream Browser");
}

void Shoutcast::toggleOptions(bool overide) {
	if(searchOptions->isVisible() || overide)
		searchOptions->hide();
	else if(!overide)
		searchOptions->show();
}

void Shoutcast::fetchGenres() {
	genres.clear();
	requestsCompleted.clear();
	streams.clear();
	tree->clear();
	connect(tree,SIGNAL(itemExpanded(QTreeWidgetItem*)),this,SLOT(fetchGenre(QTreeWidgetItem*)));

	connect(manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(recievedGenres(QNetworkReply*)));
	QNetworkReply * reply = manager->get(QNetworkRequest(QUrl("http://www.shoutcast.com/sbin/newxml.phtml?")));
	connect(reply, SIGNAL(downloadProgress(qint64,qint64)),this,SIGNAL(progress(qint64,qint64)));
}

void Shoutcast::recievedGenres(QNetworkReply * reply) {
	disconnect(manager, 0, this, 0);
	qDebug()<<"fetched_1";
	
	QDomDocument doc("genres");
	if(doc.setContent(reply->readAll())) {
		qDebug()<<"fetched_2";
		QDomNode n = doc.documentElement().firstChild();
		while(!n.isNull()) {
			QDomElement e = n.toElement(); // try to convert the node to an element.
			if(!e.isNull()) {
				QTreeWidgetItem * item = new QTreeWidgetItem();
				item->setText(0,QString(e.attribute("name")));
				item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
				tree->addTopLevelItem(item);
				genres.insert(item,e.attribute("name"));
			}
			n = n.nextSibling();
		}	
	}
}

void Shoutcast::fetchGenre(QTreeWidgetItem * genre) {
	if(genres.contains(genre) &&!requestsCompleted.contains(genre)) {
		genre->setIcon(0,QIcon(":images/repeat_all"));
		QUrl url("http://www.shoutcast.com/sbin/newxml.phtml?genre="+genres[genre]);
		connect(manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(addGenre(QNetworkReply*)));
		QNetworkReply * reply =	manager->get(QNetworkRequest(url));
		requests.insert(reply,genre);
		connect(reply, SIGNAL(downloadProgress(qint64,qint64)),this,SIGNAL(progress(qint64,qint64)));
	}
}

void Shoutcast::addGenre(QNetworkReply * reply) {
	disconnect(manager,0,this,0);
	QDomDocument doc("streams");
	QList<QTreeWidgetItem*> items;
	if(doc.setContent(reply->readAll())) {
		QDomNode n = doc.documentElement().firstChild();
		while(!n.isNull()) {
			QDomElement e = n.toElement(); // try to convert the node to an element.
			if(!e.isNull() && e.hasAttribute("name")) {
				QTreeWidgetItem * stream_item = new QTreeWidgetItem();
				stream_item->setText(0,e.attribute("name"));
				stream_item->setToolTip(0,"Currently Playing: "+e.attribute("ct")+"\nStream Type: "+e.attribute("mt")+"\nBitrate: "+e.attribute("br")+"\nShoutcast ID: "+e.attribute("id")+"\nNumber of Listeners: "+e.attribute("lc"));
				streams.insert(e.attribute("name"),
					QUrl("http://www.shoutcast.com/sbin/tunein-station.pls?id="+e.attribute("id")));
				items.append(stream_item);
				
			}
			n = n.nextSibling();
		}	
	}
	requests[reply]->addChildren(items);
	requests[reply]->sortChildren(0,Qt::AscendingOrder);
	requests[reply]->setIcon(0,QIcon());
	requestsCompleted.append(requests[reply]);
	requests.remove(reply);
}

void Shoutcast::processStream() {
	QList<QTreeWidgetItem *> items = tree->selectedItems();
	QTreeWidgetItem * item = items.first();
	if(streams.contains(item->text(0))) {
		qDebug()<<streams[item->text(0)];
		connect(manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(recievedStream(QNetworkReply*)));
		QNetworkReply * reply = manager->get(QNetworkRequest(streams[item->text(0)]));
		connect(reply, SIGNAL(downloadProgress(qint64,qint64)),this,SIGNAL(progress(qint64,qint64)));
	}
}

void Shoutcast::recievedStream(QNetworkReply * reply) {
	disconnect(manager, 0, this, 0);
	
	QList<QUrl> toAdd;
	QString stream(reply->readAll());
	stream = stream.simplified();
	int begin = 0;
	int end = 0;
	while(stream.indexOf("http://",begin)!=-1) {
		begin = stream.indexOf("http://",begin);
		end = stream.indexOf(' ',begin);
		toAdd.append(QUrl(stream.mid(begin,end-begin)));
		begin = end;
	}
	
	//Possible options "Append to Playlist"<<"New Playlist"<<"New Collection";
	if(addMethod->currentText()=="Append to Playlist") {
		foreach(QUrl station, toAdd) {
			qDebug()<<"Adding "<<station.toString()<<" to current playlist";
			conn->playlist.addUrl(station.toString().toStdString());
		}
		//conn.playlist
	}
	else if(addMethod->currentText()=="New Playlist") {
		
	}
}


//http://www.shoutcast.com/sbin/newxml.phtml?search=Techno

void Shoutcast::fetchQuery() {
	genres.clear();
	requestsCompleted.clear();
	streams.clear();
	tree->clear();
	disconnect(tree,0,this,0);
	
	QString str = search->text();
	if(searchButton->isChecked()) {
		if(!bitrate->text().remove("*").isEmpty()) str += "&br=" + bitrate->text().remove("*");
		if(!streamtype->text().remove("*").isEmpty()) str += "&mt=" + streamtype->text().remove("*");
		if(!genre->text().remove("*").isEmpty())str += "&genre=" + genre->text().remove("*");
		if(!limit->text().remove("*").isEmpty())str += "&limit=" + limit->text().remove("*");
	}
	currentSearch = "Search="+str;
	connect(manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(recievedQuery(QNetworkReply*)));
	QNetworkReply * reply = manager->get(QNetworkRequest(QUrl("http://www.shoutcast.com/sbin/newxml.phtml?search="+str)));
	qDebug()<<QUrl("http://www.shoutcast.com/sbin/newxml.phtml?search="+str);
	connect(reply, SIGNAL(downloadProgress(qint64,qint64)),this,SIGNAL(progress(qint64,qint64)));
}

void Shoutcast::recievedQuery(QNetworkReply * reply) {
	disconnect(manager, 0, this, 0);
	//qDebug()<<"fetched_search_1";
	
	QDomDocument doc("streams");
	QTreeWidgetItem * search = new QTreeWidgetItem();
	search->setText(0,currentSearch);
	tree->addTopLevelItem(search);
	
	QList<QTreeWidgetItem*> items;
	//I don't like the cut and paste ... but ....
	if(doc.setContent(reply->readAll())) {
		//qDebug()<<"fetched_search_2";
		QDomNode n = doc.documentElement().firstChild();
		while(!n.isNull()) {
			QDomElement e = n.toElement(); // try to convert the node to an element.
			if(!e.isNull() && e.hasAttribute("name")) {
				QTreeWidgetItem * stream_item = new QTreeWidgetItem();
				stream_item->setText(0,e.attribute("name"));
				stream_item->setToolTip(0,"Currently Playing: "+e.attribute("ct")+"\nStream Type: "+e.attribute("mt")+"\nBitrate: "+e.attribute("br")+"\nShoutcast ID: "+e.attribute("id")+"\nNumber of Listeners: "+e.attribute("lc"));
				streams.insert(e.attribute("name"),
					QUrl("http://www.shoutcast.com/sbin/tunein-station.pls?id="+e.attribute("id")));
				items.append(stream_item);
				
			}
			n = n.nextSibling();
		}	
	}
	search->addChildren(items);
	//Shoutcast gives the list sorted by number of listeners
	//search->sortChildren(0,Qt::AscendingOrder);
}




/******* Drag Drop *******/
//I do not know a way to do drag and drop intuitively for shoutcast, because each stream is really a 
//playlist, but the user doesn't know how many items are in a stream

/*
QMimeData* Shoutcast::getMimeData(QList<QTreeWidgetItem*> items) {
	QList<QUrl> stationlist;
	foreach(QTreeWidgetItem * item, items) {
		if(streams.contains(item->text(0))) {
		qDebug()<<"Data"<<streams[item->text(0)];
		stationlist.append(streams[item->text(0)]);
		}
	}	
	QMimeData * mimeData = new QMimeData;
	mimeData->setUrls(stationlist);
	return mimeData;
}

// //Dragging
// QMimeData* MediaLib::getCurrentMimeData() {
// 	QMimeData * mimeData = new QMimeData;
// 	mimeData->setUrls(urlList);
// // 	std::cout<<urlList.size()<<std::endl;
// 	
// 	QByteArray encodedData;
// // 	QDataStream stream(&encodedData, QIODevice::WriteOnly);
// 	Xmms::Coll::Coll* tmp = new Xmms::Coll::Union(*selectedAsColl());
// // 	std::cout<<tmp<<std::endl;
// // 	stream << tmp;	
// // 	encodedData.append((char*)tmp);
// 	encodedData =QByteArray::number((qlonglong)tmp);
// // 	std::cout<<tmp<<std::endl;
// 
// 	mimeData->setData("application/x-collstruct", encodedData);
// 	return mimeData;
// }

DragTreeWidget::DragTreeWidget(Shoutcast* newShout):QTreeWidget() {
	setDragDropMode(QAbstractItemView::DragOnly);
	setAcceptDrops(false);
	shout = newShout;
	setFocusPolicy(Qt::StrongFocus);
}

DragTreeWidget::~DragTreeWidget() {
//CODE THIS!!!
}

void DragTreeWidget::dragEnterEvent(QDragEnterEvent *event) {
	if(event->mimeData()->hasUrls() || event->mimeData()->hasFormat("application/x-collname") && event->source()!=this)
	event->acceptProposedAction();
	else
	event->ignore();
}

//Nothing to do here
void DragTreeWidget::dragMoveEvent ( QDragMoveEvent * event) {
}


//Time to actually do some shite
void DragTreeWidget::dropEvent(QDropEvent *event){
	if(event->source()==shout ||event->source()==this) {
	event->ignore();
	return;
	}*//*
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
	}*/
/*	
}


QStringList DragTreeWidget::mimeTypes() const {
	QStringList types;
	types << "text/plain"<<"text/uri-list" << "application/x-collname" <<"application/x-collstruct";
	return types;
}

QMimeData* DragTreeWidget::mimeData(const QList<QTreeWidgetItem *> items) const {
	return shout->getMimeData(items);
}
*/

#endif
