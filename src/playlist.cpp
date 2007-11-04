
#ifndef PLAYLISTFRONTEND_CPP
#define PLAYLISTFRONTEND_CPP
#include "playlist.h"
Playlist::Playlist(DataBackend * c, QWidget * p):QTreeWidget(p)
	{
	QPalette pal = palette();
	pal.setColor( QPalette::Active, QPalette::Base,QColor(240, 247, 255));
	this->setPalette(pal); 

	clear();
	items=0;
	curItem = NULL;
	conn=c;

		std::cout<<this <<" "<<conn->getMainPlaylist()<<std::endl;
	

	labels<<"Title"<<"Artist"<<"Album"<<"Time";
	QSettings s;
	QStringList tmp=s.value("konfetka/playlistValues").toStringList();
	if(tmp.isEmpty()) {
		s.setValue("konfetka/playlistValues",QVariant(labels));
	}
	else {
	labels = tmp;
	}
	setHeader(labels);
	
	playRand=false;
	if(s.contains("playListPlayRand"))
	playRand=s.value("playListPlayRand").toBool();
	s.setValue("playListPlayRand",QVariant(playRand));

	editing=false;
	this->setSortingEnabled(false);
	this->setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	QHeaderView * horiz=this->header();
// 	horiz->setClickable(false);
	horiz->setMovable(false);
	horiz->setVisible(true);
	this->setDragDropMode(QAbstractItemView::DragDrop);
	this->setDropIndicatorShown(true);
	this->setDragDropOverwriteMode(false);
	this->setAcceptDrops(true);
	this->setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOn);

	connect(conn,SIGNAL(playlistChanged(const Xmms::Dict&)),
							this,SLOT(changeResponse(const Xmms::Dict&)));
	connect(conn,SIGNAL(currentId(int)),this,SLOT(refresh (int)));
	connect(this,SIGNAL(itemDoubleClicked ( QTreeWidgetItem *, int)  ),
					this,SLOT(slotDoubleClicked(QTreeWidgetItem *, int )));
	connect(&timer, SIGNAL(timeout()),this, SLOT(fadeItems()));
	connect(conn,SIGNAL(newSong(Xmms::PropDict)),this,SLOT(highlightCurrent(Xmms::PropDict)));
	
	connect(conn,SIGNAL(aboutToChangeSong()),this,SLOT(respondToSongAboutToEnd()));
	connect(conn,SIGNAL(changeSong(int)),this,SLOT(changeSongRel(int)));

	conn->playlist.currentActive()(Xmms::bind(&DataBackend::getCurrentPlaylist,conn));
	setObjectName ("playlist");
	curItem = NULL;

	randNum=-1; 
	}

void Playlist::respondToSongAboutToEnd() {
	if(conn->getMainPlaylist() != this) return;
		
	if(playRand) {
	changeSongRel();
	}
}

void Playlist::highlightCurrent(Xmms::PropDict info) {
	int val = info.get<int>("id");
	val = rowToId.key(val);
	if(val!=pos && currentPosOverride) {
	val=pos;
	currentPosOverride = false;
	}
	QFont f;
	curRow = val;

	if(curItem != NULL) {
	f = curItem->font(0);
	f.setBold(false);
	for(int i=0;i<curItem->columnCount();i++)
	curItem->setFont(i,f);
	}

	curItem = topLevelItem(val);
	if(curItem==NULL) return;
	f = curItem->font(0);
	f.setBold(true);
	for(int i=0;i<curItem->columnCount();i++)
	curItem->setFont(i,f);

	if(!havePlayed.isEmpty() && havePlayed.top()==val)
	havePlayed.pop();
	havePlayed.push(val);
	scrollToItem(curItem,QAbstractItemView::PositionAtCenter); 
}

void Playlist::currentPos(const unsigned int& val) {
	pos=val;
	currentPosOverride = true;
	//This might be usefull in the future
}

// QStandardItemModel * Playlist::back() {return backend;}

