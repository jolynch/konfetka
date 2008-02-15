#ifndef NICELABEL_CPP
#define NICELABEL_CPP
#include "mainbar.h"

/**************************NiceLabel**************************************/
NiceLabel::NiceLabel(int speed,QWidget * parent, Qt::WindowFlags f):QLabel(parent,f) {
	pressed = false;
	ms = speed;
	pressPos = 0;
}

void NiceLabel::slotScroll(int amount) {
	x += amount;
	update();
}

void NiceLabel::mousePressEvent(QMouseEvent * event) {
	pressed = true;
	pressPos = event->x();
	killTimer(id);
}

void NiceLabel::mouseReleaseEvent(QMouseEvent * event) {
	pressed = false;
	id = startTimer(ms);
}

void NiceLabel::mouseMoveEvent(QMouseEvent * event){
	if(pressed) {
		slotScroll(event->x()-pressPos);
		pressPos = event->x();
	}
}

void NiceLabel::resizeEvent(QResizeEvent * event) {
	x = rect().x();
}

void NiceLabel::paintEvent(QPaintEvent * event) {
	QString strToPaint = text() + "  ***  ";
	QPainter painter(this);
	painter.setPen(QColor(90,130,150,255));
	int widthOfText = fontMetrics().width(strToPaint);
	if(abs(x)>widthOfText) x = 0;
		//Paint the text enough to get the complete scroll feel
	for(int i = x; i<width(); i+=widthOfText) {
		painter.drawText(i,0,widthOfText,height(),Qt::AlignLeft | Qt::AlignVCenter,strToPaint);
		//If we go too far right make sure the text wraps
		if(i>10 && (i + widthOfText) >= width()) {
		painter.drawText(-1*(widthOfText-x),0,widthOfText,height(),Qt::AlignLeft | Qt::AlignVCenter,strToPaint);
		}
	}
}

void NiceLabel::timerEvent(QTimerEvent* event) {
	if(event->timerId()==id) {
		slotScroll();
	}
	else
	QLabel::timerEvent(event);
}

void NiceLabel::showEvent(QShowEvent * event) {
	id = startTimer(ms);
}

void NiceLabel::hideEvent(QHideEvent * event) {
	killTimer(id);
}

#endif

