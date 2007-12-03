#ifndef PLISTDATA_CPP
#define PLISTDATA_CPP
#include "plistdata.h"

PlaylistDelegate::PlaylistDelegate(QAbstractItemModel * m,DataBackend * c)
	{
	conn=c;
	model=m;
	editing=false;
	connect(conn,SIGNAL(changeStatus(Xmms::Playback::Status)),this,SLOT(statusChanged(Xmms::Playback::Status)));
	connect(conn,SIGNAL(currentPos(const unsigned int)),this,SLOT(posChanged(uint)));
	}

QSize PlaylistDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
	{
	return QSize(20,50);
	}

void PlaylistDelegate::paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
	{
	if(!index.isValid()) return;
	painter->setPen(Qt::black);
	QStyleOptionViewItem o (option);
	QFont f (o.font);
	if(((MlibData *)conn->getDataBackendObject(DataBackend::MLIB))->getInfo(QString("status"),index.internalId()).toInt()==3)
		painter->setPen(Qt::gray);
	if((!editing)&&(index.row()==pos))
		{
		f.setItalic(true);
		if(status==Xmms::Playback::PLAYING)
			f.setBold(true);
		}
	o.font = f;
	QItemDelegate::paint (painter, o, index);
	painter->setPen(Qt::black);
	}

void PlaylistDelegate::setEditing(bool val)
	{editing=val;}
bool PlaylistDelegate::isEditing() {return editing;}

void PlaylistDelegate::posChanged(uint p)
	{
	int old=pos;
	pos=p;
	if(old<model->rowCount())
		((SinglePlaylist *)model)->forceRefresh(old);
	if(p<model->rowCount())
		((SinglePlaylist *)model)->forceRefresh(p);
	}

void PlaylistDelegate::statusChanged(Xmms::Playback::Status s)
	{
	status=s;
	if(pos<model->rowCount())
		((SinglePlaylist *)model)->forceRefresh(pos);
	}

/*****************************************/

SinglePlaylist::SinglePlaylist(DataBackend * c,std::string name,QStringList hv_)
	{
	conn=c; humanReadableHeader=hv_;
	parseHumanReadableHeader();
	connected=false;
	plistName=name;
	delegate=new PlaylistDelegate(this,conn);
	connect(((MlibData *)conn->getDataBackendObject(DataBackend::MLIB)),SIGNAL(infoChanged(int)),this,SLOT(infoChanged(int)));
	}

void SinglePlaylist::parseHumanReadableHeader()
	{
	header.clear();
	for(int i=0; i<humanReadableHeader.size(); i++)
		{
		QString lbl=humanReadableHeader[i].toLower();
		if(lbl=="filename") lbl=QString("url");
		if(lbl=="track") lbl=QString("tracknr");
		if(lbl=="time") lbl=QString("duration");
		header.append(lbl);
		}
	this->reset();
	}

PlaylistDelegate * SinglePlaylist::getDelegate()
	{return delegate;}

void SinglePlaylist::setHeader(QStringList newVal)
	{
	humanReadableHeader=newVal;
	parseHumanReadableHeader();
	this->headerDataChanged(Qt::Horizontal,0,humanReadableHeader.size()-1);
	}

QModelIndex SinglePlaylist::index ( int row, int column, const QModelIndex & parent) const
	{
	if(parent.isValid()) return QModelIndex();
	if(row<0||column<0) return QModelIndex();
	if(rowCount()<=0||columnCount()<=0) return QModelIndex();
	return createIndex(row,column,ids[row]);
	}

QModelIndex SinglePlaylist::parent ( const QModelIndex & index ) const
	{return QModelIndex();}

QVariant SinglePlaylist::data ( const QModelIndex & index, int role) const
	{
	if(role!=Qt::DisplayRole) return QVariant();
	MlibData * mlib=(MlibData *)(conn->getDataBackendObject(DataBackend::MLIB));
	QVariant tmp=mlib->getInfo(header[index.column()],index.internalId());
	if(tmp.toInt()!=-1)
		return tmp;
	else
		return QString(" ");
	}

int SinglePlaylist::rowCount ( const QModelIndex & parent) const
	{
	if(parent.isValid()) return 0;
	return ids.size();
	}

int SinglePlaylist::columnCount ( const QModelIndex & parent) const
	{return header.size();}

QVariant SinglePlaylist::headerData ( int section, Qt::Orientation orientation, int role) const
	{
	if(role!=Qt::DisplayRole) return QVariant();
	if(orientation!=Qt::Horizontal) return QVariant();
	return humanReadableHeader[section];
	}