void Playlist::showDuplicates() {
	QList<QTreeWidgetItem*> duplicates;
	QList<int> idList;
	checkedItems.clear();
	fadingItems.clear();
	timer.stop();
	QFont f = QApplication::font();
	f.setBold(true);
	for(int i=0;i<topLevelItemCount();i++) {
		idList.append(rowToId[i]);
	}
	for(int j=0;j<topLevelItemCount();j++) {
		if(idList.count(rowToId[j])>1) {
			int last=-1;
			for(int m=0;m<idList.count(rowToId[j]);m++) {
			last = idList.indexOf(rowToId[j],last+1);
				if(last!=-1) {
				duplicates.append(topLevelItem(last));
				topLevelItem(last)->setCheckState(0,Qt::Checked);
				checkedItems.append(topLevelItem(last));
				}
			}
			int first = idList.indexOf(rowToId[j],0);
			topLevelItem(first)->setCheckState(0,Qt::Unchecked);
			checkedItems.removeAt(checkedItems.indexOf(topLevelItem(first)));
		}
	}
	for(int n=0;n<topLevelItemCount();n++) {
		if(duplicates.contains(topLevelItem(n))) {
			for(int o=0;o<topLevelItem(n)->columnCount();o++) {
			topLevelItem(n)->setFont(o,f);
			topLevelItem(n)->setForeground(o,QBrush(QColor(0,0,0,255),Qt::SolidPattern));
			}
		}
		else {
/*			for(int p=0;p<topLevelItem(n)->columnCount();p++) {*/
		fadingItems.append(topLevelItem(n));
		topLevelItem(n)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled|Qt::ItemIsEnabled);
// 			topLevelItem(n)->setForeground(p,QBrush(QColor(160,160,160,255),Qt::SolidPattern));
/*			}*/
		}
	}
	curColor = 0;
	timer.start(33);
// 	QTimer::singleShot(30, this, SLOT(quit()));
}

void Playlist::resetColors() {
	QFont f = QApplication::font();
	for(int i =0;i<topLevelItemCount();i++) {
		topLevelItem(i)->setCheckState(0, Qt::Unchecked);
		topLevelItem(i)->setFlags(topLevelItem(i)->flags() |Qt::ItemIsUserCheckable); 
		for(int j=0;j<topLevelItem(i)->columnCount();j++) {
			if(topLevelItem(i)==curItem) 
			f.setBold(true);
			else
			f.setBold(false);
		topLevelItem(i)->setFont(j,f);
		topLevelItem(i)->setForeground(j,QBrush(QColor(0,0,0,255),Qt::SolidPattern));
		}
	}
}

void Playlist::fadeItem(QTreeWidgetItem* item,int lastColor) {
	if(lastColor<120) {
	QFont f;
		for(int i=0;i<item->columnCount();i++) {
		item->setForeground(i,QBrush(QColor(lastColor,lastColor,lastColor,255),Qt::SolidPattern));
		}
	return;
	}
	timer.stop();
}

void Playlist::fadeItems() {
	if(fadingItems.size()==0)
	timer.stop();
	for(int i=0;i<fadingItems.size();i++) {
	fadeItem(fadingItems.value(i),curColor);
	}
	curColor+=10;	
}

void Playlist::playRandomly() {

	std::cout<< conn->getMainPlaylist()<<" "<<this<<std::endl;
	if(conn->getMainPlaylist() != this) return;

	std::cout<<"toggling "<<this<<std::endl;
	
	playRand = !playRand;
	QSettings s;
	s.setValue("playListPlayRand",QVariant(playRand));	

	if(playRand) {
	conn->playlist.setNext(randNum)(Xmms::bind(&DataBackend::scrapResultI, conn)); 
	}
	else {
	conn->playlist.setNext(indexOfTopLevelItem(curItem))(Xmms::bind(&DataBackend::scrapResultI, conn)); 
	}
}


void Playlist::refresh (int id)
	{
	//curRow = id;
	}

