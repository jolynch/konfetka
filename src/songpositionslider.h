#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#include <xmmsclient/xmmsclient.h>
#endif
#ifndef SONGPOSITIONSLIDER_H
#define SONGPOSITIONSLIDER_H
#include "databackend.h"
#include <QTimer>
#include <QSlider>
#include <QDebug>
#include <QMouseEvent>
#include <QStyle>
class DataBackend;
class SongPositionSlider:public QSlider 
	{	Q_OBJECT
		private:
		DataBackend * conn;
		int duration,time;
		bool hasTime;
		Xmms::Playback::Status status;
		QTimer timer;
		bool songEmitted;
	
		public:
		SongPositionSlider(DataBackend * c,Qt::Orientation , QWidget * parent = 0);	
		int getDuration();
		int getPositiveTime();
		int getNegativeTime();
		void setDuration(int);

		public slots:
		void mousePressEvent ( QMouseEvent *);
		void mouseReleaseEvent ( QMouseEvent *);
		void leaveEvent (QEvent *);
		void setTimeFromSlider();
		void setInitTime(int);	
		void setNewStatus(Xmms::Playback::Status);
		void timerEvent();
		void syncTimeFromOtherSliders(int,SongPositionSlider*);
		
		signals:
		void timeChanged(int);
		void syncTime(int,SongPositionSlider*);
		void aboutToChangeSong();
	};


#endif

