#ifndef HANDLE_CPP
#define HANDLE_CPP
#include "handle.h"
Handle::Handle(QString image,  QWidget * parent, Qt::WindowFlags f):QLabel(parent, f)
	{
	this->setPixmap(*(new QPixmap(image)));
	this->setMargin(0);
	locked=false;
	animator=new QTimer();
	animator->setInterval(1);
	}

void Handle::lock()
	{locked=true;}

void Handle::unlock()
	{locked=false;}

void Handle::moveHandle()
	{
	if(amount<0)
		if(currentPos+amount<=destination)
			{
			this->move(destination,height);
			animator->disconnect();
			animator->stop();
			emit(done());
			unlock();
			return;
			}
	if(amount>0)
		if(currentPos+amount>=destination)
			{
			this->move(destination,height);
			animator->disconnect();
			animator->stop();
			unlock();
			emit(done());
			return;
			}
	this->move(currentPos+amount,height);
	currentPos+=amount;
	}

void Handle::moveThis(int h, int d, int c, int a)
	{
	lock();
	currentPos=c;
	destination=d;
	height=h;
	amount=a;
	QObject::connect(animator,SIGNAL(timeout()),this,SLOT(moveHandle()));
	animator->start();
	}

void Handle::mousePressEvent(QMouseEvent* event)
	{
	if(locked)
		return;
	emit(clicked());
	}
#endif
