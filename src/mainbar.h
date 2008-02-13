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
#include <QFontMetrics>

class NiceLabel;
class MainBar:public QWidget{	
Q_OBJECT
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
		QToolButton * stopButton;
		QToolButton * forwardButton;
		QToolButton * volumeButton;
		QToolButton * miniButton; //~!
		SongPositionSlider * positionSlider;
		QSlider * volumeSlider;
		QLabel * positionTime;
		QLabel * positionMinusTime;
		QLabel * volume;
		NiceLabel * infoBar;
//		QPushButton * makeSmallButton;
		QPushButton * optionsButton;
		QToolButton * quitButton;
		QTimer * scrollTimer;
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

class NiceLabel:public QLabel{	
Q_OBJECT
	private:
	int x; bool pressed; int pressPos; int id;

	public:
	NiceLabel(QWidget * parent = 0, Qt::WindowFlags f = 0);
	void mousePressEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent * event);
	void mouseMoveEvent(QMouseEvent * event);
	void paintEvent(QPaintEvent *);
	void resizeEvent(QResizeEvent *);
	void timerEvent(QTimerEvent* event);
	void showEvent(QShowEvent * event);
	void hideEvent(QHideEvent * event);
	
	public slots:
	void slotScroll(int amount = -1);
};

#endif
