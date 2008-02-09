#ifndef XMMS2Interface_CPP
#define XMMS2Interface_CPP
#include "xmms2interface.h"

XMMS2Interface::XMMS2Interface(QObject * parent, const std::string &name):QObject(parent), Xmms::Client (name) {
	quitting=false;
	char * path =  getenv("XMMS_PATH");
	try {
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
				exit(EXIT_FAILURE);
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
			else {
			std::cout<<"Autostarting the server failed, either this is your configuration, or an unkown error was recieved."<<std::endl;
			exit(EXIT_FAILURE);
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
	this->collection.broadcastCollectionChanged()(Xmms::bind(&XMMS2Interface::handleCollChange, this));
	this->playback.signalPlaytime()(Xmms::bind (&XMMS2Interface::handlePlaytimeSignal, this));
}

void XMMS2Interface::emitInitialXmms2Settings()
	{
	this->playlist.currentActive() (Xmms::bind(&XMMS2Interface::getCurrentPlaylist,this));
	this->playback.currentID()(Xmms::bind(&XMMS2Interface::newSongResponse, this));
	this->playback.getStatus()(Xmms::bind(&XMMS2Interface::getstatus, this));
	this->playback.getPlaytime()(Xmms::bind(&XMMS2Interface::getCurPlaytime, this));
	this->playback.volumeGet()(Xmms::bind(&XMMS2Interface::volumeResponse, this));
	this->playlist.currentPos()(Xmms::bind(&XMMS2Interface::curPos,this));
	this->config.valueList()(Xmms::bind(&XMMS2Interface::configResponse, this));
	}

void XMMS2Interface::changeAndSaveXMMS2Settings(const std::string &key, const std::string &val) {
	this->config.valueSet(key, val);
}

XMMS2Interface::~XMMS2Interface()
	{quitting=true;}


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
	return true;
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

bool XMMS2Interface::handlePlaytimeSignal(uint newTime) {
	emit songPositionChanged(newTime);
	return true;
}

QString XMMS2Interface::encodeUrl(QString val,int forWhat) {

        QByteArray exclude = "ABCDEFGHIJKLMNOPQRSTUVWXWZabcdefghijklmnopqrstuvwxyz0123456789:/-._";
        QByteArray ray = QUrl::toPercentEncoding(val,exclude);
        QString final(ray);
	switch(forWhat) {
		case XMMS2:
	        final.replace("%20","+");
		break;
		case WIKI:	
		final.replace("%20","_");
		break;
		default:
		break;
	}

        return final;
}


#endif