bool SinglePlaylist::removeRows ( int row, int count, const QModelIndex & parent)
	{
std::cout<<"removeRows called"<<std::endl;
	if(parent.isValid()) return false;
	if(row+count>ids.size()) return false;
	for(int i=0; i<count; i++)
		conn->playlist.removeEntry(row+1,plistName)(Xmms::bind(&DataBackend::scrapResult, conn));
	return true;
	}

bool SinglePlaylist::setInitialPlist(const Xmms::List< unsigned int > &list)
	{
	ids.clear();
	for (list.first (); list.isValid (); ++list)
		{ids.append(*list);}
	this->reset();
	return true;
	}

void SinglePlaylist::infoChanged(int id)
	{
	if(!ids.contains(id)) return;
	for(int i=ids.indexOf(id); i!=-1; i=ids.indexOf(id,i+1))
		dataChanged(index(i,0),index(i,header.size()-1));
	}

void SinglePlaylist::addToModel(uint id,int pos)
	{
	if(pos<0)
		{
		beginInsertRows(QModelIndex(),ids.size(),ids.size());
		ids.append(id);
		endInsertRows();
		}
	else
		{
		beginInsertRows(QModelIndex(),pos,pos);
		ids.insert(pos,id);
		endInsertRows();
		}
	}

void SinglePlaylist::removeFromModel(int pos)
	{
	if(pos<0||pos>ids.size()-1) return;
	beginRemoveRows(QModelIndex(),pos,pos);
	ids.removeAt(pos);
	endRemoveRows();
	}

void SinglePlaylist::moveInModel(int oldPos,int newPos)
	{
	if(oldPos==newPos) return;
	uint id=ids[oldPos];
	if(newPos>oldPos)
		newPos--;
	removeFromModel(oldPos);
	addToModel(newPos,id);
	}

void SinglePlaylist::respondToChanges(const Xmms::Dict& val)
	{
	if(val.get<std::string> ("name")!=plistName) return;
	int pos = 0, npos = 0;
	uint32_t id = 0;
	if (val.contains ("position")) pos = val.get<int32_t> ("position");
	if (val.contains ("id")) id = val.get<uint32_t> ("id");
	switch (val.get<int32_t> ("type"))
		{
		case XMMS_PLAYLIST_CHANGED_ADD:
			addToModel(id);
			break;
		case XMMS_PLAYLIST_CHANGED_INSERT:
std::cout<<"insert "<<pos<<std::endl;
			addToModel(id,pos);
			break;
		case XMMS_PLAYLIST_CHANGED_MOVE:
			npos = val.get<int32_t> ("newposition");
			moveInModel(pos,npos);
			break;
		case XMMS_PLAYLIST_CHANGED_REMOVE:
std::cout<<"removed "<<pos<<std::endl;
			removeFromModel(pos);
			break;
		default:
			conn->playlist.listEntries(plistName)(Xmms::bind (&SinglePlaylist::setInitialPlist, this));
			break;
		}
	}

Qt::ItemFlags SinglePlaylist::flags(const QModelIndex &index) const
	{
	return Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled|Qt::ItemIsEnabled;
	}

Qt::DropActions SinglePlaylist::supportedDropActions() const
	{
	return Qt::MoveAction|Qt::CopyAction;
	}


QList<uint> SinglePlaylist::getIdsFromPositions(QList <uint> pos)
	{
	QList <uint> out;
	for(int i=0; i<pos.size(); i++)
		out.append(ids[pos[i]]);
	return out;
	}

PlaylistDragInfo * SinglePlaylist::getDragInfoFromMimeData(const QMimeData *data)
	{
	if(data->hasFormat("application/x-plistdraginfo"))
		{
		PlaylistDragInfo * out = new struct PlaylistDragInfo();
		QByteArray encodedData = data->data("application/x-plistdraginfo");
		QDataStream stream(&encodedData, QIODevice::ReadOnly);
		QString name;
		QList<uint> posList;
		stream>>name;
		out->name=name;
		while (!stream.atEnd())
			{
			uint tmp;
			stream>>tmp;
			posList.append(tmp);
			}
		qSort(posList);
		out->positions=posList;
		return out;
		}
	else
		return NULL;
	}