bool Playlist::refreshList(const Xmms::List< unsigned int > &list)
	{
	rowToId.clear();
	havePlayed.clear();
	haveNotPlayed.clear();
	int i=0;
	curItem = NULL;
	clear();

	for (list.first (); list.isValid (); ++list, ++i)
		{
		rowToId.insert(i,*list);
		////std::cout<<*list<<" "<<i<<std::endl;
		conn->medialib.getInfo(*list)(boost::bind(&Playlist::enterInfo,this,_1,i));
		}
	haveNotPlayed.fill("1",i);
	return true;
	}

void Playlist::toggleEdit()
	{
	editing = !editing;
	std::cout<<editing<<" "<<conn->getMainPlaylist()<<" "<<std::endl;
	if(!editing) conn->setMainPlaylist(this);
	}

bool Playlist::isEditing()
	{return editing;}

bool Playlist::enterInfo(const Xmms::PropDict & info, int row)
	{
	checkableQTreeWidgetItem * newItem = new checkableQTreeWidgetItem();
	addTopLevelItem(newItem);
	newItem->setFlags(newItem->flags() | Qt::ItemIsUserCheckable);
	newItem->setCheckState(0, Qt::Unchecked);
	int id=info.get<int>("id");
	
	rowToId.insert(row,id);
// 		else {
// 		  delete newItem;
// 		  return false;
// 		}
	////std::cout<<info.get<std::string>("title")<<std::endl;

	////std::cout<<id<<std::endl;
	QString qfilename=QString(info.get<std::string>("url").c_str());
	for(int i=0; i<labels.size(); i++)
		{
		QString inf;
		if(labels[i]=="duration"||labels[i]=="tracknr")
			{
			int num=0;
			try{num=info.get<int>(labels[i].toStdString());}
			catch(Xmms::no_such_key_error& err) {num=0;}
			if(labels[i]=="duration")
				{
				QTime foo (0,0,0);
				foo=foo.addMSecs(num);
				if(foo.hour()>0)
					inf=foo.toString("h:mm:ss");
				else
					inf=foo.toString("mm:ss");
				}
			else
				inf.setNum(num);
			}
		else if(labels[i]=="url")
			{
			inf=QString((info.get<std::string>(labels[i].toStdString())).c_str());
			if(inf.startsWith("file://"))
				{
				inf=inf.remove(0,inf.lastIndexOf("/")+1);
				}
			if(inf.startsWith("http://")||inf.startsWith("ftp://"))
				inf=QString("<Internet stream>");
			}
		else
			{
			try{inf=QString((info.get<std::string>(labels[i].toStdString())).c_str());}
			catch(Xmms::no_such_key_error& err) {inf=QString(" ");}
			}
    
		if(!inf.isEmpty()) {
		newItem->setText(i,inf);
		}
		}

// 		//std::cout<<row<<" "<<curId<<std::endl;
// 		//std::cout<<mlibIdToPlistId[curId]<<" "<<row<<std::endl;
		if(curRow==row) {
			curItem=newItem;
			QFont f = newItem->font(0);
			f.setBold(true);
			for(int i=0;i<newItem->columnCount();i++)
			newItem->setFont(i,f);	
		}
	
	return true;
	}

void Playlist::slotDoubleClicked(QTreeWidgetItem * item, int column )  
	{
	if(editing) return;
	conn->playlist.setNext(indexOfTopLevelItem(item))(Xmms::bind(&DataBackend::scrapResultI, conn));
	conn->playback.tickle()(Xmms::bind(&DataBackend::scrapResult, conn));
}

