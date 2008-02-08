#ifndef BASICVIS_CPP
#define BASICVIS_CPP
#include "basicvis.h"


BasicVis::BasicVis(DataBackend * c,QWidget* parent,Qt::WindowFlags f):QWidget(parent, f) {
	conn = c;
	numBars = 20;
	fps = 30;
	secondsRunning = 1; initY = 0;
	linearGrad = new QLinearGradient(QPointF(0,0), QPointF(10,180));
	linearGrad->setColorAt(0, QColor( 187,213,225,255 ));
	linearGrad->setColorAt(1, QColor( 239,239,239,255 ));
	
	timeline.setFrameRange(0,fps);
	timeline.setCurveShape(QTimeLine::LinearCurve);
	connect(&timeline,SIGNAL(frameChanged(int)),this,SLOT(paintNext(int)));
// 	timeline.start();
	connect(&timeline,SIGNAL(finished()),this,SLOT(timelineDone()));
	connect(conn,SIGNAL(qsettingsValueChanged(QString,QVariant)),this,SLOT(respondToConfigChange(QString,QVariant)));
	
}

void BasicVis::paintNext(int val) {
	update();
}

void BasicVis::timelineDone() {
	secondsRunning += 1;
	if(secondsRunning <= 10){
		timeline.start();
	} else { secondsRunning = 0; update();}
}

void BasicVis::doAnimation() {
	timeline.start();
}

void BasicVis::respondToConfigChange(QString name,QVariant value) {
	if(name == "konfetka/visFps") {
	fps = value.toInt();
	}
	else if(name == "konfetka/visNumBars") {
	numBars = value.toInt();
	}
}

void BasicVis::paintEvent(QPaintEvent * event) {
	QPainter painter(this);
	
	QImage im(":images/speaker.png");
	im = im.scaled(100,100, Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
	QImage c;
	int x,y;
	x = 20; y = 20;
	painter.eraseRect(geometry());
		painter.drawImage(x,y,im);
		if(timeline.currentFrame()%2==0) {
			painter.eraseRect(geometry());
			c = im.scaled(95,95, Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
			painter.drawImage(x+2,y+2,c);
		}
	
	return;

	if(secondsRunning == 0 || numBars == 0 || fps == 0) {
		if(numBars == 0 || fps == 0)
			timeline.stop();
		painter.eraseRect(geometry());
		return;
	}
	
	painter.setBrush(*linearGrad);
	painter.setPen(QPen(Qt::white));
	int w = width() / numBars;
	int step = height() / numBars;
	int tmp = initY;
	initY = initY + 1; 
	if(initY> height()) initY = (initY-height());
		
	for(int i = 0; i < numBars;i++) {
		tmp = tmp + step; 
		if(tmp> height()) tmp = (tmp-height());
		painter.drawRect(i*w,tmp,w,height());
	}
}

void BasicVis::setNumAndLen(int num,int len) {fps = len; numBars = num;}

void BasicVis::mousePressEvent(QMouseEvent * event) {
	if(event->button() == Qt::LeftButton) {
		if((timeline.state()==QTimeLine::NotRunning)) {
		secondsRunning = 1;
		timeline.start();
		} else {timeline.stop();}
	}
	
}
#endif

