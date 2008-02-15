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
#include "nicelabel.h"
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
#include <QFontMetrics>

class NiceLabel;
class MainBar:public QWidget{	
Q_OBJECT
	private:
		QString scrollInfo;
		int volVal;bool muted;
		int channels;
		int screenW,screenH;
		bool mini;
		QWidget * p;
		DataBackend * conn;
		QGridLayout * layout;
		QToolButton * backButton;
		QToolButton * playButton;
		QToolButton * stopButton;
		QToolButton * forwardButton;
		QToolButton * volumeButton;
		QToolButton * miniButton; 
		SongPositionSlider * positionSlider;
		QSlider * volumeSlider;
		QLabel * positionTime;
		QLabel * positionMinusTime;
		QLabel * volume;
		NiceLabel * infoBar;
		QPushButton * optionsButton;
		QToolButton * quitButton;
		QTimer * scrollTimer;
		QIcon * miniIcon; 
		Xmms::Playback::Status stat;

	private slots:
		void slotPlay();
		void slotPause();
		void slotStop();
		void slotUpdateInfo(int id);
		void slotSetVolume(int);
		void slotGetVolume(const Xmms::Dict&);
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
};

#endif
