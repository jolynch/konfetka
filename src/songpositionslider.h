#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#include <xmmsclient/xmmsclient.h>
#endif
#ifndef BOOST_BIND
#define BOOST_BIND
#include <boost/bind.hpp>
#endif
#ifndef SONGPOSITIONSLIDER_H
#define SONGPOSITIONSLIDER_H
#include "databackend.h"
#include "databackend/mlibdata.h"
#include <QTimer>
#include <QSlider>
#include <QDebug>
#include <QMouseEvent>
#include <QStyle>
#include <QPainter>
#include <QLinearGradient>
#include <QSet>
#include <QMenu>
#include <QTime>

class DataBackend;

class SongPositionSlider:public QSlider 
	{	Q_OBJECT
		private:
		DataBackend * conn;
		int duration,time,currentID,barheight;
		bool released;
		bool allowUpdates;
		QTimer timer;
		bool songEmitted;
		int MAGFACTOR;
		int GRACE_DISTANCE;
		int CUEGRACE_DISTANCE;
		QSet<int> markers;
		
		public:
		SongPositionSlider(DataBackend * c,Qt::Orientation o, QWidget * parent = 0,int h = 20);	
		int getDuration();
		int getPositiveTime();
		int getNegativeTime();
		enum SongType {FILE,STREAM,UNKNOWN};
		SongType curType;
		void sendToServer();

		public slots:
		void mousePressEvent ( QMouseEvent *);
		void mouseReleaseEvent ( QMouseEvent *);
		void mouseMoveEvent(QMouseEvent *);
		void paintEvent(QPaintEvent * event);
		
		void setTimeFromSlider();
		void setInitTime(int);	
		void setDuration(int id);
		bool handlePlaytimeSignal(int);
		void setMarker(); //sets a marker at wherever the time variable is
		void delMarker(); //same, but removes one
		
		signals:
		void timeChanged(int);
		void aboutToChangeSong();
	};


#endif

