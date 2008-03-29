#ifndef COLLDATA_CPP
#define COLLDATA_CPP
#include "colldata.h"
CollData::CollData(DataBackend * c,QObject * parent):QObject(parent) 
	{
	conn=c;
	conn->collection.list(Xmms::Collection::PLAYLISTS)
		(Xmms::bind(&CollData::getPlaylistsFromServer,this));
	conn->collection.list(Xmms::Collection::COLLECTIONS)
		(Xmms::bind(&CollData::getCollectionsFromServer,this));
	connect(conn,SIGNAL(collectionsChanged(const Xmms::Dict&)),this,SLOT(respondToCollectionChangedBroadcast(const Xmms::Dict&)));
	connect(conn,SIGNAL(qsettingsValueChanged(QString,QVariant)),this,SLOT(respondToConfigChange(QString,QVariant)));
	}

void CollData::respondToConfigChange(QString name,QVariant value) {
	if(name == "konfetka/collectionImportSortOrder") {
		importOrder = value.toStringList();
	}
}

bool CollData::getPlaylistsFromServer(const Xmms::List<std::string>& list)
	{
		playlistsOnly.clear();
		for (list.first (); list.isValid (); ++list)
			{
			if((*list).substr(0,1)=="_") continue;
			playlistsOnly.append((*list).c_str());
			}
		emit playlistsChanged(playlistsOnly);
	return false;
	}

bool CollData::getCollectionsFromServer(const Xmms::List<std::string>& list)
	{
		collectionsOnly.clear();
		for (list.first (); list.isValid (); ++list)
			{
			if((*list).substr(0,1)=="_") continue;
			collectionsOnly.append((*list).c_str());
			}
		emit collectionsChanged(collectionsOnly);
	return false;
	}

//Might be more effective to actually respond to changes.
void CollData::respondToCollectionChangedBroadcast(const Xmms::Dict& change)
	{
	switch(change.get<int>("type"))
		{
		case XMMS_COLLECTION_CHANGED_UPDATE:
			if(change.contains("name") && change.contains("namespace"))
			emit(updated(change.get<std::string>("name").c_str(),change.get<std::string>("namespace").c_str()));
			break;
		default:
			Xmms::Collection::Namespace foo=change.get<std::string>("namespace").c_str();
			if(std::string(foo).find(Xmms::Collection::PLAYLISTS)!=std::string::npos)
				conn->collection.list(Xmms::Collection::PLAYLISTS)
					(Xmms::bind(&CollData::getPlaylistsFromServer,this));
			else
				conn->collection.list(Xmms::Collection::COLLECTIONS)
					(Xmms::bind(&CollData::getCollectionsFromServer,this));
		};
	}

QStringList CollData::getCollections()
	{return collectionsOnly;}

QStringList CollData::getPlaylists()
	{return playlistsOnly;}

void CollData::createCollection(const Xmms::Coll::Coll& coll,std::string name,Xmms::Collection::Namespace ns)
	{
	conn->collection.save(coll,name,ns)(Xmms::bind(&DataBackend::scrapResult, conn));
	}

QList<QString> CollData::getImportOrder() {
	return importOrder;
}


QString CollData::collAsQString(const Xmms::Coll::Coll& coll) {
	QString result;
	QString tmpStr;
		switch(coll.getType()) {
			case Xmms::Coll::REFERENCE:
			result = "Reference";
			break;
			case Xmms::Coll::UNION: {
			Xmms::Coll::OperandIterator temp = coll.getOperandIterator();
				for(temp.first();temp.valid();temp.valid()) {
				tmpStr = collAsQString((**temp));
				result += tmpStr;
				temp.next();
					if(temp.valid())
					result+= " OR ";
				}
			break;
			}
			case Xmms::Coll::INTERSECTION: {
			Xmms::Coll::OperandIterator temp = coll.getOperandIterator();
				for(temp.first();temp.valid();temp.valid()) {
				tmpStr = collAsQString((**temp));
				result += tmpStr;
				temp.next();
					if(temp.valid())
					result+= " AND ";
				}
			break;
			}
			case Xmms::Coll::COMPLEMENT: {
			result = ("!["+collAsQString(*(coll.getOperand()))+"]");
			break;
			}
			case Xmms::Coll::HAS: {
			result = ("[HAS tag "+QString(coll.getAttribute("field").c_str())+"]");
			break;
			}
			case Xmms::Coll::EQUALS: {
			result = ("["+QString(coll.getAttribute("field").c_str())+" == "+QString(coll.getAttribute("value").c_str())+"]");
			break;
			}
			case Xmms::Coll::MATCH: {
			result = ("["+QString(coll.getAttribute("field").c_str())+" ~ "+QString(coll.getAttribute("value").c_str())+"]");
			break;
			}
			case Xmms::Coll::SMALLER: {
			result = ("["+QString(coll.getAttribute("field").c_str())+" < "+QString(coll.getAttribute("value").c_str())+"]");
			break;
			}
			case Xmms::Coll::GREATER: {
			result = ("["+QString(coll.getAttribute("field").c_str())+" > "+QString(coll.getAttribute("value").c_str())+"]");
			break;
			}
			case Xmms::Coll::IDLIST: {
			result = "ID List";
			break;
			}       
			case Xmms::Coll::QUEUE: {
			result = ("Queue");
			break;
			}   
			case Xmms::Coll::PARTYSHUFFLE: {
			result = ("Party Shuffle with input collection: "+collAsQString(*(coll.getOperand())));
			break;
			}
			default:
			result = "Unrecognized Structure";
			break;	
		}
	return result;
}
#endif