void Playlist::changeResponse(const Xmms::Dict& val)
	{
// 	if (val.contains ("title"))
// 	//std::cout<<val.get<std::string>("title")<<std::endl;
	int pos = 0, npos = 0;
	uint32_t id = 0;
	if (val.contains ("position"))
		pos = val.get<int32_t> ("position");
	if (val.contains ("id"))
		id = val.get<uint32_t> ("id");
	QTreeWidgetItem* tmp;
	switch (val.get<int32_t> ("type"))
		{
		case XMMS_PLAYLIST_CHANGED_ADD: 
// 			//std::cout<<"adding2"<<id<<std::endl;
			conn->medialib.getInfo(id)(boost::bind(&Playlist::enterInfo,this,_1,id));
			haveNotPlayed.append(pos);
			break;
		case XMMS_PLAYLIST_CHANGED_INSERT:
			conn->medialib.getInfo(id)(boost::bind(&Playlist::enterInfo,this,_1,pos));
			break;
		case XMMS_PLAYLIST_CHANGED_MOVE: {
			npos = val.get<int32_t> ("newposition");
			tmp= takeTopLevelItem(pos);
			insertTopLevelItem(npos,tmp);
			int tmp2 = rowToId.value(pos);
			rowToId.remove(pos);
			rowToId.insert(npos,tmp2);
			break;
			}
		case XMMS_PLAYLIST_CHANGED_REMOVE: {
			tmp = topLevelItem(pos);
			if(tmp == NULL) return;
			delete tmp;
			rowToId.remove(pos);
			haveNotPlayed.remove(pos);
				if(tmp==curItem) {
					emit itemDoubleClicked (topLevelItem((pos+1)%topLevelItemCount()),0);
					curItem=topLevelItem((pos+1)%topLevelItemCount());
				}
			break;
			}
		case XMMS_PLAYLIST_CHANGED_SHUFFLE:
		case XMMS_PLAYLIST_CHANGED_SORT:
		case XMMS_PLAYLIST_CHANGED_CLEAR:
		default:
			conn->playlist.listEntries(currentPlist)
						(Xmms::bind (&Playlist::refreshList, this));
			break;
		}
// 	this->updateT();
	}

void Playlist::getCurrentName(const std::string & name)
	{
// 	//std::cout<<"GETTING LIST"<<name<<std::endl;
	clear();
	currentPlist=name;
	conn->playlist.listEntries(currentPlist)(Xmms::bind(&Playlist::refreshList,this));
	}

void Playlist::setHeader(QList<QString> info)
	{
	setHeaderLabels(QStringList(info));
	labels.clear();
	int i=0;
	for(i=0; i<info.size(); i++)
		{
		QString lbl=info[i].toLower();
		if(lbl=="filename") lbl=QString("url");
		if(lbl=="track") lbl=QString("tracknr");
		if(lbl=="time") lbl=QString("duration");
		labels.append(lbl);
		}
	items=i;
	}

int Playlist::itemCount()
	{return items;}


void Playlist::dragEnterEvent(QDragEnterEvent *event)
	{
	event->acceptProposedAction();
	}

void Playlist::dragMoveEvent ( QDragMoveEvent * ) {
doAutoScroll();
}

