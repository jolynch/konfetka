#ifndef BASICVIS_CPP
#define BASICVIS_CPP
#include "basicvis.h"


BasicVis::BasicVis(DataBackend * c,int h,int w,QWidget* parent, Qt::WindowFlags f, QColor col):QWidget(parent, f) {
	conn = c;
	height=180;
	width=(int)(w*(2.0/7));
	time = 0;
	color = col;
	
	numBars = 20;
	timerLength = 100;
// 	QSettings s;
// 		if(s.contains("visNumBars") && s.value("visNumBars").toInt()==0)
// 		numBars=0;
// 		else if(s.contains("visNumBars"))
// 		numBars = s.value("visNumBars").toInt();
// 		if(s.contains("visFps") && s.value("visFps").toInt()==0)
// 		timerLength = 0;
// 		else if(s.contains("visFps"))
// 		timerLength = 1000/ s.value("visFps").toInt();
	
	layout = new QGridLayout();
	
	scene = new MyScene(0,0,width,height,this);
	linearGrad = new QLinearGradient(QPointF(0,0), QPointF(10,180));
	linearGrad->setColorAt(0, QColor( 187,213,225,255 ));
	linearGrad->setColorAt(1, QColor( 239,239,239,255 ));
	
	//scene->addText("Visualizer will go here");
	waitTimer = new QTimer(this);
	waitTimer->start(30000);
	QObject::connect(waitTimer, SIGNAL(timeout()), this, SLOT(toggleAnimation()));
	
	
	view = new QGraphicsView(scene);
	view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	
	if(col != Qt::black) {
	QPalette pal = view->palette();
	pal.setBrush(QPalette::Window,QBrush(col));
	view->setPalette(pal);
	linearGrad->setColorAt(0, QColor( 130,170,188,255 ));
	linearGrad->setColorAt(1, QColor( 157,160,162,255 ));
	}
	view->setBackgroundRole(QPalette::Window);
	
	timer = new QTimer(this);
	
	layout->addWidget(view);
	layout->setMargin(0);
	this->setLayout(layout);
	connect(timer, SIGNAL(timeout()), this, SLOT(doAnimation()));
	connect(conn,SIGNAL(qsettingsValueChanged(QString, QVariant)),this,SLOT(processSettingsUpdate(QString,QVariant)));
	setNumAndLen(numBars,timerLength);
}

BasicVis::~BasicVis() {
	delete layout;
	delete scene;
	delete view;
	delete timer;
	delete waitTimer;
	delete linearGrad;
}

void BasicVis::processSettingsUpdate(QString name,QVariant value) {
	if(name == "konfetka/visFps") {
		if(value.toInt()!=0)
		timerLength = 1000/value.toInt();
		else
		timerLength = 0;
	timer->stop();
	timer->start(timerLength);
	}
	else if(name == "konfetka/visNumBars") {
		numBars = value.toInt();
		setNumAndLen(numBars,timerLength);
	}
	
	if (timerLength == 0 || numBars == 0)
	timer->stop();
}

void BasicVis::paintEvent(QPaintEvent * event) {
QPainter painter(this);

	QStyleOption opt;
	opt.init(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

void BasicVis::updateItem(int) {
}

void BasicVis::doAnimation() {
if(numBars == 0 || timerLength == 0) return;
time=(time+1)%180; 
QList<QGraphicsItem *> list =scene->items();
int step = 180 / numBars;
	for(int i=0;i<list.size();i++) {
// 	//std::cout<<(time+(180-(i*step))) % 180<<std::endl;
	list.value(i)->setPos(QPointF(list.value(i)->x(),(time+(180-(i*step))) % 180));
	}
}

void BasicVis::setNumAndLen(int num,int len) {
if(timer->isActive())
timer->stop();
numBars = num;
timerLength = len;
// //std::cout<<num<<" "<<timerLength<<std::endl;
int q = 0;
time = 0;

qDeleteAll( scene->items() );

if(num == 0 || len == 0) return;
QGraphicsRectItem * it;
	for(int i=0;i<num;i++,q+=(width/num+1)) {
	it = scene->addRect(QRectF(q,0,width/num,180) );
	it->setBrush(QBrush(*linearGrad));
	
	if(color!=Qt::black) {
	it->setPen(QPen(Qt::transparent));
	}
	else
	it->setPen(QPen(QColor(255,255,255,255)));
	}
timer->start(len);
}

void BasicVis::toggleAnimation() {
qDeleteAll( scene->items() );

if(timer->isActive())
timer->stop();
else
setNumAndLen(numBars,timerLength);

}


MyScene::MyScene(qreal x, qreal y, qreal width, qreal height,QObject * parent):QGraphicsScene(x,y,width,height,parent) {
parentVis=(BasicVis*)parent;
}

void MyScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent) {
parentVis->toggleAnimation();
}

void MyScene::setVis(BasicVis* vis) {
parentVis = vis;
}

#endif

