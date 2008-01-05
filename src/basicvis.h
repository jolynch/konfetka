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
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QRectF>
#include <QTimer>
#include <QList>
#include <QSettings>
#include <QtAlgorithms>
#include <QStyleOption>

class MyScene;
class BasicVis:public QWidget {
	Q_OBJECT
	private:
	DataBackend * conn;
	QGridLayout * layout;
	MyScene * scene;
	QGraphicsView * view;
	//QGraphicsItem * item;
	QGraphicsRectItem * thing;
	int height;
	int width;
	int numBars;
	int timerLength;
	int time;
	QTimer * timer;
	QTimer * waitTimer;
	QLinearGradient * linearGrad;
	QColor color;
	//QList<QGraphicsRectItem *> bars;

	public:
	BasicVis(DataBackend * c,int h,int w,QWidget * parent = 0, Qt::WindowFlags f = 0, QColor col = Qt::black);
	~BasicVis();

	public slots:
	void updateItem(int);
	void setNumAndLen(int,int);
	void doAnimation();
	void toggleAnimation();
	void paintEvent(QPaintEvent *);
	void processSettingsUpdate(QString,QVariant);
	};
class MyScene:public QGraphicsScene
	{	Q_OBJECT
	private:
	BasicVis * parentVis;
	public:
	void setVis(BasicVis*);
	MyScene( qreal x, qreal y, qreal width, qreal height, QObject * parent = 0 );
	void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);
	};

#endif
