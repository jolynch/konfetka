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
		bool released;
		bool allowUpdates;
		QTimer timer;
		bool songEmitted;
		int MAGFACTOR;
		int GRACE_DISTANCE;

		public:
		SongPositionSlider(DataBackend * c,Qt::Orientation , QWidget * parent = 0);	
		int getDuration();
		int getPositiveTime();
		int getNegativeTime();

		public slots:
		void mousePressEvent ( QMouseEvent *);
		void mouseReleaseEvent ( QMouseEvent *);
		void mouseMoveEvent(QMouseEvent *);
		
		void setTimeFromSlider();
		void setInitTime(int);	
		void setDuration(const Xmms::PropDict&);
		bool handlePlaytimeSignal(uint);
		
		signals:
		void timeChanged(int);
		void aboutToChangeSong();
	};


#endif