void Playlist::dropEvent(QDropEvent *event)
	{
	if(event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")&&event->source()==this)
		{
		int idx=indexAt(event->pos()).row();
		QByteArray encoded = event->mimeData()->data("application/x-qabstractitemmodeldatalist");
		QDataStream stream(&encoded, QIODevice::ReadOnly);
		QVector<int> rows;
		while (!stream.atEnd())
			{
			int r, c;
			QMap<int, QVariant> v;
			stream >> r >> c >> v;
			if(!rows.contains(r))
				rows.append(r);
			}
		QList<int> rows_=rows.toList();
		for (int i = 0; i < rows_.size(); ++i)
			{
			conn->playlist.moveEntry(rows_[i],idx,currentPlist) (Xmms::bind(&DataBackend::scrapResult, conn));
			if(idx>rows_[i])
				for(int j = i; j < rows_.size(); ++j)
					rows_[j]--;
			else
				idx++;
			}
		}
	else if(event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist"))
		{
		QByteArray encoded = event->mimeData()->data("application/x-qabstractitemmodeldatalist");
		QDataStream stream(&encoded, QIODevice::ReadOnly);
		QVector<int> rows,columns;
		QVector<QMap<int, QVariant> > data;
		while (!stream.atEnd())
			{
			int r, c;
			QMap<int, QVariant> v;
			stream >> r >> c >> v;
			rows.append(r);
			columns.append(c);
			data.append(v);
			}
		//for(int i=0; i<rows.size(); i++)
		//	//std::cout<<rows.at(i)<<std::endl;
		}
	/*ADDED BY JOEY - so that drag drop with mlib works*/
/*	else */if(event->mimeData()->hasUrls())
		{
		QList<QUrl> list = event->mimeData()->urls();
			for(int i=0;i<list.size();i++) {
// 			//std::cout<<"add "<<list.value(i).toString().toStdString()<<std::endl;
			conn->playlist.addUrl(list.value(i).toString().toStdString())(Xmms::bind(&DataBackend::scrapResult,conn));
			}
		}
// 	updateT();
	}
void Playlist::itemCheckStateChanged(checkableQTreeWidgetItem* item) {
	if(item->checkState(0) == Qt::Unchecked && checkedItems.contains(item))
	checkedItems.removeAt(checkedItems.indexOf(item));
	else
	checkedItems.append(item);
}

void Playlist::removeCheckedItems() {
	QSet<QTreeWidgetItem*> items = checkedItems.toSet();
	checkedItems = items.toList();
	for(int i=0;i<checkedItems.size();i++) {
// 	//std::cout<<indexOfTopLevelItem(checkedItems.value(i))<<std::endl;
	conn->playlist.removeEntry(indexOfTopLevelItem(checkedItems.value(i)),currentPlist)(Xmms::bind(&DataBackend::scrapResult, conn));
	}
}

void Playlist::changeSongRel(int loc) {
	
	if(conn->getMainPlaylist() != this || (!playRand && (curRow+1) == topLevelItemCount())) return;
	std::cout<<playRand<<" Randomly Picking next Song Because next/prev"<<std::endl;
	if(loc == -1) {
		if(playRand) {
			if(havePlayed.size()<2) {
			changeSongRel(1);
			return;
			}
			else {
			havePlayed.pop();
			conn->playlist.setNext(havePlayed.pop())(Xmms::bind(&DataBackend::scrapResultI, conn));
			}
		conn->playback.tickle()(Xmms::bind(&DataBackend::scrapResult, conn));
		}
		else {
		conn->playlist.setNextRel(-1)(Xmms::bind(&DataBackend::scrapResultI, conn));
		conn->playback.tickle()(Xmms::bind(&DataBackend::scrapResult, conn));
		}
	}
	else if (loc == 1) {
		if(playRand) {
		randomlyPickNextSong();
		}
		else {
		conn->playlist.setNextRel(1)(Xmms::bind(&DataBackend::scrapResultI, conn));
		}
	conn->playback.tickle()(Xmms::bind(&DataBackend::scrapResult, conn));
	}
	else {
	std::cout<<"ERROR: Playlist::changeSongRel(int) passed a non -1 or 1 value"<<std::endl;
	}
}

void Playlist::randomlyPickNextSong() {
	if(haveNotPlayed.size()==0) return;
	srand(time(NULL));
	randNum = -1;
		if(!haveNotPlayed.contains(1)) {
		haveNotPlayed.fill("1",haveNotPlayed.size());
		}
	randNum = haveNotPlayed.indexOf(1,rand()%(haveNotPlayed.size()));
	
		//Added to make it possible to repeat values;
		if( (rand()%2) == 1) {
		randNum = rand()%(haveNotPlayed.size()); 
		}

		if(randNum == -1)
		randNum = haveNotPlayed.indexOf(1);
	
	haveNotPlayed[randNum] = 0;
	conn->playlist.setNext(randNum)(Xmms::bind(&DataBackend::scrapResultI, conn)); 
}


void checkableQTreeWidgetItem::setData(int column, int role, const QVariant& value)
{
	if (role == Qt::CheckStateRole)
	{
		Qt::CheckState newState = static_cast<Qt::CheckState>(value.toInt());
		Qt::CheckState oldState = static_cast<Qt::CheckState>(data(column, role).toInt());
		
		QTreeWidgetItem::setData(column, role, value);
		
		if (newState != oldState)
		{
			Playlist* tree = qobject_cast<Playlist*>(treeWidget());
			if (tree)
			{
				tree->itemCheckStateChanged(this);
			}
		}
	}
	else
	{
		QTreeWidgetItem::setData(column, role, value);
	}
}
	

#endif