bool SinglePlaylist::dropMimeData(const QMimeData *data,Qt::DropAction action, int row, int column, const QModelIndex &parent)
	{
std::cout<<"called dropMimeData: "<<parent.row()<<" "<<parent.column()<<std::endl;
	if (action == Qt::IgnoreAction)
		return true;
	PlaylistDragInfo * info=getDragInfoFromMimeData(data);
	if (info!=NULL)
		{
		if(info->name.toStdString()!=plistName)
			{
			if(action==Qt::CopyAction||action==Qt::MoveAction)
				{
				QList <uint> ids_=(((PlistData *)conn->getDataBackendObject(DataBackend::PLIST)))
							->getPlist(info->name.toStdString())->getIdsFromPositions(info->positions);
				for(int i=0; i<info->positions.size(); i++)
					{
					if(parent.isValid())
						conn->playlist.insertId(parent.row()+i,ids_[i],plistName)
									(Xmms::bind(&DataBackend::scrapResult, conn));
					else
						conn->playlist.addId(ids_[i],plistName)
									(Xmms::bind(&DataBackend::scrapResult, conn));
					if(action==Qt::MoveAction)
						conn->playlist.removeEntry(info->positions[i]-i,info->name.toStdString())
									(Xmms::bind(&DataBackend::scrapResult, conn));
					}
				return true;
				}
			else return false;
			}
		else if(action==Qt::CopyAction||action==Qt::MoveAction)
			{
			QList <uint> ids_=getIdsFromPositions(info->positions);
			int idx=-1;
			if(parent.isValid()) idx=parent.row();
			if(action==Qt::MoveAction)
				{
				for(int i=0; i<info->positions.size(); i++)
					{
					conn->playlist.removeEntry(info->positions[i]-i,plistName)
										(Xmms::bind(&DataBackend::scrapResult, conn));
					if(parent.isValid()&&idx>info->positions[i]-i) idx--;
					}
				}
			for(int i=0; i<ids_.size(); i++)
				{
				if(idx<0) conn->playlist.addId(ids_[i],plistName) (Xmms::bind(&DataBackend::scrapResult, conn));
				else conn->playlist.insertId(idx+1,ids_[i],plistName)(Xmms::bind(&DataBackend::scrapResult, conn));
				}
			return true;
			}
		else return true;
		}
	else if (data->hasUrls())
		{
		QList<QUrl> urls=data->urls();
		for(int i=0; i<urls.size(); i++)
			{
			std::string tmp=urls[i].toString().toUtf8().data();
			QString dirCheck=urls[i].toString();
			if(dirCheck.startsWith("file://"))
				dirCheck.remove(0,7);
			if(QDir(dirCheck).exists())
				{
				conn->playlist.addRecursive(tmp,plistName)(Xmms::bind(&DataBackend::scrapResult, conn));
				}
			else if(!parent.isValid())
				{
				conn->playlist.addUrl(tmp,plistName)(Xmms::bind(&DataBackend::scrapResult, conn));
				}
			else
				{
				conn->playlist.insertUrl(parent.row()+i,tmp,plistName)(Xmms::bind(&DataBackend::scrapResult, conn));
				}
			}
		return true;
		}
	else if (data->hasFormat("application/x-collname"))
		{
		QByteArray encodedData = data->data("application/x-collname");
		QDataStream stream(&encodedData, QIODevice::ReadOnly);
		QString title;
		QString ns;
		stream >> title;
		stream >> ns;
		if(parent.isValid())
			insertCollectionAt=-1;
		else
			insertCollectionAt=parent.row();
		if(ns.contains("PLAYLIST"))
			conn->collection.get(title.toStdString(),Xmms::Collection::PLAYLISTS)
					(Xmms::bind(&SinglePlaylist::insertCollection,this));
		else
			conn->collection.get(title.toStdString(),Xmms::Collection::COLLECTIONS)
					(Xmms::bind(&SinglePlaylist::insertCollection,this));
		return true;
		}
	else return false;
	}

bool SinglePlaylist::insertCollection(const Xmms::Coll::Coll& c)
	{
	if(insertCollectionAt<0)
			conn->playlist.addCollection(c,sortOrder,plistName)(Xmms::bind(&DataBackend::scrapResult, conn));
	else
			conn->playlist.insertCollection(insertCollectionAt,c,sortOrder,plistName)
									(Xmms::bind(&DataBackend::scrapResult, conn));
	return true;
	}

QStringList SinglePlaylist::mimeTypes() const
	{
	QStringList types;
	types << "text/uri-list" << "application/x-plistdraginfo" << "application/x-collname";
	return types;
	}

QMimeData * SinglePlaylist::mimeData(const QModelIndexList &indexes) const
	{
std::cout<<"called mimeData"<<std::endl;
	QMimeData *mimeData = new QMimeData();
	QByteArray encodedData;
	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	stream<<QString(plistName.c_str());
	foreach (QModelIndex index, indexes)
		if (index.isValid()&&index.column()==0)
			stream << index.row();
	mimeData->setData("application/x-plistdraginfo", encodedData);
	return mimeData;
	}


