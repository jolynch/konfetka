#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#endif
#ifndef MAINBAR_H
#define MAINBAR_H
#include <iostream>
#include "databackend.h"
#include "databackend/mlibdata.h"
#include "songpositionslider.h"
#include <QWidget>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QHBoxLayout>
#include <QTime>
#include <QIcon>
#include <QToolButton>
#include <QPainter>
#include <QDesktopWidget>
#include <QShortcut>
#include <iostream>
#include <string>

class MainBar:public QWidget
	{	Q_OBJECT
	private:
		std::string scrollInfo;
		int volVal;bool muted;
		int channels;
		int screenW,screenH;
		bool mini;
		QWidget * p;
		DataBackend * conn;
		QGridLayout * layout;
		QToolButton * backButton;
		QToolButton * playButton;
		QToolButton * pauseButton;
		QToolButton * stopButton;
		QToolButton * forwardButton;
		QToolButton * volumeButton;
		QToolButton * miniButton; //~!
		SongPositionSlider * positionSlider;
		QSlider * volumeSlider;
		QLabel * positionTime;
		QLabel * positionMinusTime;
		QLabel * volume;
		QLabel * TitleBar;
//		QPushButton * makeSmallButton;
		QPushButton * optionsButton;
		QToolButton * quitButton;
		QTimer * scrollTimer;
		QIcon * playIcon;
		QIcon * pauseIcon;
		QIcon * stopIcon;
		QIcon * backIcon;
		QIcon * nextIcon;
		QIcon * quitIcon;
		QIcon * volumeIcon;
		QIcon * muteIcon;
		QIcon * miniIcon; 
		Xmms::Playback::Status stat;
		//~!
//		QShortcut * shortcut;

	private slots:
//		void slotBack();
		void slotPlay();
		void slotPause();
		void slotStop();
//		void slotForward();
		void slotUpdateInfo(int id);
		void slotScroll();
		void slotSetVolume(int);
		void slotGetVolume(const Xmms::Dict&);
//		void slotHide();
		void slotMini();
		void slotMute();
		void newStatus(Xmms::Playback::Status);
		void updateTime();
	public:
		MainBar(DataBackend * c, QWidget* papa,
				QWidget * parent = 0, Qt::WindowFlags f = 0);
		~MainBar();
		QString curInfo();
	signals:
		void infoChanged();
		void volumeChanged(int);
//		void changeSongRel(int);
	};

#endif
