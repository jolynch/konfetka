#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#include <xmmsclient/xmmsclient.h>
#endif
#ifndef XMMS2Interface_H
#define XMMS2Interface_H
#include "xmmsqt4.h"
#include <QString>
#include <QEventLoop>
#include <QHash>
#include <QPair>
#include <QSettings>
#include <QMessageBox>
#include <QProcess>
#include <qobject.h>
#include <qtimer.h>
#include <string>
class SongPositionSlider;
class Playlist;

class XMMS2Interface:public QObject, public Xmms::Client
	{	Q_OBJECT
	private:
		bool quitting;
//What the hell is this stuff? ~A.F       <- LESSON #1 of programming, if you don't know what something is, DONT COMMENT IT OUT (hint
//search for plist in the .cpp file, it may be uneccesary but clean the cpp before you clean the h
		QHash < uint32_t, QPair <QString, QString> > plist;
		QEventLoop * loop;
		Playlist * mainPlaylist;
	public:
		XMMS2Interface(QObject * parent, const std::string &name);
		~XMMS2Interface();
		bool scrapResult(); //function for async returns with unimportant results.
		bool scrapResultI(unsigned int foo); //@overload for scrapping 1 int returns.
//		bool nonFatalError(const std::string& error); - Don't use this anymore!
		bool newSongResponse(const unsigned int& id);
		bool getNewSongInfo(const Xmms::PropDict& info);
		bool getstatus(const Xmms::Playback::Status& status);
		bool getCurPlaytime(int);
		bool configResponse(const Xmms::Dict& val);
		bool volumeResponse(const Xmms::Dict& val);
		bool plistChangeResponse(const Xmms::Dict& val);
		QString val(QString key, unsigned int id);
		bool getDat(const Xmms::PropDict& i);
		bool curPos(const unsigned int& val);
		bool getCurrentPlaylist(const std::string& name);
		bool handleCollChange(const Xmms::Dict& change);
		void emitSongAboutToChange();
		//there can only be one master playlist taking movement directions.
		Playlist* getMainPlaylist();
		void setMainPlaylist(Playlist*);
		void next();
		void previous();
// 	public slots:
// 		void changeSongInPlaylist(int);
	signals:
		void newSong(Xmms::PropDict);
		void changeStatus(Xmms::Playback::Status);
		void currentId(int);
		void currentPlaytime(int);
		void configValChanged(const Xmms::Dict& val);
		void volumeChanged(const Xmms::Dict& val);
		void playlistChanged(const Xmms::Dict& val);
		void currentPos(const unsigned int& pos);
		void playlistNameChanged(const std::string& name);
		void collectionsChanged(const Xmms::Dict& change);
		void aboutToChangeSong();
		void changeSong(int);
		void syncTime(int,SongPositionSlider*);
	};
#endif
