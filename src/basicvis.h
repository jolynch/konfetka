#ifndef XMMSCLIENTPP
#define XMMSCLIENTPP
#include <xmmsclient/xmmsclient++.h>
#include <xmmsclient/xmmsclient.h>
#endif
#ifndef BASICVIS_H
#define BASICVIS_H
#include "databackend.h"
#include <QWidget>
#include <QGridLayout>
#include <QRectF>
#include <QTimer>
#include <QTimeLine>
#include <QPainter>
#include <QLinearGradient>
#include <QMouseEvent>

class BasicVis:public QWidget {
Q_OBJECT
	private:
	DataBackend * conn;
	int numBars; int fps; int secondsRunning; int initY;
	QTimer timer;
	QTimeLine timeline;
	QLinearGradient * linearGrad;
	
	private slots:
	void paintNext(int);
	void timelineDone();
	
	public:
	BasicVis(DataBackend * c,QWidget * parent = 0, Qt::WindowFlags f = 0);
	void mousePressEvent(QMouseEvent * event);
	
	public slots:
	void setNumAndLen(int,int);
	void paintEvent(QPaintEvent *);
	void respondToConfigChange(QString,QVariant);
	void doAnimation();
};


#endif
