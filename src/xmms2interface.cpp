#ifndef XMMS2Interface_CPP
#define XMMS2Interface_CPP
#include "xmms2interface.h"

XMMS2Interface::XMMS2Interface(QObject * parent, const std::string &name):QObject(parent), Xmms::Client (name)
	{
	quitting=false;
	mainPlaylist = NULL;
	char * path;
	path = getenv("XMMS_PATH");
	try
		{
		if (path!=NULL)
			((Xmms::Client*)this)->connect(path);
		else
			((Xmms::Client*)this)->connect();
		}
	catch( Xmms::connection_error& err )
		{
 		std::cout << "Connection failed: " << err.what() << std::endl;
		QSettings s;
		std::string str = err.what();
			if (s.value("autostart").toBool() && std::string(err.what())=="xmms2d is not running." || !s.contains("autostart")) {
			QString command = "xmms2-launcher";
			QStringList arguments;
			bool first=true;
			while(QProcess::execute(command,arguments)) {
			std::cout<<command.toStdString();
			if(arguments.size()>0)std::cout<<arguments.at(0).toStdString();
			std::cout<<std::endl;
			int ret = QMessageBox::critical	(NULL,
						"Problem Connecting",
						"Cannot Connect to Server\nTry again?",
						QMessageBox::Yes | QMessageBox::No,
						QMessageBox::No);
				if(ret==(QMessageBox::No)) {
				qApp->quit();
				return;
				}
				
				if(ret==(QMessageBox::Yes)&&first) {
				first = false;
				
				ret = QMessageBox::critical	(NULL,
							"Are You Root",
							"Are You Root",
							QMessageBox::Yes | QMessageBox::No,
							QMessageBox::No);
					if(ret==QMessageBox::Yes)
					arguments<<"--yes-run-as-root";
				}
                        }
			
			if (path!=NULL)
			((Xmms::Client*)this)->connect(path);
			else
			((Xmms::Client*)this)->connect();
		
			}

		}
	this->setMainloop (new XmmsQT4 (this->getConnection ()));
	this->playback.broadcastCurrentID()(Xmms::bind(&XMMS2Interface::newSongResponse, this));
	this->playback.broadcastStatus()(Xmms::bind(&XMMS2Interface::getstatus, this));
	this->playback.getPlaytime()(Xmms::bind(&XMMS2Interface::getCurPlaytime, this));
	this->config.broadcastValueChanged()(Xmms::bind(&XMMS2Interface::configResponse, this));
	this->playback.broadcastVolumeChanged()(Xmms::bind(&XMMS2Interface::volumeResponse, this));
	this->playlist.broadcastChanged()(Xmms::bind(&XMMS2Interface::plistChangeResponse, this));
	this->playlist.broadcastCurrentPos()(Xmms::bind(&XMMS2Interface::curPos, this));
	this->playlist.broadcastLoaded()(Xmms::bind(&XMMS2Interface::getCurrentPlaylist, this));
	this->collection.broadcastCollectionChanged()
								(Xmms::bind(&XMMS2Interface::handleCollChange, this));
	}

XMMS2Interface::~XMMS2Interface()
	{quitting=true;}

Playlist* XMMS2Interface::getMainPlaylist() {
	return mainPlaylist;
}

void XMMS2Interface::setMainPlaylist(Playlist* plist) {
	mainPlaylist = plist;
}

bool XMMS2Interface::scrapResult()
	{return true;}

bool XMMS2Interface::scrapResultI(unsigned int foo)
	{return true;}

/*bool XMMS2Interface::nonFatalError(const std::string& error)
	{std::cout<<"Non Fatal Error: "<<error<<std::endl; return false;}*/

bool XMMS2Interface::newSongResponse(const unsigned int& id)
	{
	if(quitting) return false;
	emit(currentId(id));
	this->medialib.getInfo(id)(Xmms::bind(&XMMS2Interface::getNewSongInfo, this));
	return true;
	}

bool XMMS2Interface::getNewSongInfo(const Xmms::PropDict& info)
	{
	emit(newSong(info));
	return false;
	}

bool XMMS2Interface::getstatus(const Xmms::Playback::Status& status)
	{
	if(quitting) return false;
	emit (changeStatus(status));
	return true;
	}

bool XMMS2Interface::getCurPlaytime(int time)
	{
	if(quitting) return false;
	emit (currentPlaytime(time));
	return true;
	}

bool XMMS2Interface::configResponse(const Xmms::Dict& val)
	{
	if(quitting) return false;
	emit (configValChanged(val));
	return true;
	}

bool XMMS2Interface::volumeResponse(const Xmms::Dict& val)
	{
	if(quitting) return false;
	emit (volumeChanged(val));
	return true;
	}

bool XMMS2Interface::plistChangeResponse(const Xmms::Dict& val)
	{
	if(quitting) return false;
	emit (playlistChanged(val));
	return true;
	}

bool XMMS2Interface::curPos(const unsigned int& val)
	{
	if(quitting) return false;
	emit (currentPos(val));
	return true;
	}

bool XMMS2Interface::getDat(const Xmms::PropDict& i)
	{
	int32_t id = i.get<int32_t> ("id");
	QPair<QString, QString> info;
	try {info.first=QString(i.get<std::string>("artist").c_str());}
	catch(Xmms::no_such_key_error& err )
		{try{
			QString foo=QString(i.get<std::string>("url").c_str());
			if(foo.startsWith("file://"))
				{
				foo.remove(0,foo.lastIndexOf("/")+1);
				info.first=foo;
				}
			if(foo.startsWith("http://")||foo.startsWith("ftp://"))
				info.first=QString("<Internet stream>");
			}
		catch(Xmms::no_such_key_error& err )
			{info.first=QString(" ");}}
	try {info.second=QString(i.get<std::string>("title").c_str());}
	catch(Xmms::no_such_key_error& err )
		{info.second=QString(" ");}
	try{i.get<std::string>("url");}
	catch(Xmms::no_such_key_error& err )
		{std::cout<<"no"<<std::endl;}
	plist.insert (id, info);
	return true;
	}

QString XMMS2Interface::val(QString key, unsigned int id)
	{
	if (!plist.contains (id))
		{
		this->medialib.getInfo(id)(Xmms::bind(&XMMS2Interface::getDat,this));
		plist[id] = QPair<QString, QString>();
		}
	if(key=="artist")
		return plist[id].first;
	else
		return plist[id].second;
	}

bool XMMS2Interface::getCurrentPlaylist(const std::string& name)
	{
	if(quitting) return false;
	emit (playlistNameChanged(name));
	return true;
	}

bool XMMS2Interface::handleCollChange(const Xmms::Dict& change)
	{
	if(quitting) return false;
	emit (collectionsChanged(change));
	return true;
	}
XMMS2Interface::Client::~Client() {}

void XMMS2Interface::emitSongAboutToChange() {
	emit aboutToChangeSong();
}


#endif
