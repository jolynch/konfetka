#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#include <xmmsclient/xmmsclient.h>
#endif
#ifndef BASICVIS_H
#define BASICVIS_H
#include "databackend.h"
#include <math.h>
#include <QWidget>
#include <QPixmap>
#include <QRectF>
#include <QTimer>
#include <QTimeLine>
#include <QPainter>
#include <QLinearGradient>
#include <QMouseEvent>
#include <QPainterPath>
#include <QtDebug>

enum VisType { SPEAKER, SPECTRUM, SCOPE, OZONE};
class Speaker;

class BasicVis:public QWidget {
Q_OBJECT
	private:
	VisType type;
	DataBackend * conn;
	int numBars; int fps; int secondsRunning; int initY;
	QTimer timer;
	QTimeLine timeline;
	QLinearGradient * linearGrad;
	QImage fullSpeaker;
	QImage smallSpeaker;
	short lastChunk;

	//For Speaker vis
	Speaker * t1;
	Speaker * t2;
	Speaker * t3;
	Speaker * t4;
	Speaker * sub;
	
	//For Scope vis
	QPainterPath path;

	private slots:
	void paintNext(int);
	void timelineDone();
	
	public:
	BasicVis(DataBackend * c,QWidget * parent = 0, Qt::WindowFlags f = 0);
	void mousePressEvent(QMouseEvent * event);
	void paintEvent(QPaintEvent *);
	void resizeEvent(QResizeEvent *);
	void wheelEvent(QWheelEvent * event);
	int dummyGetChunk(short *buffer, int drawtime,unsigned int blocking);

	public slots:
	void setNumAndLen(int,int);
	void respondToConfigChange(QString,QVariant);
	void doAnimation();
};

class Speaker:public QWidget {
Q_OBJECT
	private:
	DataBackend * conn;
	QImage fullSpeaker;
	QImage smallSpeaker;
	int frequency;
	int value; int delta;
	bool draw;

	public: 
	Speaker(DataBackend * c,int w, int h,int freq,int delta,QWidget * parent = 0, Qt::WindowFlags f = 0);
	void paintEvent(QPaintEvent *);
	void updateSpeaker();
};



#endif
