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
#include <QObject>
#include <QTimer>
#include <QUrl>
#include <string>
class SongPositionSlider;

typedef int UrlType;
class XMMS2Interface:public QObject, public Xmms::Client
	{	Q_OBJECT
	private:
		bool quitting;
		QEventLoop * loop;
	public:
		XMMS2Interface(QObject * parent, const std::string &name);
		~XMMS2Interface();
		bool scrapResult(); //function for async returns with unimportant results.
		bool scrapResultI(  int foo); //@overload for scrapping 1 int returns.
		bool newSongResponse(const   int& id);
		bool getstatus(const Xmms::Playback::Status& status);
		bool getCurPlaytime(int);
		bool configResponse(const Xmms::Dict& val);
		bool configValueList(const Xmms::Dict& val);
		bool volumeResponse(const Xmms::Dict& val);
		bool plistChangeResponse(const Xmms::Dict& val);
		bool curPos(const   int& val);
		bool curPosD(const Xmms::Dict& val);
		bool getCurrentPlaylist(const std::string& name);
		bool handleCollChange(const Xmms::Dict& change);
		bool handlePlaytimeSignal(int);
		void next();
		void previous();
		QString encodeUrl(QString,UrlType toXmms2 = XMMS2);
		QString convertBoostVariantToQString(boost::variant<int32_t, uint32_t, std::string>);
		
		void emitInitialXmms2Settings();
		void changeAndSaveXMMS2Settings(const std::string &key, const std::string &val);
		static const UrlType WIKI = 0x001;
		static const UrlType XMMS2 = 0x010;
// 	public slots:
// 		void changeSongInPlaylist(int);
	signals:
		void changeStatus(Xmms::Playback::Status);
		void currentId(int);
		void currentPlaytime(int);
		void configValChanged(const Xmms::Dict& val);
		void configValList(const Xmms::Dict& val);
		void volumeChanged(const Xmms::Dict& val);
		void playlistChanged(const Xmms::Dict& val);
		void currentPos(const   int& pos);
		void playlistNameChanged(const std::string& name);
		void collectionsChanged(const Xmms::Dict& change);
		void aboutToChangeSong();
//		void changeSong(int);
		void syncTime(int,SongPositionSlider*);
		void songPositionChanged(int);
	};
#endif
