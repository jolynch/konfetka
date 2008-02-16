#ifndef CONTEXTINFO_CPP
#define CONTEXTINFO_CPP
#include "contextinfo.h"

ContextInfo::ContextInfo(DataBackend * c,QWidget * parent, Qt::WindowFlags f):QWidget(parent,f) {
	conn = c;
	mlib = (MlibData*)(conn->getDataBackendObject(DataBackend::MLIB));

	QGridLayout * layout = new QGridLayout();
	
	tree = new EventTree();
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
	tree->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	artistHeader = new QTreeWidgetItem(tree);
	artistHeader->setText(0,"Artist: ");
	tree->setMouseTracking(true);

	layout->addWidget(tree,0,0);
	this->setLayout(layout);
	connect(mlib,SIGNAL(infoChanged(int)),this,SLOT(infoChanged(int)));
	connect(conn,SIGNAL(currentId(int)),this,SLOT(slotUpdateInfo(int)));
}
void ContextInfo::infoChanged(int id) {
	if(id == curId) {
		delete artistHeader;
		artistHeader = new QTreeWidgetItem(tree);
		
		if(mlib->getInfo("artist",id).toInt() != -1)
		artistHeader->setText(0,"Artist: " + mlib->getInfo("artist",id).toString());
		else
		artistHeader->setText(0,"Artist: Unknown");
		constructArtist();
	} else if (idToItem.contains(id)) {
		QString tmp = idToItem.value(id)->text(0) + " (" + mlib->getInfo("time",id).toString()+")";
		idToItem.value(id)->setText(0,tmp);
	}
		
}

void ContextInfo::slotUpdateInfo(int id){
	if(curId == id) return;
	curId = id;
	
	delete artistHeader;
	artistHeader = new QTreeWidgetItem(tree);
	
	if(mlib->getInfo("id",id).toInt() != -1) infoChanged(id);
}


void ContextInfo::setInfo(int id) {
	slotUpdateInfo(id);
}

void ContextInfo::constructArtist() {
	idToItem.clear();
	albumToItem.clear();
	setUpdatesEnabled(false);
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
		tmp = QString((list->get<std::string>("album")).c_str());
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
			conn->collection.queryInfos(songItems,what)(boost::bind(&ContextInfo::constructAlbum,this,newAlbum,_1));
	}
	artistHeader->setExpanded(1);
	return true;
}


bool ContextInfo::constructAlbum(QTreeWidgetItem* album,const Xmms::List <Xmms::Dict> &list) {
	QHash<QString,int> songList; QString tmp; int tmpId;
	for (list.first();list.isValid(); ++list) {
		if(list->contains("id")&&list->contains("title")) {
		tmp = QString(list->get<std::string>("title").c_str());
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
				album->setFont(0,f);
			}
		album->addChild(item);
		idToItem.insert(songList.value(key),item);
		if(mlib->getInfo("picture_front",songList.value(key)).toString()!="Unknown") {
		conn->bindata.retrieve(mlib->getInfo("picture_front",songList.value(key)).toString().toStdString())
						(boost::bind(&ContextInfo::gotAlbumCover,this,songList.value(key),_1));
		QIcon icon;
// 		std::cout<<(album->icon(0)==icon)<<std::endl;
		} else {
		cntr-=1;
		if(cntr<=1)
		setUpdatesEnabled(true);
		}
	}
/*	album->setExpanded(true);*/
	return true;
}

bool ContextInfo::gotAlbumCover(int id,const Xmms::bin& res) {
	cntr-=1;
		if(cntr<=1)
		setUpdatesEnabled(true);
		
		if(albumToItem.contains(mlib->getInfo("album",id).toString())) return true;
	QBuffer buffer;	
	buffer.setData((const char*)(res.c_str()), res.size());
	QImage tmp = QImage::fromData(buffer.data());
	QIcon icon(QPixmap::fromImage(tmp));
	idToItem.value(id)->parent()->setIcon(0,icon);
	albumToItem.insert(mlib->getInfo("album",id).toString(),idToItem.value(id)->parent());
	return true;
}

void EventTree::mouseMoveEvent(QMouseEvent * event) {
	int delta = -1000;
	int sensitivity = 10;
	if(event->y()<sensitivity) {
	delta = -1;
	}
	else if(event->y()+ (sensitivity+1)*2 + header()->height() > (height())) {
	delta = 1;
	}
	if(delta!=-1000)
	verticalScrollBar()->setValue(verticalScrollBar()->value()+delta);
	
	QTreeWidget::mouseMoveEvent(event);
	event->ignore();
}

#endif
