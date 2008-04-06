#ifndef CONTEXTINFO_CPP
#define CONTEXTINFO_CPP
#include "contextinfo.h"

ContextInfo::ContextInfo(DataBackend * c,bool autoUpdate,QWidget * parent, Qt::WindowFlags f):QWidget(parent,f) {
	conn = c; 
	mlib = (MlibData*)(conn->getDataBackendObject(DataBackend::MLIB));

	QGridLayout * layout = new QGridLayout();
	
	tree = new QTreeWidget();
	QPalette pal = tree->palette();
        pal.setColor(QPalette::Base,pal.color(QPalette::Window));
        tree->setPalette(pal);
	tree->setFrameStyle(QFrame::StyledPanel|QFrame::Sunken);
	tree->setIconSize(QSize(50,50));
	QFont fo = qApp->font();
	fo.setPointSize(10);
	setFont(fo);
	setBackgroundRole(QPalette::Window);
	QStringList Hlabels;
	Hlabels << "Context Infomation";
	tree->setHeaderLabels(Hlabels);
	tree->setMouseTracking(true);
	artistHeader = new QTreeWidgetItem(tree);
	artistHeader->setText(0,"Artist: ");

	layout->addWidget(tree,0,0);
	this->setLayout(layout);
	if(autoUpdate) {
		connect(mlib,SIGNAL(infoChanged(int)),this,SLOT(infoChanged(int)));
		connect(conn,SIGNAL(currentId(int)),this,SLOT(slotUpdateInfo(int)));
	}	
	connect(tree,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(addToPlist(QTreeWidgetItem*)));
	connect(conn,SIGNAL(qsettingsValueChanged(QString,QVariant)),this,SLOT(respondToConfigChange(QString,QVariant)));
	doubleClickPlay = true;
}

void ContextInfo::clearTree() {
	QTreeWidgetItem * tmp;
	while(artistHeader->childCount()>0) {
		tmp = artistHeader->takeChild(0);
		if(tmp) delete tmp;
	}	
}
void ContextInfo::infoChanged(int id) {
	if(id == curId) {
		idToItem.clear();
		albumToItem.clear();
		clearTree();
		
		if(mlib->getInfo("artist",id).toInt() != -1)
		artistHeader->setText(0,"Artist: " + mlib->getInfo("artist",id).toString());
		else
		artistHeader->setText(0,"Artist: Unknown");
		constructArtist();
	} else if (idToItem.contains(id)) {
		QString tmp = mlib->getInfo("album",id).toString() + " (" + mlib->getInfo("time",id).toString()+")";
		idToItem.value(id)->setText(0,tmp);
	}
		
}

void ContextInfo::slotUpdateInfo(int id){
	if(curId == id) return;
	curId = id;
	
	setUpdatesEnabled(false);
	//In case we get some weird problems later
	QTimer::singleShot(4000, this, SLOT(setUpdatesEnabled()));
	
	if(mlib->getInfo("id",id).toInt() != -1) infoChanged(id);
}


void ContextInfo::setInfo(int id) {
	slotUpdateInfo(id);
}

void ContextInfo::constructArtist() {
	Xmms::Coll::Universe univ;
	Xmms::Coll::Equals albumItems(univ,"artist",mlib->getInfo("artist",curId).toString().toStdString(),true);
	std::list<std::string> what;
	what.push_back("album");
	conn->collection.queryInfos(albumItems,what)(Xmms::bind(&ContextInfo::gotAlbums,this));
}

bool ContextInfo::gotAlbums(const Xmms::List <Xmms::Dict> &list) {
	QStringList albumList; QString tmp;
	for (list.first();list.isValid(); ++list) {
		if(list->contains("album")) {
		tmp = QString::fromUtf8((list->get<std::string>("album")).c_str());
		}
		else {
		continue;
		}
			if(albumList.contains(tmp))continue;
		albumList.append(tmp);
		}
	qSort(albumList);
	foreach(QString key,albumList) {
		QTreeWidgetItem * newAlbum = new QTreeWidgetItem(artistHeader);		
		newAlbum->setText(0,key);
		Xmms::Coll::Universe univ;
		Xmms::Coll::Equals albumItems(univ,"artist",mlib->getInfo("artist",curId).toString().toStdString(),true);
		Xmms::Coll::Equals songItems(albumItems,"album",key.toStdString(),true);
		std::list<std::string> what;
		what.push_back("title");
		what.push_back("id");
			lastID = curId;
			conn->collection.queryInfos(songItems,what)(boost::bind(&ContextInfo::constructAlbum,this,newAlbum,_1));
	}
	artistHeader->setExpanded(1);
	if(albumList.size()<1) setUpdatesEnabled(true);
	return true;
}


