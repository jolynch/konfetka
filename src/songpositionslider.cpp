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
	connect(conn,SIGNAL(currentPlaytime(int)),this,SLOT(setInitTime(int)));
	connect(conn,SIGNAL(newSong(Xmms::PropDict)),this,SLOT(setDuration(Xmms::PropDict)));
	connect(this,SIGNAL(valueChanged(int)),this,SIGNAL(timeChanged(int)));
	connect(conn,SIGNAL(songPositionChanged(uint)), this, SLOT(handlePlaytimeSignal(uint)));
}

bool SongPositionSlider::handlePlaytimeSignal(uint newTime) {
	if(isSliderDown() && !released || duration==0) return true;
	newTime = newTime/MAGFACTOR;
	setValue(newTime);
	
	if(getNegativeTime()-2<0 && !songEmitted) {
	conn->emitSongAboutToChange();
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
void SongPositionSlider::setDuration(const Xmms::PropDict& info) {
	if(info.contains("duration")){
	duration = (info.get<int>("duration")) /MAGFACTOR;
	setRange(0,duration);
	songEmitted = false;
 	}
 	else {
 	qDebug()<<"Could not retrieve duration of song from Server ... DIE";
	}
}

//Sets the initial time of the slider, so if yous start the program when 
// the song is at time 500, the slider knows, also allows my little callback
void SongPositionSlider::setInitTime(int time) {
	this->time = time;
	setValue(this->time);
}

// This allows you to one click seek and drag seek as well
void SongPositionSlider::mousePressEvent(QMouseEvent * event) {
	time = value();
	timer.stop();
	std::cout<<"press"<<std::endl;
	//!FIX!// we need to make it so that if the mouse leaves the slider the slide stops and goes to time
	QSlider::mousePressEvent(event);
	released = false;
}

// Allows one click seek
void SongPositionSlider::mouseReleaseEvent(QMouseEvent * event) {
	if(allowUpdates) {
	time = -1;
	setValue(QStyle::sliderValueFromPosition(0,duration,event->x(),width(),0));
	setTimeFromSlider();
	std::cout<<"release"<<std::endl;
	released = true;
	}
	allowUpdates = true;
}

//Ensures that when your mosue leaves the slider, it stops sliding ...
void SongPositionSlider::mouseMoveEvent(QMouseEvent *event) {
	int x,y,w,h,gd;
	x = event->x();	y = event->y();
	w = width(); h = height();
	gd = GRACE_DISTANCE;
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

#endif
