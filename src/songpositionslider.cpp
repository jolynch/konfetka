#ifndef SONGPOSITIONSLIDER_CPP
#define SONGPOSITIONSLIDER_CPP
#include "songpositionslider.h"

//How sensitive do we want the slider to be.
//#define MAGFACTOR 1000

/******** SongPositionSlider - Allows one to create a functioning seekBar by just constructing the object *****/
SongPositionSlider::SongPositionSlider(DataBackend * c,Qt::Orientation o, QWidget* p):QSlider(o,p) {
	conn = c;
	duration = 0;
	time = -1;
	released = false;
	songEmitted = false;
	allowUpdates =  true;
	MAGFACTOR = 1000;
	GRACE_DISTANCE = 10;
	CUEGRACE_DISTANCE = 2;
	connect(conn,SIGNAL(currentPlaytime(int)),this,SLOT(setInitTime(int)));
	connect(conn,SIGNAL(currentId(int)),this,SLOT(setDuration(int)));
	connect(this,SIGNAL(valueChanged(int)),this,SIGNAL(timeChanged(int)));
	connect(this,SIGNAL(valueChanged(int)),this,SLOT(update()));
	connect(conn,SIGNAL(songPositionChanged(uint)), this, SLOT(handlePlaytimeSignal(uint)));
}

bool SongPositionSlider::handlePlaytimeSignal(uint newTime) {
	if(isSliderDown() && !released || duration==0 || curType != FILE) return true;
	newTime = newTime/MAGFACTOR;
	setValue(newTime);
	
	if(getNegativeTime()-2<0 && !songEmitted) {
	//conn->emitSongAboutToChange();
	songEmitted = true;
	}	

	return true;
}

//Returns the duration of the current song
int SongPositionSlider::getDuration() {
	return duration;
}

//Returns the amount of time that has passed 
int SongPositionSlider::getPositiveTime() {
	return value();
}

//Returns the amount of time left
int SongPositionSlider::getNegativeTime() {
	return duration-value();
}	

//actually seek somewhere in the song
void SongPositionSlider::setTimeFromSlider() {
	conn->playback.seekMs(value()*MAGFACTOR)(Xmms::bind(&DataBackend::scrapResult, conn));
}

//Sets the duration of the slider, this !!NEEDS!! to be called elsewhere
void SongPositionSlider::setDuration(int id) {
	MlibData * mlib=((MlibData *)conn->getDataBackendObject(DataBackend::MLIB));
	markers.clear();
	QTime tmp=QTime::fromString(mlib->getInfo(QString("time"),id).toString(),"mm:ss");
	if(!tmp.isValid())
		tmp=QTime::fromString(mlib->getInfo(QString("time"),id).toString(),"h:mm:ss");
	int ms=tmp.msec()+tmp.second()*1000+tmp.minute()*60*1000+tmp.hour()*60*60*1000;
	duration = ms/MAGFACTOR;
	if((mlib->getInfo(QString("url"),id).toString().toStdString()).find("file://") == std::string::npos){
 	curType = STREAM;
	emit timeChanged(-1);
	update();
	}
	else if(duration>=0){
	setRange(0,duration);
	songEmitted = false;
	curType = FILE;
	}
	else {
	curType = UNKNOWN;
	qDebug()<<"Could not retrieve duration of song from Server and is not a stream  [ERROR]";
	}

	currentID = id;
	QString temp = (mlib->getInfo(QString("cuepoints"),id)).toString();
	if(temp!="Unknown"){
	QStringList tmp = temp.split(",");
		for(int i =0;i<tmp.size();i++) {
		markers.insert(tmp.value(i).toInt());
		}
	}
}

//Sets the initial time of the slider, so if yous start the program when 
// the song is at time 500, the slider knows
void SongPositionSlider::setInitTime(int time) {
	this->time = time;
	setValue(this->time);
}

// This allows you to one click seek and drag seek as well
void SongPositionSlider::mousePressEvent(QMouseEvent * event) {
	time = value();
	timer.stop();
// 	std::cout<<"press"<<std::endl;
	QSlider::mousePressEvent(event);
	released = false;
}