bool ContextInfo::constructAlbum(QTreeWidgetItem* album,const Xmms::List <Xmms::Dict> &list) {
	if(album == NULL || lastID != curId || artistHeader->indexOfChild(album)<0) return false;
	QHash<QString,int> songList; QString tmp; int tmpId;
	for (list.first();list.isValid(); ++list) {
		if(list->contains("id")&&list->contains("title")) {
		tmp = QString::fromUtf8(list->get<std::string>("title").c_str());
		tmpId = list->get<int>("id");
			if(songList.contains(tmp)) {
				continue;
			}
		songList.insert(tmp,tmpId);
		}
	}
	QList<QString> keys = songList.keys(); 
	qSort(keys);
	cntr = keys.size();
	foreach(QString key,keys) {
		QTreeWidgetItem* item = new QTreeWidgetItem();
			if(mlib->getInfo("time",songList.value(key)).toInt()!=-1)
			tmp = key + " (" + mlib->getInfo("time",songList.value(key)).toString()+")";
			else
			tmp = key;
		item->setText(0,tmp);
			if(songList.value(key)==curId) {
				QFont f = tree->font();
				f.setBold(true);
				item->setFont(0,f);
				///This is causing segfaults
				album->setFont(0,f);
			}
			
		album->addChild(item);
		idToItem.insert(songList.value(key),item);
		if(mlib->getInfo("picture_front",songList.value(key)).toString()!="Unknown") {
		conn->bindata.retrieve(mlib->getInfo("picture_front",songList.value(key)).toString().toStdString())
						(boost::bind(&ContextInfo::gotAlbumCover,this,songList.value(key),_1));
// 		std::cout<<(album->icon(0)==icon)<<std::endl;
		} else {
		cntr -= 1; if(cntr<=1) setUpdatesEnabled(true);
		}
	}
	if(keys.size()<1) setUpdatesEnabled(true);
	return true;
}

void ContextInfo::setUpdatesEnabled(bool w) {
	QWidget::setUpdatesEnabled(w);
}

void ContextInfo::addToPlist(QTreeWidgetItem * item) {
	int num = ((PlistData*)(conn->getDataBackendObject(DataBackend::PLIST)))->getPlist()->rowCount();
	if(idToItem.key(item)!=0) {
		conn->playlist.addId(idToItem.key(item));
		if(doubleClickPlay) {
			conn->playlist.setNext(num);
			conn->playback.tickle();
		}
	}
	else { //It's an album if not in the idToKey hash
		Xmms::Coll::Universe univ;
		Xmms::Coll::Equals albumItems(univ,"artist",mlib->getInfo("artist",curId).toString().toStdString(),true);
		Xmms::Coll::Equals songItems(albumItems,"album",item->text(0).toStdString(),true);
		std::list<std::string> order;
		order.push_back("title");
		conn->collection.queryIds(songItems,order)(Xmms::bind(&ContextInfo::addAlbumToPlist,this));
	}
}

bool ContextInfo::addAlbumToPlist(const Xmms::List <uint> &list) {
	int num = ((PlistData*)(conn->getDataBackendObject(DataBackend::PLIST)))->getPlist()->rowCount();
	for(list.first();list.isValid();++list) {
		conn->playlist.addId(*list);
	}
	if(doubleClickPlay) {
		conn->playlist.setNext(num);
		conn->playback.tickle();
	}
	return true;
}

//Need this to figure out whether or not double clicking should play the song or just append it
void ContextInfo::respondToConfigChange(QString key,QVariant value) {
	if(key == "konfetka/contextInfoDoubleClickPlay") {
		doubleClickPlay = value.toBool();
	}
}

bool ContextInfo::gotAlbumCover(int id,const Xmms::bin& res) {
	cntr -= 1; if(cntr<=1) setUpdatesEnabled(true);	
	
	if(albumToItem.contains(mlib->getInfo("album",id).toString())) return true;
	
	QBuffer buffer;	
	buffer.setData((const char*)(res.c_str()), res.size());
	//Icons come from Pixmaps which come from Images which come from char arrays ... oh yippee
	QIcon icon(QPixmap::fromImage(QImage::fromData(buffer.data())));
	if(idToItem.value(id) == 0 || idToItem.value(id)->parent() == 0) return false;
	idToItem.value(id)->parent()->setIcon(0,icon);
	albumToItem.insert(mlib->getInfo("album",id).toString(),idToItem.value(id)->parent());
	return true;
}

#endif