void SinglePlaylist::setOrder(std::list< std::string > order)
	{sortOrder=order;}

void SinglePlaylist::forceRefresh(uint row)
	{dataChanged(index(row,0),index(row,header.size()-1));}
/************************************!*********************************/

PlistData::PlistData(DataBackend * c,QObject * parent):QObject(parent)
	{
	conn=c;
	currentPlist="";
	connect(((CollData *)conn->getDataBackendObject(DataBackend::COLL)),SIGNAL(updated(QString,Xmms::Collection::Namespace)),
		this,SLOT(collectionChanged(QString,Xmms::Collection::Namespace)));
	connect(((CollData *)conn->getDataBackendObject(DataBackend::COLL)),SIGNAL(playlistsChanged(QStringList)),
		this,SLOT(playlistsChanged(QStringList)));
	connect(conn,SIGNAL(playlistNameChanged(const std::string&)),this,SLOT(setCurrentName(const std::string &)));
	connect(conn,SIGNAL(qsettingsValueChanged(QString,QVariant)),this,SLOT(qsettingsValChanged(QString,QVariant)));
	QSettings s;
	if(!s.contains("konfetka/playlistValues"))
		{
		QStringList tmp;
		tmp<<"Title"<<"Artist"<<"Album"<<"Time";
		s.setValue("konfetka/playlistValues",tmp);
		}
	if(!s.contains("konfetka/collectionImportSortOrder"))
		{
		QStringList tmp;
		tmp<<"title"<<"album"<<"artist"<<"tracknr";
		s.setValue("konfetka/collectionImportSortOrder",tmp);
		}
	}

void PlistData::createPlaylist(std::string name)
	{
std::cout<<"creating playlist: "<<name<<std::endl;
	if(plists.contains(name.c_str())) return;
	SinglePlaylist * newPlist=new SinglePlaylist(conn,name,headerVals);
	newPlist->setOrder(sortOrder);
	plists.insert(name.c_str(), newPlist);
	refreshPlaylist(newPlist,name);
	connect(conn,SIGNAL(playlistChanged(const Xmms::Dict&)),newPlist,SLOT(respondToChanges(const Xmms::Dict&)));
	}

void PlistData::refreshPlaylist(SinglePlaylist * plist, std::string name)
	{
	if(plist==NULL) return;
	conn->playlist.listEntries(name) (Xmms::bind (&SinglePlaylist::setInitialPlist, plist));
	if(name!="")
		emit playlistReady(name,plist);
	}

SinglePlaylist * PlistData::getPlist (std::string plist)
	{
	if(plist=="")
		if(currentPlist=="")
			return NULL;
		else
			plist=currentPlist;
	if(plists.contains(plist.c_str()))
		return plists[plist.c_str()];
	else
		{
		createPlaylist(plist);
		return plists[plist.c_str()];
		}
	return NULL;
	}


QString PlistData::getCurrentName()
	{return currentPlist.c_str();}


void PlistData::setCurrentName(const std::string & name)
	{
std::cout<<"current playlist: "<<name<<std::endl;
	refreshPlaylist(getPlist(name));
	currentPlist=name;
	}


void PlistData::collectionChanged(QString name, Xmms::Collection::Namespace ns)
	{
	if(ns!=Xmms::Collection::PLAYLISTS)
		return;
	if(plists.contains(name))
		refreshPlaylist(plists[name],name.toStdString());
	else
		createPlaylist(name.toStdString());
	}

void PlistData::qsettingsValChanged(QString name,QVariant newVal)
	{
	if(name=="konfetka/playlistValues")
		{
		QStringList val=newVal.toStringList();
		headerVals=val;
		QStringList keys=plists.keys();
		for(int i=0; i<keys.size(); i++)
			plists[keys[i]]->setHeader(val);
		}
	if(name=="konfetka/collectionImportSortOrder")
		{
		QStringList val=newVal.toStringList();
		std::list<std::string> out;
		QStringList keys=plists.keys();
		for(int i=0; i<val.size(); i++)
			{
std::cout<<val[i].toStdString()<<std::endl;
			out.push_back(val[i].toStdString());
			}
		for(int i=0; i<keys.size(); i++)
			plists[keys[i]]->setOrder(out);
		sortOrder=out;
		}
	}

void PlistData::playlistsChanged(QStringList newList)
	{
	for(int i=0; i<newList.size(); i++)
		{
		if(!plists.contains(newList[i]))
			createPlaylist(newList[i].toStdString());
		}
	}

#endif

