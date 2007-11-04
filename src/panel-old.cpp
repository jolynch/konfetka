#ifndef PANEL_CPP
#define PANEL_CPP
#include "panel.h"

Panel::Panel(int x,int y,int width,int height,bool type,QWidget* c,
						QWidget* parent, Qt::WindowFlags f):QFrame(parent, f)
	{
	mlib=type;
	if(mlib)
		xVal=x+20;
	else
		xVal=x;
	yVal=y;
	child=c;
	layout=new QGridLayout();
	layout->addWidget(child,0,0);
	layout->setAlignment(Qt::AlignCenter);
	layout->setMargin(0);
	this->setLayout(layout);
	heightVal=height;
	this->setFixedHeight(heightVal);
	this->setFixedWidth(0);
	this->setAutoFillBackground(true);
	this->setBackgroundRole(QPalette::Window);
	this->setFrameStyle(QFrame::StyledPanel|QFrame::Sunken);
	animator=new QTimer();
	animator->setInterval(1);
	child->hide();
	}

/*void Panel::timerEvent ( QTimerEvent * event )
	{
	if(event->timerId()!=animatorId)return;
	if(amount<0)
		if(currentPos+amount<=destination)
			{
			if(mlib)
				{
				this->move(destination,yVal);
				this->setFixedWidth(xVal-destination);
				}
			else
				{
				this->move(xVal,yVal);
				this->setFixedWidth(destination);
				}
			child->show();
			this->show();
			QObject::killTimer(animatorId);
			return;
			}
	if(amount>0)
		if(currentPos+amount>=destination)
			{
			if(mlib)
				{
				this->move(destination,yVal);
				this->setFixedWidth(xVal-destination);
				}
			else
				{
				this->move(xVal,yVal);
				this->setFixedWidth(destination);
				}
			child->show();
			this->show();
			QObject::killTimer(animatorId);
			return;
			}
	this->move(currentPos,yVal);
	currentPos+=amount;
	}*/

void Panel::moveHandle()
	{
	if(amount<0)
		if(currentPos+amount<=destination)
			{
			if(mlib)
				{
				this->move(destination,yVal);
				this->setFixedWidth(xVal-destination);
				}
			else
				{
				this->move(xVal,yVal);
				this->setFixedWidth(destination);
				}
			child->show();
			this->show();
			animator->disconnect();
			animator->stop();
			return;
			}
	if(amount>0)
		if(currentPos+amount>=destination)
			{
			if(mlib)
				{
				this->move(destination,yVal);
				this->setFixedWidth(xVal-destination);
				}
			else
				{
				this->move(xVal,yVal);
				this->setFixedWidth(destination);
				}
			child->show();
			this->show();
			animator->disconnect();
			animator->stop();
			return;
			}
	this->move(currentPos,yVal);
	currentPos+=amount;
	}

void Panel::moveThis(int h, int d, int c, int a)
	{
	currentPos=c;
	destination=d;
	if(mlib)
		{
		currentPos+=20;
		destination+=20;
		}
	this->hide();
	amount=a;

//	animatorId=QObject::startTimer(1);
	QObject::connect(animator,SIGNAL(timeout()),this,SLOT(moveHandle()));
	child->hide();
	animator->start();
	}

#endif
