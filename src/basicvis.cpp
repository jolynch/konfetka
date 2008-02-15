#ifndef BASICVIS_CPP
#define BASICVIS_CPP
#include "basicvis.h"

#include <bitset>
#include <limits>
BasicVis::BasicVis(DataBackend * c,QWidget* parent,Qt::WindowFlags f):QWidget(parent, f) {
	conn = c;
	numBars = 20;
	fps = 30; lastChunk = 0;
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
	srand(time(NULL));
	dummyGetChunk(&lastChunk,25,40);
	
	for(int i=0;i<16;i++) {
	
	}
	
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

int BasicVis::dummyGetChunk(short *buffer, int drawtime,unsigned int blocking) {
	short r = (short)rand();
	if(r%2==0)
	*buffer = lastChunk + ((short)(rand()))%5;
	else
	*buffer = lastChunk - ((short)(rand()))%5;
// 	std::cout<<*buffer<<" ";
// 	std::cout<<"Value: "<<std::bitset<std::numeric_limits<short>::digits>(*buffer)<<std::endl;
	unsigned short mask = 0x8000;
	for(short i=0;i<15;i++) {
// 		std::cout<<"MASK: "<< std::bitset<std::numeric_limits<unsigned short>::digits>(mask)<<std::endl;
// 		std::cout<<"Shift: "<< std::bitset<std::numeric_limits<short>::digits>(*buffer & mask)<<std::endl;
		mask = mask>>1;
	}
	return 8;
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
			//Sometimes the sub gets "squeezed out"
			if(subX > 5) {
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
		//draw onto a pixmap 
		QPixmap p(w,height());
		QPainter pixmapPainter(&p);
		pixmapPainter.setBrush(*linearGrad);
		pixmapPainter.setPen(QPen(Qt::white));
		pixmapPainter.drawRect(0,0,w,height());
		//draw the pixmap numBars times instead of drawing numBars rectangles (cpu intensive)
		for(int i = 0; i < numBars;i++) {
			tmp = tmp + step; 
			if(tmp> height()) tmp = (tmp-height());
			painter.drawPixmap(i*w,tmp,p);
		}
		tmp = initY;
		painter.setBrush(QBrush(Qt::gray));
		painter.setPen(QPen(Qt::transparent));
		//The peaks have to be drawn seperately
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
			painter.setPen(QPen(QBrush(QColor("#696969")),.5));
			painter.drawPath(path);
			painter.setPen(Qt::blue);
			painter.setFont(QFont("courier",25, QFont::Bold ));
			painter.drawText(rect(), Qt::AlignCenter,"DEMO ONLY");
			return;
		}
		
		painter.setRenderHint(QPainter::Antialiasing);
		painter.setPen(QPen(QBrush(QColor("#696969")),.5));
		qreal mid = height()/2.0;
		lastChunk = lastChunk%((int)mid/2);
// 		std::cout<<"SCOPE: "<<lastChunk<<std::endl;
		path = QPainterPath();
		srand(time(NULL));
		double delta = 0.0; int dis = 6; int step = 1;
		for(int j = 0;j<3;j++) {
		path.moveTo(0.0,mid-20 + j*(20));
		QPointF curPos = path.currentPosition();
		for(int i =0;i<width()-dis*5;i+=dis) {
			curPos.setX(i);
			path.lineTo(curPos);
			delta = rand() % 5;
			if(rand()%2 == 0) delta *= -1;
			delta = 5.0 * cos(sqrt(pow(sin((i+1)*lastChunk)+ 1 + timeline.currentFrame(),3)) * delta * (360 / (int)(mid/2)));
			curPos.setY(curPos.y()+delta - .5);
			path.lineTo(curPos);
		}
		step = (int)((mid-20 + j*(20)) - curPos.y()) / 5;
			for(int u=width()-dis*5;u<width();u+=dis) {
				curPos.setX(u);
				path.lineTo(curPos);
				curPos.setY(curPos.y()+step);
				path.lineTo(curPos);
			}
		}
		painter.drawPath(path);
	}
	painter.setPen(Qt::blue);
	painter.setFont(QFont("courier",25, QFont::Bold ));
	painter.drawText(rect(), Qt::AlignCenter,"DEMO ONLY");
	painter.setFont(QFont("courier",7));
	painter.drawText(rect(), Qt::AlignBottom | Qt::AlignHCenter,"OHAI XMMS2! CAN WE HAS VISUALISATION NOW PLZ? KTHXBAI!");
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
	painter.eraseRect(0,0,width(),height());
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

