#ifndef SONGPOSITIONSLIDER_CPP
#define SONGPOSITIONSLIDER_CPP
#include "songpositionslider.h"

/******** SongPositionSlider - Allows one to create a functioning seekBar by just constructing the object *****/
SongPositionSlider::SongPositionSlider(DataBackend * c,Qt::Orientation o, QWidget* p):QSlider(o,p) {
	conn = c;
	setDuration(0);
	time = -1;
	hasTime = false;
	songEmitted = false;
	
	connect(conn,SIGNAL(currentPlaytime(int)),this,SLOT(setInitTime(int)));
	connect(conn,SIGNAL(changeStatus(Xmms::Playback::Status)),this,SLOT(setNewStatus(Xmms::Playback::Status)));
	connect(&timer,SIGNAL(timeout()),this,SLOT(timerEvent()));
	connect(this,SIGNAL(valueChanged(int)),this,SIGNAL(timeChanged(int)));
	
	//This allows all seek bars to know if each other changed something
	connect(this,SIGNAL(syncTime(int,SongPositionSlider*)),conn,SIGNAL(syncTime(int,SongPositionSlider*)));
	connect(conn,SIGNAL(syncTime(int,SongPositionSlider*)),this,SLOT(syncTimeFromOtherSliders(int,SongPositionSlider*)));	

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
	return duration/1000-value();
}	

//actually seek somewhere in the song
void SongPositionSlider::setTimeFromSlider() {
	emit syncTime(value(),this);
	conn->playback.seekMs(value()*1000)(Xmms::bind(&DataBackend::scrapResult, conn));
}

//this takes care of the slider itself, moving it along as it plays
//Also this interprets the state of the server to tell what it should be doing
void SongPositionSlider::timerEvent() {
	switch(status) {
		case XMMS_PLAYBACK_STATUS_PLAY: { 
		int ms = value()*1000;
		ms+=1000;
		setValue(ms/1000);	
		break;
		}
		case XMMS_PLAYBACK_STATUS_STOP: {
		timer.stop();
		setValue(0);	
		break;
		}
		case XMMS_PLAYBACK_STATUS_PAUSE: {
		timer.stop();
		break;
		}
		default: {
		return;
		}
	}
	//Tell everyone that this current song is about to end, but only once
	if(getNegativeTime()-1<0 && !songEmitted) {
	conn->emitSongAboutToChange();
	songEmitted = true;
	}
}

//Sets the duration of the slider, this !!NEEDS!! to be called elsewhere
void SongPositionSlider::setDuration(int newDuration) {
	duration = newDuration;
	setRange(0,duration/1000);
	if(hasTime) {
	setValue(time);
	hasTime = false;
	}
	timer.start(1000);	
	songEmitted = false;
}

//Sets the initial time of the slider, so if yous start the program when 
// the song is at time 500, the slider knows, also allows my little callback
void SongPositionSlider::setInitTime(int time) {
	//Because we have a callback with conn, this might get called by itself, 
	//EX: it would change to 100 and then call setInitTime 100, which is wrong...so we leave a little buffer
	this->time = time/1000;
	setValue(this->time);
	hasTime = true;
	timer.start(1000);
}

//Something has changed in the server, let us know
void SongPositionSlider::setNewStatus(Xmms::Playback::Status newStatus) {
	status=newStatus;
}

//This allows you to one click seek and drag seek as well
void SongPositionSlider::mousePressEvent(QMouseEvent * event) {
	time = value();
	timer.stop();
	std::cout<<"press"<<std::endl;
	//!FIX!// we need to make it so that if the mouse leaves the slider the slide stops and goes to time
	QSlider::mousePressEvent(event);
}

//Allows one click seek
void SongPositionSlider::mouseReleaseEvent(QMouseEvent * event) {
	time = -1;
	setValue(QStyle::sliderValueFromPosition(0,duration/1000,event->x(),width(),0));
	setTimeFromSlider();
	timer.start(1000);
	std::cout<<"release"<<std::endl;
}

void SongPositionSlider::leaveEvent (QEvent * event) {
	if(isSliderDown()) {
	setValue(time);
	setTimeFromSlider();
	triggerAction(QAbstractSlider::SliderNoAction);
	setRepeatAction(QAbstractSlider::SliderNoAction);
	setSliderDown(false);
	}
}

void SongPositionSlider::syncTimeFromOtherSliders(int newTime,SongPositionSlider* slider) {
	if(slider!=this) {
	std::cout<<"syncing"<<std::endl;
	setInitTime(1000*newTime);
	}
}

#endif
