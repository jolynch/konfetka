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
	linearGrad->setColorAt(1, QColor("lightgray"));
	
	fullSpeaker = QImage(":images/speaker.png").scaled(150,150, Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
	smallSpeaker = fullSpeaker.scaled(145,145, Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
	
	type = SPEAKER;

	t1 = 0;
	t2 = 0;
	t3 = 0;
	t4 = 0;
	sub = 0;
	
	timeline.setFrameRange(0,fps);
	timeline.setCurveShape(QTimeLine::LinearCurve);
	connect(&timeline,SIGNAL(frameChanged(int)),this,SLOT(paintNext(int)));
// 	timeline.start();
	connect(&timeline,SIGNAL(finished()),this,SLOT(timelineDone()));
	connect(conn,SIGNAL(qsettingsValueChanged(QString,QVariant)),this,SLOT(respondToConfigChange(QString,QVariant)));
	
}

void BasicVis::paintNext(int val) {
	if(type == SCOPE && val%5==0) {
	update();
	return;
	}
	update();
	if(t1 && t2 && t3 && t4 && sub) {
		srand(time(NULL));
		if(rand()%2);
		t1->updateSpeaker();
		if(rand()%2);
		t2->updateSpeaker();
		if(rand()%2);
		t3->updateSpeaker();
		if(rand()%2);
		t4->updateSpeaker();
		sub->updateSpeaker();
	}
}

void BasicVis::resizeEvent(QResizeEvent * event) {
	std::cout<<height()<<" "<<width()<<std::endl;
	if(!t1 && !t2 && !t3 && !t4 && !sub) {
	int subPos =  qMin(height(),width()-height());
	t1 = new Speaker(conn,height()/2,height()/2,11000,2);
	t2 = new Speaker(conn,height()/2,height()/2,11000,2);
	t3 = new Speaker(conn,height()/2,height()/2,11000,2);
	t4 = new Speaker(conn,height()/2,height()/2,11000,2);
		if(subPos <= 0) subPos = 1; 
		sub = new Speaker(conn,subPos,subPos,11000,5);
	}
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
	timeline.setFrameRange(0,fps);
	}
	else if(name == "konfetka/visNumBars") {
	numBars = value.toInt();
	}
}

void BasicVis::paintEvent(QPaintEvent * event) {
	QPainter painter(this);
// 	std::cout<<width()<<" "<<height()<<std::endl;
	if(type == SPEAKER) {
		if(t1 && t2 && t3 && t4 && sub) {
		int subPos =  qMin(height(),width()-height());
		int subX = qMax(height()/2,height()/2+(width()-height()-subPos)/2);
// 		painter.eraseRect(0,0,width(),height());
		painter.drawPixmap(0,0,QPixmap::grabWidget(t1));
		painter.drawPixmap(width()-height()/2,0,QPixmap::grabWidget(t2));
		painter.drawPixmap(0,height()/2,QPixmap::grabWidget(t3));
		painter.drawPixmap(width()-height()/2,height()/2,QPixmap::grabWidget(t4));
			if(subX > 0) {
			painter.drawPixmap(subX,0,QPixmap::grabWidget(sub));
			}
		}
	}
	else if(type == SPECTRUM) {
		if(secondsRunning == 0 || numBars == 0 || fps == 0) {
			if(numBars == 0 || fps == 0)
				timeline.stop();
			painter.eraseRect(geometry());
		return;
		}
		
		painter.setBrush(*linearGrad);
		painter.setPen(QPen(Qt::white));
		int w =qRound((qreal)width() / (qreal)numBars);
		int step = height() / numBars;
		int tmp = initY;
		
		srand(time(NULL));
		for(int i = 0; i < numBars;i++) {
			tmp = tmp + step; 
			if(tmp> height()) tmp = (tmp-height());
			painter.drawRect(i*w,tmp,w,height());
		}
		tmp = initY;
		painter.setBrush(QBrush(Qt::gray));
		painter.setPen(QPen(Qt::transparent));
		for(int i = 0; i < numBars;i++) {
			tmp = tmp + step; 
			if(tmp> height()) tmp = (tmp-height());
			painter.drawRect(i*w+1,tmp-2,w-1,2);
		}
		initY = initY + 1; 
		if(initY> height()) initY = (initY-height());
	}
	else if(type == SCOPE) {
		if(timeline.state() != QTimeLine::Running) {
			painter.setRenderHint(QPainter::Antialiasing);
			painter.setPen(QPen(QBrush(QColor("#696969")),3.5));
			painter.drawPath(path);
			return;
		}
		painter.setRenderHint(QPainter::Antialiasing);
		painter.setPen(QPen(QBrush(QColor("#696969")),3.5));
		qreal mid = height()/2.0;
		path = QPainterPath();
		path.moveTo(0.0,mid);
		int delta = 0;
		srand(time(NULL));
		for(int i =0;i<width();i+=15) {
			delta =(int)(1+sqrt(rand()))*(int)(log((rand()+1)*(i+1)*(timeline.currentFrame()+1)));
			if(timeline.currentFrame()%2 == 0) delta = delta*-1;
			path.quadTo(i-7.5,mid,(qreal)i,mid-((mid/1.5)*(cos(delta))));
		}
		painter.drawPath(path);
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
	update();
}

void BasicVis::wheelEvent(QWheelEvent * event){
	int t;
	if(event->delta()>0) t = type + 1;
	if(event->delta()<0) t = type - 1;
		if(t>2 || t ==0) {
		type = SPEAKER;
		}
		else if(t==1){
		type = SPECTRUM;
		}
		else
		type = SCOPE;
}

/***********************************- Speaker- *****************************************/ 
Speaker::Speaker(DataBackend * c,int w, int h,int freq,int delta,QWidget * parent, Qt::WindowFlags f ):QWidget(parent, f){
	conn = c;
	frequency = freq;
	value = 0;
	fullSpeaker = QImage(":images/speaker.png").scaled(w,h, Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
	smallSpeaker = fullSpeaker.scaled(w-delta,h-delta, Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
	resize(w,h);
	this->delta = delta;
	draw = true;
}

void Speaker::paintEvent(QPaintEvent * ev) {
	QPainter painter(this);
	painter.drawImage(0,0,fullSpeaker);
	if(draw) {
		if(value%2==0) {
			painter.drawImage(delta/2,delta/2,smallSpeaker);
		}
	draw = false;
	}
}

void Speaker::updateSpeaker() {
	value = (value + 1) % 100;
	draw = true;
	update();
}

/***************************************************************************************/

#endif