// Allows one click seek
void SongPositionSlider::mouseReleaseEvent(QMouseEvent * event) {
	
	if(event->button()==Qt::LeftButton) {
		if(allowUpdates) {
		time = -1;
		setValue(QStyle::sliderValueFromPosition(0,duration,event->x(),width(),0));
		setTimeFromSlider();
	// 	std::cout<<"release"<<std::endl;
		released = true;
		}
		allowUpdates = true;
	}
	else if(event->button()==Qt::RightButton) {
		double percent = ((double)duration) / width();
		time = (int)((double)(event->x())*percent);
		bool temp = true;
	
		QMenu * menu = new QMenu();
		menu->setTitle("Cue Points");
			for(int i=time-CUEGRACE_DISTANCE;i<time+CUEGRACE_DISTANCE;i++) {
			if(markers.contains(i))
			temp = false;
			}
		if(temp)
		menu->addAction("Add Cue Point",this,SLOT(setMarker()));
		else
		menu->addAction("Remove Cue Point",this,SLOT(delMarker()));
		menu->popup(event->globalPos());
	}
	else
	event->ignore();
	update();
}

//Ensures that when your mosue leaves the slider, it stops sliding ...
void SongPositionSlider::mouseMoveEvent(QMouseEvent *event) {
	int x,y,w,h,gd;
	x = event->x();	y = event->y();
	w = width(); h = height();
	gd = GRACE_DISTANCE; //How far can you stray from the widget before you have 'left' the widget
		if(x + gd > 0 && x - gd < w && y + gd > 0 && y - gd < h) {
			if(!allowUpdates) allowUpdates = true;
			if(!isSliderDown()) setSliderDown(1);
		QSlider::mouseMoveEvent(event);
		}
		else if(allowUpdates){
		allowUpdates = false;
		setValue(time);
		setSliderDown(0);
		}
		else
		return;
}

void SongPositionSlider::paintEvent(QPaintEvent * event) {
	QPainter painter(this);
	if(duration!=0) {
	double percent = value()/(double)duration;
	int pxlBlack = (int)(width()*percent);
	if(curType != FILE)
	pxlBlack = 0;

	painter.fillRect(0,0,pxlBlack+2,20,QBrush(QColor("#696969")));
	painter.fillRect(pxlBlack,0,width()-(pxlBlack+2),20,QBrush(QColor("#A1A1A1")));
	
	QList<int> markerList = markers.values();
	int curMarker=0;
	QColor side;
	QColor center;
	QLinearGradient tmp;
		for(int i=0;i<markerList.size();i++) {
		percent = markerList.value(i)/(double)duration;
		curMarker = (int)(width()*percent);
			if(curMarker<pxlBlack) {
			center = QColor("#6EA0B4");
			side = QColor("#696969");
			}
			else {
			side = QColor("#A1A1A1");
			center = QColor("#507882");
			}
		tmp = QLinearGradient(QPointF(curMarker,10),QPointF(curMarker+5,10));
		tmp.setColorAt(0,side);
		tmp.setColorAt(.5,center);
		tmp.setColorAt(1,side);
// 		QColor(113,164,187,220)
		painter.fillRect(curMarker,0,5,20,QBrush(tmp));
		}
	}
}

void SongPositionSlider::setMarker(){
	QString tmp = QString::number(time);	
	markers.insert(time);
	sendToServer();
	update();
}

void SongPositionSlider::delMarker() {
	for(int i=time-CUEGRACE_DISTANCE;i<time+CUEGRACE_DISTANCE;i++) {
		if(markers.contains(i))
		markers.remove(i);
	}
	sendToServer();
	update();
}

void SongPositionSlider::sendToServer() {
	QStringList temp;
	QList<int> tmp = markers.values();
	for(int i=0;i<tmp.size();i++) {
	temp.append(QString::number(tmp.value(i)));
	}
	QString val = temp.join(",");
	conn->medialib.entryPropertySet(currentID,"cuepoints",val.toStdString(),"konfetka");
}

#endif
